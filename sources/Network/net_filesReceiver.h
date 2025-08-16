#pragma once

#include "net_fileDataTemporaryContainer.h"
#include "fileLocationInfo.h"
#include "net_safeDeque.h"
#include "net_packet.h"

#include "avatarInfo.h"
#include "packetType.h"
#include "utility.h"
#include "blob.h"

#include "asio.hpp"
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

class Blob;
class Database;

typedef std::shared_ptr<Blob> BlobPtr;

namespace net
{
	class FilesReceiver {
	public:
		FilesReceiver(SafeDeque<BlobPtr>& incomingBlobsQueue,
			asio::ip::tcp::socket& socket,
			Database& db,
			std::function<void(AvatarInfo&&)> onAvatar,
			std::function<void(FileLocationInfo&&, uint32_t)> onRequestedFileProgressUpdate,
			std::function<void(uint32_t)> onDeadlockNewVersionProgressUpdate,
			std::function<void(std::error_code, FileLocationInfo&&)> onReceiveRequestedFileError
		);

		void startReceiving();
		void setMyPrivateKey(const CryptoPP::RSA::PrivateKey& myPrivateKey);

	private:
		void readMetadataHeader();
		void readMetadataBody();

		void readChunk();
		void readChunkWithoutDecryption();

		std::string parseFileMetadata();
		std::string parseAvatarMetadata();
		std::string parseDeadlockUpdateMetadata();

		void reset();
		void processError(std::error_code ec);
		void finalizeReceiving();
		void notifyReceiveRequestedFileProgress(bool hundredPercentReceived = false);
		void removePartiallyDownloadedFile();
		void openFile(const std::string& path);

	private:
		static constexpr uint32_t c_decryptedChunkSize = 8192;
		static constexpr uint32_t c_receivedChunkSize = 8220;
		static constexpr uint32_t c_overhead = 28;

		uint32_t m_lastChunkSize;
		uint32_t m_currentChunksCount;
		uint32_t m_expectedChunksCount;
		uint64_t m_totalReceivedBytes;

		SafeDeque<BlobPtr>& m_incomingBlobsQueue;
		const CryptoPP::RSA::PrivateKey* m_myPrivateKey;
		asio::ip::tcp::socket& m_socket;
		std::array<char, c_receivedChunkSize> m_receiveBuffer{};
		CryptoPP::SecByteBlock m_sessionKey;
		Packet m_metadataPacket;
		std::ofstream m_fileStream;
		Database& m_db;

		FileDataTemporaryContainer m_fileDataTemporaryContainer;
		FileLocationInfo m_fileLocationInfoForCallback;
		AvatarInfo m_avatarInfo;

		std::function<void(AvatarInfo&&)> m_onAvatar;
		std::function<void(std::error_code, FileLocationInfo&&)> m_onReceiveRequestedFileError;
		std::function<void(FileLocationInfo&&, uint32_t)> m_onRequestedFileProgressUpdate;
		std::function<void(uint32_t)> m_onDeadlockNewVersionProgressUpdate;


		static constexpr const char* BLOB_ID = "blobId";
		static constexpr const char* FILE_ID = "fileId";
		static constexpr const char* MY_UID = "myUID";
		static constexpr const char* SENDER_UID = "senderUID";
		static constexpr const char* FILE_SIZE = "fileSize";
		static constexpr const char* FILE_NAME = "fileName";
		static constexpr const char* TIMESTAMP = "timestamp";
		static constexpr const char* FILES_COUNT_IN_BLOB = "filesCountInBlob";
		static constexpr const char* CAPTION = "caption";
		static constexpr const char* ENCRYPTED_KEY = "encryptedKey";
	};
}