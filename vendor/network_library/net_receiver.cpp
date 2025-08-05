#include "net_receiver.h"
#include "net_connection.h"

namespace net {
	Receiver::Receiver(asio::ip::tcp::socket* socket,
		SafeDeque<net::Message>& safeDequeIncomingMessages,
		std::function<void()> onDisconnect)
		: m_socket(socket),
		m_safeDequeIncomingMessages(safeDequeIncomingMessages),
		m_onDisconnect(onDisconnect)
	{
	}

	void Receiver::startReceiving() {
		readHeader();
	}

	void Receiver::readHeader() {
		asio::async_read(*m_socket, asio::buffer(&m_temporaryMessage.header, sizeof(MessageHeader)),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::operation_aborted) {
						m_onDisconnect();
					}
				}
				else {
					if (m_temporaryMessage.header.size > sizeof(MessageHeader)) {
						m_temporaryMessage.body.resize(m_temporaryMessage.header.size - sizeof(MessageHeader));
						readBody();
					}
					else {
						m_safeDequeIncomingMessages.push_back(m_temporaryMessage);
						readHeader();
					}
				}
			});
	}

	void Receiver::readBody() {
		asio::async_read(*m_socket, asio::buffer(m_temporaryMessage.body.data(), m_temporaryMessage.body.size()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::operation_aborted) {
						m_onDisconnect();
					}
				}
				else {
					m_safeDequeIncomingMessages.push_back(m_temporaryMessage);
					readHeader();
				}
			});
	}
}

