#include "net_filesReceiver.h"
#include "json.hpp"
#include "database.h"

#include <fstream>


namespace net
{
	FilesReceiver::FilesReceiver(SafeDeque<BlobPtr>& incomingBlobsQueue,
		asio::ip::tcp::socket& socket,
		Database& db,
		std::function<void(AvatarInfo&&)> onAvatar,
		std::function<void(FileLocationInfo&&, uint32_t)> onRequestedFileProgressUpdate,
		std::function<void(uint32_t)> onDeadlockNewVersionProgressUpdate,
		std::function<void(std::error_code, FileLocationInfo&&)> onReceiveRequestedFileError)
		: m_myPrivateKey(nullptr),
		m_incomingBlobsQueue(incomingBlobsQueue),
		m_socket(socket),
		m_db(db),
		m_onAvatar(onAvatar),
		m_onRequestedFileProgressUpdate(onRequestedFileProgressUpdate),
		m_onDeadlockNewVersionProgressUpdate(onDeadlockNewVersionProgressUpdate),
		m_onReceiveRequestedFileError(onReceiveRequestedFileError)
	{
		m_lastChunkSize = 0;
		m_currentChunksCount = 0;
		m_expectedChunksCount = 0;
		m_totalReceivedBytes = 0;
	}

	void FilesReceiver::startReceiving() {
		readMetadataHeader();
	}

	void FilesReceiver::setMyPrivateKey(const CryptoPP::RSA::PrivateKey& myPrivateKey) {
		m_myPrivateKey = &myPrivateKey;
	}

