#pragma once
#include "net_common.h"
#include "net_safeDeque.h"
#include "net_message.h"
#include "net_file.h"
#include "queryType.h"
#include "utility.h"

namespace net
{
	class FilesReceiver {
	public:
		FilesReceiver(CryptoPP::RSA::PrivateKey* myPrivateKey,
			SafeDeque<File>& incomingFilesQueue,
			asio::ip::tcp::socket& socket,
			std::function<void(const File&, uint32_t)> onProgressUpdate,
			std::function<void(std::error_code, std::optional<File>)> onReceiveError
		);

		void startReceiving();

	private:
		void readMetadataHeader();
		void readMetadataBody();

		void readChunk();
		void readChunkWithoutDecryption();

		void parseMetadata();
		void finalizeReceiving();
		void removePartiallyDownloadedFile();
		void openFile();

	private:
		static constexpr uint32_t c_decryptedChunkSize = 8192;
		static constexpr uint32_t c_receivedChunkSize = 8220;
		static constexpr uint32_t c_overhead = 28;

		uint32_t m_lastChunkSize;
		uint32_t m_currentChunksCount;
		uint32_t m_expectedChunksCount;
		uint64_t m_totalReceivedBytes;

		SafeDeque<File>& m_incomingFilesQueue;
		CryptoPP::RSA::PrivateKey* m_myPrivateKey;
		asio::ip::tcp::socket& m_socket;
		std::array<char, c_receivedChunkSize> m_receiveBuffer{};
		CryptoPP::SecByteBlock m_sessionKey;
		net::Message m_metadataMessage;
		std::ofstream m_fileStream;
		File m_file;

		std::function<void(std::error_code, std::optional<File>)> m_onReceiveError;
		std::function<void(const File&, uint32_t)> m_onProgressUpdate;
	};
}