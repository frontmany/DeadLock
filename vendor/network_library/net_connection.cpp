#include "net_connection.h"
#include "net_common.h"
#include "net_safeDeque.h"
#include "net_message.h"


namespace net {
	Connection::Connection(asio::io_context& asioContext,
		asio::ip::tcp::socket socket,
		SafeDeque<net::Message>& safeDequeIncomingMessages,
		std::function<void(std::error_code, Message)> onSendError,
		std::function<void()> onDisconnect)
		: m_asioContext(asioContext),
		m_socket(std::move(socket)),
		m_sender(&asioContext,
			&m_socket,
			onSendError,
			onDisconnect),
		m_receiver(&m_socket,
			safeDequeIncomingMessages,
			onDisconnect)
	{
		m_receiver.startReceiving();
	}

	Connection::~Connection() 
	{
	}

	void Connection::disconnect() {
		if (m_socket.is_open()) {
			std::error_code ec;
			m_socket.close(ec);
			if (ec) {
				std::cerr << "Socket close error my: " << ec.message() << "\n";
			}
			else {
				std::cout << "Connection closed successfully\n";
			}

			m_socket.cancel();
		}
	}

	void Connection::send(const Message& message) {
		m_sender.send(message);
	}


	asio::ip::tcp::endpoint Connection::getEndpoint() {
		return m_socket.remote_endpoint();
	}
}

