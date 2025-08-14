#pragma once
#include "net_common.h"
#include "net_safeDeque.h"
#include "net_message.h"
#include "net_file.h"
#include "queryType.h"
#include "utility.h"

namespace net 
{
	class FilesSender {
	public:
		FilesSender(asio::io_context& asioContext,
			asio::ip::tcp::socket& socket,
			std::function<void(const File&, uint32_t)> onSendProgress,
			std::function<void(std::error_code, File)> onSendError,
			std::function<void()> onAllFilesSent
		);

		void sendFile(const File& file);

	private:
		void sendMetadata();
		void sendFileChunkWithoutEncryption();
		void sendFileChunk();
		bool openFile();

	private:
		static constexpr uint32_t c_readChunkSize = 8192;
		static constexpr uint32_t c_encryptedOutputChunkSize = 8220;

		asio::ip::tcp::socket& m_socket;
		SafeDeque<File>	m_outgoingFilesQueue;

		std::array<char, c_readChunkSize> m_readBuffer{};
		std::array<char, c_encryptedOutputChunkSize> m_encryptedBuffer{};
		std::ifstream m_fileStream;
		uint64_t m_totalBytesSent;
		CryptoPP::SecByteBlock m_sessionKey;
		net::Message m_metadataMessage;
		File m_file;

		std::function<void(const File&, uint32_t)> m_onSendProgressUpdate;
		std::function<void(std::error_code, File)> m_onSendError;
		std::function<void()> m_onAllFilesSent;
		asio::io_context& m_asioContext;
	};
}