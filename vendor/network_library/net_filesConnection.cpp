#include "net_filesConnection.h"

namespace net 
{
	FilesConnection::FilesConnection(
		asio::io_context& asioContext,
		asio::ip::tcp::socket socket,
		SafeDeque<File>& incomingFilesQueue,
		CryptoPP::RSA::PrivateKey* myPrivateKey,
		std::function<void(std::error_code, std::optional<File>)> onReceiveFileError,
		std::function<void(std::error_code, File)> onSendFileError,
		std::function<void(const File&, uint32_t)> onSendProgressUpdate,
		std::function<void(const File&, uint32_t)> onReceiveProgressUpdate,
		std::function<void()> onAllFilesSent)
		: m_asioContext(asioContext),
		m_socket(std::move(socket)),
		m_filesSender(asioContext, m_socket, onSendProgressUpdate, onSendFileError, onAllFilesSent),
		m_filesReceiver(myPrivateKey, incomingFilesQueue, m_socket, onReceiveProgressUpdate, onReceiveFileError)
	{
		m_filesReceiver.startReceiving();
	}


	void FilesConnection::sendFile(const File& file) {
		m_filesSender.sendFile(file);
	}

	void FilesConnection::disconnect() {
		auto self = shared_from_this();

		asio::post(m_asioContext, [this, self]() {
			if (m_socket.is_open()) {
				std::error_code ec;
				m_socket.close(ec);
				if (ec) {
					std::cerr << "Socket close error: " << ec.message() << "\n";
				}
				else {
					std::cout << "Connection closed successfully\n";
				}
			}
		});
	}
}

