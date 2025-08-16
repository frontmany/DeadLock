#include "net_filesConnection.h"

namespace net 
{
	FilesConnection::FilesConnection(
		asio::io_context& asioContext,
		asio::ip::tcp::socket socket,
		SafeDeque<File>& incomingFilesQueue,
		std::function<void(std::error_code, std::optional<File>)> onReceiveFileError,
		std::function<void(std::error_code ec, FileLocationInfo&&)> onSendFileError,
		std::function<void(std::error_code, File)> onSendFileError,
		std::function<void()> onAvatarSent,
		std::function<void(FileLocationInfo&&, uint32_t)> onSendProgressUpdate,
		std::function<void(const File&, uint32_t)> onReceiveProgressUpdate)
		: m_asioContext(asioContext),
		m_socket(std::move(socket)),
		m_filesSender(asioContext, m_socket, onSendProgressUpdate, onSendFileError),
		m_filesReceiver(incomingFilesQueue, m_socket, onReceiveProgressUpdate, onReceiveFileError)
	{
	}

	void FilesConnection::supplyMyPrivateKeyForFilesReceiver(const CryptoPP::RSA::PrivateKey& myPrivateKey) {
		m_filesReceiver.setMyPrivateKey(myPrivateKey);
	}

	void FilesConnection::startReceiving() {
		m_filesReceiver.startReceiving();
	}

	void FilesConnection::send(const std::variant<FileTransferData, AvatarTransferData>& file) {
		m_filesSender.sendFile(file);
	}

	bool FilesConnection::isConnected() {
		return m_socket.is_open();
	}

	asio::ip::tcp::socket& FilesConnection::socket() {
		return m_socket;
	}

	void FilesConnection::disconnect() {
		if (m_socket.is_open()) {
			std::error_code ec;
			m_socket.cancel();
			m_socket.close(ec);
			if (ec) {
				std::cerr << "Socket close error my: " << ec.message() << "\n";
			}
			else {
				std::cout << "Connection closed successfully\n";
			}
		}
	}
}

