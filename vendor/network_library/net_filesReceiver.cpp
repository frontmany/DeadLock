#include "net_filesReceiver.h"

namespace net
{
	FilesReceiver::FilesReceiver(CryptoPP::RSA::PrivateKey* myPrivateKey,
		SafeDeque<File>& incomingFilesQueue,
		asio::ip::tcp::socket& socket,
		std::function<void(const File&, uint32_t)> onProgressUpdate,
		std::function<void(std::error_code, std::optional<File>)> onReceiveError)
		: m_myPrivateKey(myPrivateKey),
		m_incomingFilesQueue(incomingFilesQueue),
		m_socket(socket),
		m_onProgressUpdate(onProgressUpdate),
		m_onReceiveError(onReceiveError)
	{
		m_lastChunkSize = 0;
		m_currentChunksCount = 0;
		m_expectedChunksCount = 0;
		m_totalReceivedBytes = 0;
	}

	void FilesReceiver::startReceiving() {
		readMetadataHeader();
	}

	void FilesReceiver::readMetadataHeader() {
		asio::async_read(m_socket, asio::buffer(&m_metadataMessage.header, sizeof(MessageHeader)),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::connection_reset) {
						m_onReceiveError(ec, std::nullopt);
					}
				}
				else {
					m_metadataMessage.body.resize(m_metadataMessage.header.size - sizeof(MessageHeader));
					readMetadataBody();
				}
			});
	}


	void FilesReceiver::readMetadataBody() {
		asio::async_read(m_socket, asio::buffer(m_metadataMessage.body.data(), m_metadataMessage.body.size()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::connection_reset) {
						m_onReceiveError(ec, std::nullopt);
					}
				}
				else {
					if (m_metadataMessage.header.type == static_cast<uint32_t>(QueryType::AVATAR) ||
						m_metadataMessage.header.type == static_cast<uint32_t>(QueryType::AVATAR_FOR_PREVIEW)) {
						parseAvatarMetadata();
					}
					else {
						parseMetadata();
					}

					openFile();

					if (m_file.senderLoginHash == "server" || m_file.isAvatar) {
						readChunkWithoutDecryption();
					}
					else {
						readChunk();
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
					if (ec != asio::error::connection_reset) {
						m_onReceiveError(ec, m_file);
					}
					return;
				}
				else {
					m_currentChunksCount++;
					if (m_currentChunksCount < m_expectedChunksCount) {
						std::array<char, c_decryptedChunkSize> decryptedChunk = utility::AESDecrypt(m_sessionKey, m_receiveBuffer);
						m_fileStream.write(decryptedChunk.data(), c_decryptedChunkSize);
						m_totalReceivedBytes += bytesTransferred - c_overhead;
						readChunk();

						if (m_totalReceivedBytes < std::stoull(m_file.fileSize)) {
							const uint64_t fileSize = std::stoull(m_file.fileSize);
							const uint64_t progress = std::min<uint64_t>((m_totalReceivedBytes * 100) / fileSize, 100);
							m_onProgressUpdate(m_file, progress);
						}


					}
					else if (m_currentChunksCount == m_expectedChunksCount) {
						std::array<char, c_decryptedChunkSize> decryptedChunk = utility::AESDecrypt(m_sessionKey, m_receiveBuffer);
						m_fileStream.write(decryptedChunk.data(), m_lastChunkSize);
						finalizeReceiving();
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
					if (ec != asio::error::connection_reset) {
						m_onReceiveError(ec, m_file);
					}
					return;
				}
				else {
					m_currentChunksCount++;
					if (m_currentChunksCount < m_expectedChunksCount) {
						m_fileStream.write(m_receiveBuffer.data(), c_receivedChunkSize);
						m_totalReceivedBytes += bytesTransferred;
						readChunkWithoutDecryption();
					}
					else if (m_currentChunksCount == m_expectedChunksCount) {
						m_fileStream.write(m_receiveBuffer.data(), m_lastChunkSize);
						finalizeReceiving();
					}
				}
			});
	}

	void FilesReceiver::parseAvatarMetadata() {
		std::string metadataString;
		m_metadataMessage >> metadataString;
		std::istringstream iss(metadataString);

		std::string senderLoginHash;
		std::getline(iss, senderLoginHash);

		std::string fileSize;
		std::getline(iss, fileSize);

		m_file.fileSize = fileSize;
		m_file.senderLoginHash = senderLoginHash;
		m_file.isAvatar = true;

		if (m_metadataMessage.header.type == static_cast<uint32_t>(QueryType::AVATAR)) {
			m_file.filePath = utility::getConfigsAndPhotosDirectory() + "/" + senderLoginHash + ".dph";
		}
		else if (m_metadataMessage.header.type == static_cast<uint32_t>(QueryType::AVATAR_FOR_PREVIEW)) {
			m_file.filePath = utility::getAvatarPreviewsDirectory() + "/" + senderLoginHash + ".dph";
		}

		m_expectedChunksCount = static_cast<int>(std::ceil(static_cast<double>(std::stoi(m_file.fileSize)) / c_receivedChunkSize));
		int lastChunksSize = std::stoi(m_file.fileSize) - (m_expectedChunksCount * c_receivedChunkSize);
		if (lastChunksSize == 0) {
			m_lastChunkSize = c_receivedChunkSize;
		}
		else {
			m_lastChunkSize = lastChunksSize + c_receivedChunkSize;
		}
	}

	void FilesReceiver::parseMetadata() {
		std::string metadataString;
		m_metadataMessage >> metadataString;
		std::istringstream iss(metadataString);

		std::string encryptedKey;
		std::getline(iss, encryptedKey);
		m_sessionKey = utility::RSADecryptKey(*m_myPrivateKey, encryptedKey);

		std::string fileId;
		std::getline(iss, fileId);

		std::string blobUID;
		std::getline(iss, blobUID);

		std::string receiverLoginHash;
		std::getline(iss, receiverLoginHash);

		std::string senderLoginHash;
		std::getline(iss, senderLoginHash);

		std::string fileSize;
		std::getline(iss, fileSize);

		std::string fileName;
		std::getline(iss, fileName);
		if (senderLoginHash != "server") {
			fileName = utility::AESDecrypt(m_sessionKey, fileName);
		}

		std::string timestamp;
		std::getline(iss, timestamp);
		if (senderLoginHash != "server") {
			timestamp = utility::AESDecrypt(m_sessionKey, timestamp);
		}

		std::string filesCountInBlob;
		std::getline(iss, filesCountInBlob);

		std::string caption;
		std::getline(iss, caption);
		if (caption != "") {
			caption = utility::AESDecrypt(m_sessionKey, caption);
		}

		m_file.fileName = fileName;
		m_file.senderLoginHash = senderLoginHash;
		m_file.receiverLoginHash = receiverLoginHash;
		m_file.fileSize = fileSize;
		m_file.id = fileId;
		m_file.timestamp = timestamp;
		m_file.caption = caption;
		m_file.blobUID = blobUID;
		m_file.filesInBlobCount = filesCountInBlob;


		if (senderLoginHash == "server") {
			m_file.filePath = utility::getUpdateTemporaryPath(fileName);

			m_expectedChunksCount = static_cast<int>(std::ceil(static_cast<double>(std::stoi(m_file.fileSize)) / c_receivedChunkSize));
			int lastChunksSize = std::stoi(m_file.fileSize) - (m_expectedChunksCount * c_receivedChunkSize);
			if (lastChunksSize == 0) {
				m_lastChunkSize = c_receivedChunkSize;
			}
			else {
				m_lastChunkSize = lastChunksSize + c_receivedChunkSize;
			}
		}
		else {
			m_file.filePath = utility::getFileSavePath(fileName);

			m_expectedChunksCount = static_cast<int>(std::ceil(static_cast<double>(std::stoi(m_file.fileSize)) / c_decryptedChunkSize));
			int lastChunksSize = std::stoi(m_file.fileSize) - (m_expectedChunksCount * c_decryptedChunkSize);
			if (lastChunksSize == 0) {
				m_lastChunkSize = c_receivedChunkSize;
			}
			else {
				m_lastChunkSize = lastChunksSize + c_decryptedChunkSize;
				m_lastChunkSize += c_overhead;
			}
		}
	}

	void FilesReceiver::finalizeReceiving() {
		m_fileStream.close();

		if (m_file.senderLoginHash != "server")
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

	void FilesReceiver::openFile() {
		try {
			std::filesystem::path filePath;

#ifdef _WIN32
			filePath = std::filesystem::u8path(m_file.filePath);
#else
			filePath = m_file.filePath;
#endif
			m_fileStream.open(filePath, std::ios::binary);

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

}