	void FilesReceiver::readMetadataHeader() {
		asio::async_read(m_socket, asio::buffer(&m_metadataPacket.header_mut(), Packet::sizeOfHeader()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::connection_reset && ec != asio::error::operation_aborted) {
						processError(ec);
						reset();
					}
				}
				else {
					m_metadataPacket.body_mut().resize(m_metadataPacket.size() - Packet::sizeOfHeader());
					readMetadataBody();
				}
			});
	}


	void FilesReceiver::readMetadataBody() {
		asio::async_read(m_socket, asio::buffer(m_metadataPacket.body_mut().data(), m_metadataPacket.body().size()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::connection_reset && ec != asio::error::operation_aborted) {
						processError(ec);
						reset();
					}
				}
				else {
					if (m_metadataPacket.type() == PacketType::PREPARE_TO_RECEIVE_FILE) 
					{
						std::string path = parseFileMetadata();
						openFile(path);
						readChunk();
					}
					else if (m_metadataPacket.type() == PacketType::FRIEND_AVATAR || m_metadataPacket.type() == PacketType::FRIEND_AVATAR_FOR_PREVIEW)
					{
						std::string path = parseAvatarMetadata();
						openFile(path);
						readChunkWithoutDecryption();
					}
					else if (m_metadataPacket.type() == PacketType::DEADLOCK_UPDATE) 
					{
						std::string path = parseDeadlockUpdateMetadata();
						openFile(path);
						readChunkWithoutDecryption();
					}
				}
			});
	}

	void FilesReceiver::readChunk() {
		asio::async_read(m_socket,
			asio::buffer(m_receiveBuffer.data(), c_receivedChunkSize),
			[this](std::error_code ec, std::size_t bytesTransferred) {
				if (ec) {
					removePartiallyDownloadedFile();
					if (ec != asio::error::connection_reset && ec != asio::error::operation_aborted) {
						processError(ec);
						reset();
					}
				}
				else {
					m_currentChunksCount++;
					if (m_currentChunksCount < m_expectedChunksCount) {
						std::array<char, c_decryptedChunkSize> decryptedChunk = utility::AESDecrypt(m_sessionKey, m_receiveBuffer);
						m_fileStream.write(decryptedChunk.data(), c_decryptedChunkSize);
						m_totalReceivedBytes += bytesTransferred - c_overhead;
						
						readChunk();
						notifyReceiveRequestedFileProgress();
					}
					else if (m_currentChunksCount == m_expectedChunksCount) {
						std::array<char, c_decryptedChunkSize> decryptedChunk = utility::AESDecrypt(m_sessionKey, m_receiveBuffer);
						m_fileStream.write(decryptedChunk.data(), m_lastChunkSize);
						
						finalizeReceiving();
						reset();
					}
				}
			});
	}

	void FilesReceiver::readChunkWithoutDecryption() {
		asio::async_read(m_socket,
			asio::buffer(m_receiveBuffer.data(), c_receivedChunkSize),
			[this](std::error_code ec, std::size_t bytesTransferred) {
				if (ec) {
					removePartiallyDownloadedFile();
					reset();
				}
				else {
					m_currentChunksCount++;
					if (m_currentChunksCount < m_expectedChunksCount) {
						m_fileStream.write(m_receiveBuffer.data(), c_receivedChunkSize);
						readChunkWithoutDecryption();
					}
					else if (m_currentChunksCount == m_expectedChunksCount) {
						m_fileStream.write(m_receiveBuffer.data(), m_lastChunkSize);
						finalizeReceiving();
						reset();
					}
				}
			});
	}

	std::string FilesReceiver::parseAvatarMetadata() {
		try {
			nlohmann::json jsonObject = nlohmann::json::parse(m_metadataPacket.get());

			std::string encryptedKey = jsonObject[ENCRYPTED_KEY].get<std::string>();
			m_sessionKey = utility::RSADecryptKey(*m_myPrivateKey, encryptedKey);

			std::string senderUID = jsonObject[SENDER_UID].get<std::string>();
			std::string fileSize = jsonObject[FILE_SIZE].get<std::string>();

			m_avatarInfo.friendUID = senderUID;

			std::string filePath;
			if (m_metadataPacket.type() == PacketType::FRIEND_AVATAR) {
				filePath = utility::getConfigsAndPhotosDirectory() + "/" + senderUID + ".dph";
			}
			else if (m_metadataPacket.type() == PacketType::FRIEND_AVATAR_FOR_PREVIEW) {
				filePath = utility::getAvatarPreviewsDirectory() + "/" + senderUID + ".dph";
			}

			m_expectedChunksCount = static_cast<int>(std::ceil(static_cast<double>(std::stoi(fileSize)) / c_receivedChunkSize));
			int lastChunksSize = std::stoi(fileSize) - (m_expectedChunksCount * c_receivedChunkSize);
			if (lastChunksSize == 0) {
				m_lastChunkSize = c_receivedChunkSize;
			}
			else {
				m_lastChunkSize = lastChunksSize + c_receivedChunkSize;
			}

			return filePath;
		}
		catch (const std::exception& e) {
			std::cout << "Failed to parse avatar metadata: " + std::string(e.what());
		}
	}

	std::string FilesReceiver::parseFileMetadata() {
		nlohmann::json jsonObject(m_metadataPacket.get());

		try {
			nlohmann::json jsonObject = nlohmann::json::parse(m_metadataPacket.get());

			std::string encryptedKey = jsonObject[ENCRYPTED_KEY].get<std::string>();
			m_sessionKey = utility::RSADecryptKey(*m_myPrivateKey, encryptedKey);

			std::string fileId = jsonObject[FILE_ID].get<std::string>();
			std::string blobId = jsonObject[BLOB_ID].get<std::string>();
			std::string senderUID = jsonObject[MY_UID].get<std::string>();
			std::string fileSize = jsonObject[FILE_SIZE].get<std::string>();

			std::string encryptedFileName = jsonObject[FILE_NAME].get<std::string>();
			std::string fileName = utility::AESDecrypt(m_sessionKey, encryptedFileName);

			std::string encryptedTimestamp = jsonObject[TIMESTAMP].get<std::string>();
			std::string timestamp = utility::AESDecrypt(m_sessionKey, encryptedTimestamp);

			std::string encryptedFilesCount = jsonObject[FILES_COUNT_IN_BLOB].get<std::string>();
			std::string filesCountInBlob = utility::AESDecrypt(m_sessionKey, encryptedFilesCount);

			m_fileDataTemporaryContainer.blobId = blobId;
			m_fileDataTemporaryContainer.fileId = fileId;
			m_fileDataTemporaryContainer.fileName = fileName;
			m_fileDataTemporaryContainer.friendUID = senderUID;
			m_fileDataTemporaryContainer.fileSize = std::stoi(fileSize);
			m_fileDataTemporaryContainer.filePath = utility::getFileSavePath(fileName);

			m_fileLocationInfoForCallback.blobId = blobId;
			m_fileLocationInfoForCallback.fileId = fileId;
			m_fileLocationInfoForCallback.friendUID = senderUID;

			if (jsonObject.contains(CAPTION)) {
				std::string encryptedCaption = jsonObject[CAPTION].get<std::string>();
				std::string caption = utility::AESDecrypt(m_sessionKey, encryptedFilesCount);

				if (!m_db.isBlobBuffer(blobId)) {
					m_db.addBlobBuffer(blobId, senderUID, timestamp, std::stoi(filesCountInBlob), caption);
				}
			}
			else {
				if (!m_db.isBlobBuffer(blobId)) {
					m_db.addBlobBuffer(blobId, senderUID, timestamp, std::stoi(filesCountInBlob), std::nullopt);
				}
			}

			m_expectedChunksCount = static_cast<int>(std::ceil(static_cast<double>(std::stoi(fileSize)) / c_decryptedChunkSize));
			int lastChunksSize = std::stoi(fileSize) - (m_expectedChunksCount * c_decryptedChunkSize);
			if (lastChunksSize == 0) {
				m_lastChunkSize = c_receivedChunkSize;
			}
			else {
				m_lastChunkSize = lastChunksSize + c_decryptedChunkSize;
				m_lastChunkSize += c_overhead;
			}

			return m_fileDataTemporaryContainer.filePath;
		}
		catch (const std::exception& e) {
			std::cout << "Failed to parse file metadata: " + std::string(e.what());
		}
	}

	std::string FilesReceiver::parseDeadlockUpdateMetadata() {
		nlohmann::json jsonObject = nlohmann::json::parse(m_metadataPacket.get());
		std::string fileSize = jsonObject[FILE_SIZE].get<std::string>();
		std::string fileName = jsonObject[FILE_NAME].get<std::string>();

		m_expectedChunksCount = static_cast<int>(std::ceil(static_cast<double>(std::stoi(fileSize)) / c_receivedChunkSize));
		int lastChunksSize = std::stoi(fileSize) - (m_expectedChunksCount * c_receivedChunkSize);
		if (lastChunksSize == 0) {
			m_lastChunkSize = c_receivedChunkSize;
		}
		else {
			m_lastChunkSize = lastChunksSize + c_receivedChunkSize;
		}

		return utility::getUpdateTemporaryPath(fileName);
	}

	void FilesReceiver::processError(std::error_code ec) {
		if (m_db.isRequestedFileId(m_fileDataTemporaryContainer.fileId)) {
			m_onReceiveRequestedFileError(ec, std::move(m_fileLocationInfoForCallback));
		}
	}

	void FilesReceiver::notifyReceiveRequestedFileProgress(bool hundredPercentReceived) {
		if (m_db.isRequestedFileId(m_fileDataTemporaryContainer.fileId)) {
			if (hundredPercentReceived) {
				m_onRequestedFileProgressUpdate(std::move(m_fileLocationInfoForCallback), 100);
			}

			if (m_totalReceivedBytes < m_fileDataTemporaryContainer.fileSize) {
				const uint32_t fileSize = m_fileDataTemporaryContainer.fileSize;
				const uint32_t progress = std::min<uint32_t>((m_totalReceivedBytes * 100) / fileSize, 100);
				m_onRequestedFileProgressUpdate(std::move(m_fileLocationInfoForCallback), progress);
			}
		}
	}

	void FilesReceiver::finalizeReceiving() {
		m_fileStream.close();

		if (m_file.senderLoginHash != "server" && !m_file.isAvatar)
			m_onProgressUpdate(m_file, 100);

		m_lastChunkSize = 0;
		m_currentChunksCount = 0;
		m_expectedChunksCount = 0;
		m_totalReceivedBytes = 0;

		m_incomingFilesQueue.push_back(m_file);
		m_file = File();

		readMetadataHeader();
	}

	void FilesReceiver::removePartiallyDownloadedFile() {
		std::string path = m_file.filePath;

		if (path.empty()) {
			return;
		}

		std::error_code ec;
		bool removed = std::filesystem::remove(path, ec);

		if (ec) {
			std::cerr << "Failed to delete " << path << ": " << ec.message() << "\n";
		}
	}

	void FilesReceiver::openFile(const std::string& path) {
		try {
			std::filesystem::path filePath;

#ifdef _WIN32
			filePath = std::filesystem::u8path(path);
#else
			filePath = path;
#endif
			m_fileStream.open(filePath, std::ios::binary | std::ios::trunc);

			if (!m_fileStream.is_open()) {
				std::cerr << "Failed to open file: " << filePath.string() << '\n';
				return;
			}

		}
		catch (const std::exception& e) {
			std::cerr << "Error opening file: " << e.what() << '\n';
			return;
		}
	}

	void FilesReceiver::reset() {

	}
}

