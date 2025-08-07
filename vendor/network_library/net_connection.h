#pragma once
#include "net_common.h"
#include "net_receiver.h"
#include "net_sender.h"

namespace net {
	class Connection : public std::enable_shared_from_this<Connection> {
	public:

		Connection(asio::io_context& asioContext,
			asio::ip::tcp::socket socket,
			SafeDeque<net::Message>& safeDequeIncomingMessages,
			std::function<void(std::error_code, net::Message)> onSendError,
			std::function<void()> onDisconnect
		);

		~Connection();

		asio::ip::tcp::socket& socket();
		void disconnect();
		bool isConnected();
		void startReceiving();
		void send(const net::Message& message);

	private:
		asio::ip::tcp::socket m_socket;
		asio::io_context& m_asioContext;
		Receiver m_receiver;
		Sender m_sender;

	};
}
