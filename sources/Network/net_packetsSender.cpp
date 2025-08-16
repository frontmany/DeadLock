#include "net_packetsSender.h"

#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

namespace net {

	PacketsSender::PacketsSender(asio::io_context* asioContext,
		asio::ip::tcp::socket* socket, 
		std::function<void(std::error_code, Packet&&)> onSendPacketError,
		std::function<void()> onDisconnect,
		std::function<void(Packet&&)> onPacketSent)
		: m_socket(socket),
		m_asioContext(asioContext),
		m_onSendPacketError(onSendPacketError),
		m_onDisconnect(onDisconnect),
		m_onPacketSent(onPacketSent)
	{
	}

	void PacketsSender::sendPacket(const Packet& packet, std::optional<std::string> ifConfirmationPacketId) {
		asio::post(*m_asioContext, [this, packet]() {
			bool isAbleToWrite = m_safeDequeOutgoingPackets.empty();

			m_safeDequeOutgoingPackets.push_back(packet);

			if (isAbleToWrite) {
				writeHeader();
			}
		});
	}

	PacketsSender::PacketsSender(PacketsSender&& other) noexcept
		: m_socket(std::exchange(other.m_socket, nullptr)),
		m_asioContext(std::exchange(other.m_asioContext, nullptr)),
		m_onSendPacketError(std::move(other.m_onSendPacketError)),
		m_onDisconnect(std::move(other.m_onDisconnect)) {
	}

	PacketsSender& PacketsSender::operator=(PacketsSender&& other) noexcept {
		if (this != &other) {
			m_socket = std::exchange(other.m_socket, nullptr);
			m_asioContext = std::exchange(other.m_asioContext, nullptr);
			m_onSendPacketError = std::move(other.m_onSendPacketError);
			m_onDisconnect = std::move(other.m_onDisconnect);
		}
		return *this;
	}

	void PacketsSender::writeHeader() {
		asio::async_write(
			*m_socket,
			asio::buffer(&m_safeDequeOutgoingPackets.front().header(), Packet::sizeOfHeader()),
			[this](std::error_code ec, std::size_t length) {
				if (ec)
				{
					m_onDisconnect();

					if (ec != asio::error::connection_reset) {
						m_onSendPacketError(ec, m_safeDequeOutgoingPackets.pop_front());

					}
				}
				else
				{
					if (m_safeDequeOutgoingPackets.front().body().size() > 0)
					{
						writeBody();
					}
					else
					{
						m_onPacketSent(std::move(m_safeDequeOutgoingPackets.pop_front()));

						if (!m_safeDequeOutgoingPackets.empty())
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
			asio::buffer(m_safeDequeOutgoingPackets.front().body().data(), m_safeDequeOutgoingPackets.front().body().size()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					m_onDisconnect();

					if (ec != asio::error::connection_reset) {
						m_onSendPacketError(ec, m_safeDequeOutgoingPackets.pop_front());

					}
				}
				else {
					m_onPacketSent(std::move(m_safeDequeOutgoingPackets.pop_front()));

					if (!m_safeDequeOutgoingPackets.empty())
					{
						writeHeader();
					}
				}
			}
		);
	}
}

