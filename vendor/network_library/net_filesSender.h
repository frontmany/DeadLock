#pragma once
#include "net_common.h"
#include "net_safe_deque.h"
#include "net_message.h"
#include "net_file.h"
#include "queryType.h"
#include "utility.h"

namespace net 
{
	template <typename T>
	class filesSender {
	public:
		filesSender(asio::io_context& asioContext, asio::ip::tcp::socket& socket, std::function<void(const net::file<T>&, uint32_t)> onSendProgress, std::function<void(std::error_code, net::file<T>)> onSendError, std::function<void()> disconnect)
			: m_socket(socket), m_onSendProgressUpdate(onSendProgress), m_onSendError(onSendError), m_disconnect(disconnect), m_asioContext(asioContext)
		{
			m_totalBytesSent = 0;
		}

		void sendFile(const net::file<T>& file) {
			asio::post(m_asioContext, [this, file]() {
				bool isSendingAllowed = m_outgoingFilesQueue.empty();
				m_outgoingFilesQueue.push_back(file);
				if (isSendingAllowed) {
					m_file = m_outgoingFilesQueue.front();
					sendMetadata();
				}
			});
		}

	private:
		void sendMetadata() {
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
				<< utility::AESEncrypt(m_sessionKey, m_file.timestamp) << '\n';
			
			if (m_file.caption != "") {
			oss << utility::AESEncrypt(m_sessionKey, m_file.caption) << '\n';
			}
			else {
				oss << "" << '\n';
			}

			oss << m_file.filesInBlobCount;

			m_metadataMessage.header.type = QueryType::PREPARE_TO_RECEIVE_FILE;
			m_metadataMessage << oss.str();
			m_metadataMessage.header.size = m_metadataMessage.size();

			asio::async_write(
				m_socket,
				asio::buffer(&m_metadataMessage.header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length) {
					if (ec)
					{
						m_onSendError(ec, m_outgoingFilesQueue.pop_front());
						m_disconnect();
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

		void sendFileChunk() {
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
							m_disconnect();
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
				m_metadataMessage = message<T>{};
				m_file = file<T>{};
				m_sessionKey = CryptoPP::SecByteBlock{};

				m_onSendProgressUpdate(m_outgoingFilesQueue.front(), 100);
				
				m_outgoingFilesQueue.pop_front();
				if (!m_outgoingFilesQueue.empty())
				{
					m_file = m_outgoingFilesQueue.front();
					sendMetadata();
				}
			}
		}

		bool openFile() {
#ifdef _WIN32
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring widePath = converter.from_bytes(m_file.filePath);
			std::filesystem::path fsPath(widePath);
#else
			std::filesystem::path fsPath(m_file.filePath);
#endif

			m_fileStream.open(fsPath, std::ios::binary);

			if (m_fileStream) {
				return true;
			}
			else {
				return false;
			}
		}

	private:
		static constexpr uint32_t c_readChunkSize = 8192;
		static constexpr uint32_t c_encryptedOutputChunkSize = 8220;

		asio::ip::tcp::socket& m_socket;
		safe_deque<file<T>>	m_outgoingFilesQueue;

		std::array<char, c_readChunkSize> m_readBuffer{};
		std::array<char, c_encryptedOutputChunkSize> m_encryptedBuffer{};
		std::ifstream m_fileStream;
		uint64_t m_totalBytesSent;
		CryptoPP::SecByteBlock m_sessionKey;
		message<T>	m_metadataMessage;
		net::file<T> m_file;

		std::function<void(const net::file<T>&, uint32_t)> m_onSendProgressUpdate;
		std::function<void(std::error_code, net::file<T>)> m_onSendError;
		std::function<void()> m_disconnect;
		asio::io_context& m_asioContext;
	};
}