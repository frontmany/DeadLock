#include "net_packetsSender.h"
#include "net_connection.h"

namespace net {

	PacketsSender::PacketsSender(asio::io_context* asioContext,
		asio::ip::tcp::socket* socket, 
		std::function<void(std::error_code, net::Message)> onSendMessageError,
		std::function<void()> onDisconnect,
		std::function<void(uint32_t)> onPacketSent)
		: m_socket(socket),
		m_asioContext(asioContext),
		m_onSendMessageError(onSendMessageError),
		m_onDisconnect(onDisconnect),
		m_onPacketSent(onPacketSent)
	{
	}

	void PacketsSender::send(const net::Message& msg) {
		asio::post(*m_asioContext, [this, msg]() {
			bool isAbleToWrite = m_safeDequeOutgoingMessages.empty();

			m_safeDequeOutgoingMessages.push_back(msg);

			if (isAbleToWrite) {
				writeHeader();
			}
		});
	}

	PacketsSender::PacketsSender(PacketsSender&& other) noexcept
		: m_socket(std::exchange(other.m_socket, nullptr)),
		m_asioContext(std::exchange(other.m_asioContext, nullptr)),
		m_onSendMessageError(std::move(other.m_onSendMessageError)),
		m_onDisconnect(std::move(other.m_onDisconnect)) {
	}

	PacketsSender& PacketsSender::operator=(PacketsSender&& other) noexcept {
		if (this != &other) {
			m_socket = std::exchange(other.m_socket, nullptr);
			m_asioContext = std::exchange(other.m_asioContext, nullptr);
			m_onSendMessageError = std::move(other.m_onSendMessageError);
			m_onDisconnect = std::move(other.m_onDisconnect);
		}
		return *this;
	}

	void PacketsSender::writeHeader() {
		asio::async_write(
			*m_socket,
			asio::buffer(&m_safeDequeOutgoingMessages.front().header,
			sizeof(MessageHeader)),
			[this](std::error_code ec, std::size_t length) {
				if (ec)
				{
					m_onDisconnect();

					if (ec != asio::error::connection_reset) {
						m_onSendMessageError(ec, m_safeDequeOutgoingMessages.pop_front());

					}
				}
				else
				{
					if (m_safeDequeOutgoingMessages.front().body.size() > 0)
					{
						writeBody();
					}
					else
					{
						m_safeDequeOutgoingMessages.pop_front();

						if (!m_safeDequeOutgoingMessages.empty())
						{
							writeHeader();
						}
					}
				}
			}
		);
	}

	void PacketsSender::writeBody() {
		asio::async_write(
			*m_socket,
			asio::buffer(m_safeDequeOutgoingMessages.front().body.data(),
				m_safeDequeOutgoingMessages.front().body.size()),
			[this](std::error_code ec, std::size_t length)
			{
				if (ec) {
					m_onDisconnect();

					if (ec != asio::error::connection_reset) {
						m_onSendMessageError(ec, m_safeDequeOutgoingMessages.pop_front());

					}
				}
				else {
					m_safeDequeOutgoingMessages.pop_front();

					if (!m_safeDequeOutgoingMessages.empty())
					{
						writeHeader();
					}
				}
			}
		);
	}
}

