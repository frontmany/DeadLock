#pragma once

#include "net_common.h"
#include "net_safe_deque.h"
#include "net_message.h"
#include "net_file.h"
#include "queryType.h"
#include "utility.h"

#include "secblock.h"

#include <fstream>              
#include <filesystem>          
#include <system_error>          
#include <string>              
#include <locale>                
#include <codecvt>   

namespace net {
	template<typename T>
	class files_connection : public std::enable_shared_from_this<files_connection<T>> {

		static const uint32_t c_chunkSizeToRead = 8192;
		static const uint32_t c_chunkSizeToTransfer = 8604;
		static const uint32_t c_encryptedChunkSize = 8220;
		static const uint32_t c_overhead = 412;
		static const uint32_t c_encryptedSessionKeySize = 384;
		static const uint32_t c_hundredPercent = 100;

	public:
		files_connection(
			owner owner,
			asio::io_context& asioContext,
			asio::ip::tcp::socket socket,
			safe_deque<owned_file<T>>& safeDequeIncomingFiles,
			CryptoPP::RSA::PrivateKey* privateKey,
			std::function<void(std::error_code, net::file<T>)> errorReceiveCallback,
			std::function<void(std::error_code, net::file<T>)> errorSendCallback,
			std::function<void(net::file<T>)> onFileSent,
			std::optional<std::function<void(const net::file<T>&, uint32_t)>> onSendProgressUpdate,
			std::optional<std::function<void(const net::file<T>&, uint32_t)>> onReceiveProgressUpdate)
			: m_asio_context(asioContext),
			m_socket(std::move(socket)),
			m_safe_deque_incoming_files(safeDequeIncomingFiles),
			m_my_private_key(privateKey),
			m_on_file_sent(std::move(onFileSent)),
			m_on_send_error(std::move(errorSendCallback)),
			m_on_receive_error(std::move(errorReceiveCallback)),
			m_on_send_progress_update(std::move(onSendProgressUpdate)),
			m_on_receive_progress_update(std::move(onReceiveProgressUpdate))
		{
			m_last_packet_size = 0;
			m_received_file_size = 0;
			m_curent_number_of_occurrences = 0;
			m_number_of_full_occurrences = 0;
			m_total_bytes_sent = 0;
			m_owner = owner;

			readPreviewHeader();
		}

		virtual ~files_connection() {}

		void sendFile(const net::file<T>& file) {
			asio::post(m_asio_context, [this, file]() {
				bool isAbleToWrite = m_safe_deque_outgoing_files.empty();
				m_safe_deque_outgoing_files.push_back(file);
				if (isAbleToWrite) {
					writeFirstFromQueueFileMetadata();
				}
			});
		}

		bool isConnected() const {
			return m_socket.is_open();
		}

		void disconnect() {
			if (m_receive_file_stream.is_open()) {
				m_receive_file_stream.close();
			}
			if (m_send_file_stream.is_open()) {
				m_send_file_stream.close();
			}

			if (isConnected()) {
				m_socket.close();
			}
		}

	private:

		void readPreviewHeader() {
			asio::async_read(m_socket, asio::buffer(&m_file_preview_message_tmp.header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_receive_error(ec, net::file<T>{});
						disconnect();
					}
					else {
						m_file_preview_message_tmp.body.resize(m_file_preview_message_tmp.header.size - sizeof(message_header<T>));
						readPreviewBody();
					}
				});
		}

