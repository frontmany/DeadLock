#include "net_filesSender.h"


namespace net
{
	FilesSender::FilesSender(asio::io_context& asioContext,
		asio::ip::tcp::socket& socket,
		std::function<void(const File&, uint32_t)> onSendProgress,
		std::function<void(std::error_code, File)> onSendError,
		std::function<void()> onAllFilesSent)
		: m_socket(socket),
		m_onSendProgressUpdate(onSendProgress),
		m_onSendError(onSendError),
		m_asioContext(asioContext),
		m_onAllFilesSent(onAllFilesSent)
	{
		m_totalBytesSent = 0;
	}

	void FilesSender::sendFile(const File& file) {
		asio::post(m_asioContext, [this, file]() {
			bool isSendingAllowed = m_outgoingFilesQueue.empty();
			m_outgoingFilesQueue.push_back(file);
			if (isSendingAllowed) {
				m_file = m_outgoingFilesQueue.front();
				sendMetadata();
			}
			});
	}

	void FilesSender::sendMetadata() {
		utility::generateAESKey(m_sessionKey);
		std::string encryptedKey = utility::RSAEncryptKey(m_file.friendPublicKey, m_sessionKey);

		std::ostringstream oss;
		oss << encryptedKey << '\n'
			<< m_file.id << '\n'
			<< m_file.blobUID << "\n"
			<< m_file.receiverLoginHash << '\n'
			<< m_file.senderLoginHash << '\n'
			<< m_file.fileSize << '\n'
			<< utility::AESEncrypt(m_sessionKey, m_file.fileName) << '\n'
			<< utility::AESEncrypt(m_sessionKey, m_file.timestamp) << '\n'
			<< m_file.filesInBlobCount << '\n';

		if (m_file.caption != "") {
			oss << utility::AESEncrypt(m_sessionKey, m_file.caption) << '\n';
		}



		m_metadataMessage.header.type = static_cast<uint32_t>(QueryType::PREPARE_TO_RECEIVE_FILE);
		m_metadataMessage << oss.str();
		m_metadataMessage.header.size = m_metadataMessage.size();

		asio::async_write(
			m_socket,
			asio::buffer(&m_metadataMessage.header, sizeof(MessageHeader)),
			[this](std::error_code ec, std::size_t length) {
				if (ec)
				{
					m_onSendError(ec, m_outgoingFilesQueue.pop_front());
				}
				else
				{
					asio::async_write(
						m_socket,
						asio::buffer(m_metadataMessage.body.data(), m_metadataMessage.body.size()),
						[this](std::error_code ec, std::size_t length)
						{
							if (ec) {
								m_onSendError(ec, m_outgoingFilesQueue.pop_front());
							}
							else {
								sendFileChunk();
							}
						}
					);
				}
			}
		);
	}

	void FilesSender::sendFileChunk() {
		if (!m_fileStream.is_open()) {
			bool isOpen = openFile();
			if (!isOpen)
				return;
		}

		m_fileStream.read(m_readBuffer.data(), c_readChunkSize);
		std::streamsize bytesRead = m_fileStream.gcount();

		if (bytesRead > 0) {
			m_encryptedBuffer = utility::AESEncrypt(m_sessionKey, m_readBuffer);

			asio::async_write(
				m_socket,
				asio::buffer(m_encryptedBuffer.data(), c_encryptedOutputChunkSize),
				[this](std::error_code ec, std::size_t) {
					if (ec) {
						m_onSendError(ec, m_outgoingFilesQueue.front());
					}
					else {
						m_totalBytesSent += c_readChunkSize;
						if (m_totalBytesSent < std::stoull(m_file.fileSize)) {
							const auto& frontFile = m_outgoingFilesQueue.front();

							const uint64_t fileSize = std::stoull(frontFile.fileSize);
							uint32_t progress_percent = static_cast<uint32_t>(std::min<uint64_t>((m_totalBytesSent * 100) / fileSize, 100));
							m_onSendProgressUpdate(frontFile, progress_percent);
						}
						sendFileChunk();
					}
				}
			);
		}
		else {
			m_totalBytesSent = 0;
			m_fileStream.close();
			m_metadataMessage = net::Message{};
			m_file = File{};
			m_sessionKey = CryptoPP::SecByteBlock{};

			m_onSendProgressUpdate(m_outgoingFilesQueue.front(), 100);

			m_outgoingFilesQueue.pop_front();
			if (!m_outgoingFilesQueue.empty())
			{
				m_file = m_outgoingFilesQueue.front();
				sendMetadata();
			}
			else {
				m_onAllFilesSent();
			}
		}
	}

	bool FilesSender::openFile() {
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
				return false;
			}

		}
		catch (const std::exception& e) {
			std::cerr << "Error opening file: " << e.what() << '\n';
			return false;
		}
	}
}