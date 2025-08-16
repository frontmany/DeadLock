#pragma once
#include "net_safeDeque.h"
#include "net_filesSender.h"
#include "net_filesReceiver.h"
#include "net_fileTransferData.h"
#include "net_avatarTransferData.h"

#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

namespace net {
	class FilesConnection : public std::enable_shared_from_this<FilesConnection> {
	public:
		FilesConnection(
			asio::io_context& asioContext,
			asio::ip::tcp::socket socket,
			SafeDeque<File>& incomingFilesQueue,
			std::function<void(std::error_code, std::optional<File>)> onReceiveFileError,
			std::function<void(std::error_code ec, FileLocationInfo&&)> onSendFileError,
			std::function<void()> onAvatarSent,
			std::function<void(FileLocationInfo&&, uint32_t)> onSendProgressUpdate,
			std::function<void(const File&, uint32_t)> onReceiveProgressUpdate
		);

		~FilesConnection() = default;

		asio::ip::tcp::socket& socket();
		void supplyMyPrivateKeyForFilesReceiver(const CryptoPP::RSA::PrivateKey& myPrivateKey);
		void disconnect();
		bool isConnected();
		void startReceiving();
		void send(const std::variant<FileTransferData, AvatarTransferData>& file);

	private:
		FilesSender m_filesSender;
		FilesReceiver m_filesReceiver;
		asio::ip::tcp::socket m_socket;
		asio::io_context& m_asioContext;
	};
}
