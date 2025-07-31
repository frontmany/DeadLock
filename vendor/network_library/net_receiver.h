#pragma once
#include "net_message.h"
#include "net_safeDeque.h"
#include "asio.hpp"

namespace net {
	class Connection;

	class Receiver {
	public:
		Receiver() = default;

		Receiver(asio::ip::tcp::socket* socket,
			SafeDeque<net::Message>& safeDequeIncomingMessages,
			std::function<void()> onDisconnect
		);

		Receiver(const Receiver&) = delete;
		Receiver& operator=(const Receiver&) = delete;

		void startReceiving();

	private:
		void readHeader();
		void readBody();

	private:
		asio::ip::tcp::socket* m_socket;
		net::Message m_temporaryMessage;
		SafeDeque<net::Message>& m_safeDequeIncomingMessages;

		std::function<void()> m_onClientDisconnected;
		std::function<void()> m_onDisconnect;
	};
}