		void readPreviewBody() {
			asio::async_read(m_socket, asio::buffer(m_file_preview_message_tmp.body.data(), m_file_preview_message_tmp.body.size()),
				[this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_receive_error(ec, net::file<T>{});
						disconnect();
					}
					else {
						extractFileMetadata();
						openFileForReceiving();
						readFileChunk();
					}
				});
		}

		void readFileChunk() {
			asio::async_read(m_socket,
				asio::buffer(m_receive_file_buffer.data(), c_chunkSizeToTransfer),
				[this](std::error_code ec, std::size_t bytes_transferred) {
					if (ec) {
						disconnect();
						removePartiallyDownloadedFile();
						if (m_on_receive_error) {
							m_on_receive_error(ec, m_file_tmp);
						}
						return;
					}
					else {
						m_received_file_size += bytes_transferred - c_overhead;
						m_curent_number_of_occurrences++;

						std::array<char, c_encryptedSessionKeySize> encryptedKey;
						std::copy_n(m_receive_file_buffer.begin(), c_encryptedSessionKeySize, encryptedKey.begin());
						CryptoPP::SecByteBlock key = utility::RSADecryptKey(
							*m_my_private_key,
							std::string(encryptedKey.begin(), encryptedKey.end())
						);

		
						std::array<char, c_chunkSizeToTransfer - c_encryptedSessionKeySize> remainingDataEncrupted;
						std::copy_n(m_receive_file_buffer.begin() + c_encryptedSessionKeySize,
							c_chunkSizeToTransfer - c_encryptedSessionKeySize,
							remainingDataEncrupted.begin());

						std::array<char, c_chunkSizeToRead> remainingData = utility::AESDecrypt(key, remainingDataEncrupted);

						if (std::stoi(m_file_tmp.fileSize) >= c_chunkSizeToRead) {
							if (m_curent_number_of_occurrences > m_number_of_full_occurrences) {
								m_receive_file_stream.write(remainingData.data(), m_last_packet_size);
							}
							else {
								m_receive_file_stream.write(remainingData.data(), c_chunkSizeToRead);
							}
						}
						else {
							m_receive_file_stream.write(remainingData.data(), m_last_packet_size);
						}

						if (m_received_file_size < std::stoi(m_file_tmp.fileSize)) {
							if (m_owner == owner::client) {
								double progress = (static_cast<double>(m_received_file_size) / std::stoi(m_file_tmp.fileSize)) * c_hundredPercent;
								progress = std::clamp(progress, 0.0, static_cast<double>(c_hundredPercent));
								(*m_on_receive_progress_update)(m_file_tmp, progress);	
							}
							readFileChunk();
						}
						else {
							finalizeFileReceiving();
						}
					}

				});
		}

		void writeFileChunk() {
			if (!m_send_file_stream.is_open()) {
				bool isOpen = openFirstFromQueueFileForSending();
				if (!isOpen)
					return;
			}

			m_send_file_stream.read(m_send_file_buffer.data(), c_chunkSizeToRead);
			std::streamsize bytesRead = m_send_file_stream.gcount();

			if (bytesRead > 0) {
				CryptoPP::SecByteBlock sessionKey;
				utility::generateAESKey(sessionKey);

				std::array<char, c_encryptedChunkSize> encryptedBuffer = utility::AESEncrypt(sessionKey, m_send_file_buffer);

				std::string encryptedKey = utility::RSAEncryptKey(m_safe_deque_outgoing_files.front().friendPublicKey, sessionKey);

				std::array<char, c_chunkSizeToTransfer> encryptedBufferWithKey;

				if (encryptedKey.size() > c_encryptedSessionKeySize) {
					throw std::runtime_error("Encrypted key is too large for the buffer");
				}

				std::memcpy(encryptedBufferWithKey.data(), encryptedKey.data(), encryptedKey.size());

				std::memcpy(encryptedBufferWithKey.data() + encryptedKey.size(),
					encryptedBuffer.data(),
					encryptedBuffer.size());

				asio::async_write(
					m_socket,
					asio::buffer(encryptedBufferWithKey.data(), c_chunkSizeToTransfer),
					[this](std::error_code ec, std::size_t) {
						if (ec) {
							if (m_on_send_error) {
								m_on_send_error(ec, m_safe_deque_outgoing_files.front());
							}
							disconnect();
						}
						else {
							m_total_bytes_sent += c_chunkSizeToRead;
							if (m_total_bytes_sent < std::stoi(m_safe_deque_outgoing_files.front().fileSize) && m_owner == owner::client) {
								if (m_on_send_progress_update.has_value()) {
									const auto& front_file = m_safe_deque_outgoing_files.front();

									uint32_t progress_percent = 0;
									if (std::stoi(front_file.fileSize) > 0) {
										double progress = (static_cast<double>(m_total_bytes_sent) / static_cast<double>(std::stoi(front_file.fileSize))) * c_hundredPercent;
										progress = std::clamp(progress, 0.0, static_cast<double>(c_hundredPercent));
										progress_percent = static_cast<uint32_t>(progress);
									}

									(*m_on_send_progress_update)(front_file, progress_percent);
								}
							}
							writeFileChunk();
						}
					}
				);
			}
			else {
				m_total_bytes_sent = 0;
				m_send_file_stream.close();
				m_msg_tmp_for_send_metadata = message<T>();

				if (m_owner == owner::client) {
					if (m_on_send_progress_update.has_value()) {
						(*m_on_send_progress_update)(
							m_safe_deque_outgoing_files.front(),
							static_cast<uint32_t>(c_hundredPercent)
							);
					}
				}
				m_on_file_sent(m_safe_deque_outgoing_files.pop_front());
				if (!m_safe_deque_outgoing_files.empty())
				{
					writeFirstFromQueueFileMetadata();
				}
			}
		}

		void writeFirstFromQueueFileMetadata() {
			const auto& file = m_safe_deque_outgoing_files.front();

			std::ostringstream oss;
			CryptoPP::SecByteBlock key;
			utility::generateAESKey(key);

			std::string encryptedKey = utility::RSAEncryptKey(file.friendPublicKey, key);

			oss << encryptedKey << '\n'
				<< file.id << '\n'
				<< file.blobUID << "\n"
				<< file.receiverLoginHash << '\n'
				<< file.senderLoginHash << '\n'
				<< file.fileSize << '\n'
				<< utility::AESEncrypt(key, file.fileName) << '\n'
				<< utility::AESEncrypt(key, file.timestamp) << '\n'
				<< utility::AESEncrypt(key, "MESSAGE_BEGIN") << '\n'
				<< utility::AESEncrypt(key, file.caption) << '\n'
				<< utility::AESEncrypt(key, "MESSAGE_END") << '\n'
				<< utility::AESEncrypt(key, file.filesInBlobCount);

			m_msg_tmp_for_send_metadata.header.type = QueryType::PREPARE_TO_RECEIVE_FILE;
			m_msg_tmp_for_send_metadata << oss.str();
			m_msg_tmp_for_send_metadata.header.size = m_msg_tmp_for_send_metadata.size();

			asio::async_write(
				m_socket,
				asio::buffer(&m_msg_tmp_for_send_metadata.header, sizeof(message_header<T>)),
				[this](std::error_code ec, std::size_t length) {
					if (ec)
					{
						disconnect();
						m_on_send_error(ec, m_safe_deque_outgoing_files.pop_front());
					}
					else
					{
						asio::async_write(
							m_socket,
							asio::buffer(m_msg_tmp_for_send_metadata.body.data(),
								m_msg_tmp_for_send_metadata.body.size()),
							[this](std::error_code ec, std::size_t length)
							{
								if (ec) {
									disconnect();
									m_on_send_error(ec, m_safe_deque_outgoing_files.pop_front());
								}
								else {
									writeFileChunk();
								}
							}
						);

					}

				}
			);
		}

		bool openFirstFromQueueFileForSending() {
			const auto& file = m_safe_deque_outgoing_files.front();

#ifdef _WIN32
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring widePath = converter.from_bytes(file.filePath);
			std::filesystem::path fsPath(widePath);
#else
			std::filesystem::path fsPath(file.filePath);
#endif

			m_send_file_stream.open(fsPath, std::ios::binary);

			if (m_send_file_stream.is_open()) {
				return true;
			}
			else {
				return false;
			}
		}

		void openFileForReceiving() {
			std::error_code ec;

#ifdef _WIN32
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring originalPath = converter.from_bytes(m_file_tmp.filePath);
			std::wstring newPath = originalPath;
#else
			std::string originalPath = m_file_tmp.filePath;
			std::string newPath = originalPath;
#endif
			m_receive_file_stream.open(newPath, std::ios::binary);
			if (!m_receive_file_stream.is_open()) {
				std::wcerr << "Failed to create file: " << newPath << "\n";
				return;
			}
			else {
				std::wcout << "File created: " << newPath << "\n";
			}
		}

		void extractFileMetadata() {
			std::string filePreviewString;
			m_file_preview_message_tmp >> filePreviewString;

			std::istringstream iss(filePreviewString);

			std::string encryptedKey;
			std::getline(iss, encryptedKey);
			CryptoPP::SecByteBlock key = utility::RSADecryptKey(*m_my_private_key, encryptedKey);

			std::string fileId;
			std::getline(iss, fileId);

			std::string blobUID;
			std::getline(iss, blobUID);

			std::string receiverLoginHash;
			std::getline(iss, receiverLoginHash);
			receiverLoginHash = utility::AESDecrypt(key, receiverLoginHash);

			std::string senderLoginHash;
			std::getline(iss, senderLoginHash);
			senderLoginHash = utility::AESDecrypt(key, senderLoginHash);

			std::string fileSize;
			std::getline(iss, fileSize);

			std::string fileName;
			std::getline(iss, fileName);
			fileName = utility::AESDecrypt(key, fileName);

			std::string fileTimestamp;
			std::getline(iss, fileTimestamp);
			fileTimestamp = utility::AESDecrypt(key, fileTimestamp);

			std::string messageBegin;
			std::getline(iss, messageBegin);
			messageBegin = utility::AESDecrypt(key, messageBegin);

			std::string caption;
			std::getline(iss, caption);
			messageBegin = utility::AESEncrypt(key, caption);

			std::string messageEnd;
			std::getline(iss, messageEnd);
			messageEnd = utility::AESDecrypt(key, messageEnd);

			std::string filesCountInBlobStr;
			std::getline(iss, filesCountInBlobStr);
			filesCountInBlobStr = utility::AESDecrypt(key, filesCountInBlobStr);
			size_t filesInBlobCount = std::stoi(filesCountInBlobStr);

			m_file_tmp.filePath = createFilePath(fileName, fileId);
			m_file_tmp.fileName = fileName;
			m_file_tmp.senderLoginHash = senderLoginHash;
			m_file_tmp.receiverLoginHash = receiverLoginHash;
			m_file_tmp.fileSize = fileSize;
			m_file_tmp.id = fileId;
			m_file_tmp.timestamp = fileTimestamp;
			m_file_tmp.caption = caption;
			m_file_tmp.blobUID = blobUID;
			m_file_tmp.filesInBlobCount = filesInBlobCount;

			m_number_of_full_occurrences = std::stoi(m_file_tmp.fileSize) / c_chunkSizeToRead;
			int lastPacketSize = std::stoi(m_file_tmp.fileSize) - (m_number_of_full_occurrences * c_chunkSizeToRead);
			if (lastPacketSize == 0) {
				m_last_packet_size = c_chunkSizeToRead;
			}
			else {
				m_last_packet_size = lastPacketSize;
			}

			m_last_packet_size += c_overhead;
		}

		void finalizeFileReceiving() {
			m_receive_file_stream.close();
			if (m_owner == owner::client) {
				if (m_on_receive_progress_update.has_value()) {
					(*m_on_receive_progress_update)(m_file_tmp, c_hundredPercent);
				}
			}
			m_last_packet_size = 0;
			m_received_file_size = 0;
			m_curent_number_of_occurrences = 0;
			m_number_of_full_occurrences = 0;
			m_total_bytes_sent = 0;

			queueReceivedFile();
			readPreviewHeader();
		}

		void queueReceivedFile() {
			if (m_owner == owner::server)
				m_safe_deque_incoming_files.push_back({ this->shared_from_this(), m_file_tmp });
			else
				m_safe_deque_incoming_files.push_back({ nullptr, m_file_tmp });

			m_file_tmp = file<T>();
		}

		std::string createFilePath(const std::string& fileName, const std::string& fileId) {
			if (m_owner == owner::server) {
				std::filesystem::create_directory("ReceivedFiles");
				const std::string filePath = "ReceivedFiles/" + fileId;

				size_t dotPos = fileName.find_last_of('.');
				std::string fullPath;
				if (dotPos != std::string::npos && dotPos + 1 < fileName.length()) {
					std::string extension = fileName.substr(dotPos + 1);
					fullPath = filePath + "." + extension;
				}

				return fullPath;
			}
			else {
				namespace fs = std::filesystem;

#ifdef _WIN32
				std::string downloadsPath = std::string(std::getenv("USERPROFILE")) + "\\Downloads\\";
#else
				std::string downloadsPath = std::string(std::getenv("HOME")) + "/Downloads/";
#endif

				std::string deadlockDir = downloadsPath + "Deadlock Messenger";
				if (!fs::exists(deadlockDir)) {
					fs::create_directory(deadlockDir);
				}

				std::string filePath = deadlockDir + "/" + fileName;
#ifdef _WIN32
				filePath = deadlockDir + "\\" + fileName;
#endif

				int counter = 1;
				while (fs::exists(filePath)) {
					size_t dotPos = fileName.find_last_of('.');
					std::string nameWithoutExt = fileName.substr(0, dotPos);
					std::string extension = (dotPos != std::string::npos) ? fileName.substr(dotPos) : "";

#ifdef _WIN32
					filePath = deadlockDir + "\\" + nameWithoutExt + " (" + std::to_string(counter) + ")" + extension;
#else
					filePath = deadlockDir + "/" + nameWithoutExt + " (" + std::to_string(counter) + ")" + extension;
#endif
					counter++;
				}

				return filePath;
			}

		}

		void removePartiallyDownloadedFile() {
			std::string path = m_file_tmp.filePath;

			if (path.empty()) {
				return;
			}

			std::error_code ec;
			bool removed = std::filesystem::remove(path, ec);

			if (ec) {
				std::cerr << "Failed to delete " << path << ": " << ec.message() << "\n";
			}
		}


	private:
		uint32_t					  m_total_bytes_sent;
		uint32_t					  m_received_file_size;
		uint32_t					  m_last_packet_size;
		uint32_t					  m_number_of_full_occurrences;
		uint32_t					  m_curent_number_of_occurrences;

		std::array<char, c_chunkSizeToRead> m_send_file_buffer{};
		std::ifstream				  m_send_file_stream;

		std::array<char, c_chunkSizeToTransfer> m_receive_file_buffer{};
		std::ofstream				  m_receive_file_stream;

		owner						  m_owner;
		asio::ip::tcp::socket		  m_socket;
		asio::io_context&			  m_asio_context;

		message<T>					  m_file_preview_message_tmp;
		message<T>					  m_msg_tmp_for_send_metadata;

		safe_deque<file<T>>			  m_safe_deque_outgoing_files;
		safe_deque<owned_file<T>>&	  m_safe_deque_incoming_files;
		file<T>						  m_file_tmp;

		CryptoPP::RSA::PrivateKey*	  m_my_private_key;

		// callbacks
		std::function<void(net::file<T>)> m_on_file_sent;
		std::function<void(std::error_code, net::file<T>)> m_on_send_error;
		std::function<void(std::error_code, net::file<T>)> m_on_receive_error;
		std::optional<std::function<void(const net::file<T>&, uint32_t)>> m_on_send_progress_update;
		std::optional<std::function<void(const net::file<T>&, uint32_t)>> m_on_receive_progress_update;
	};
}
