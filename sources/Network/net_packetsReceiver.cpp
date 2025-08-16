#include "net_packetsReceiver.h"

#include "asio.hpp"
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

namespace net {
	PacketsReceiver::PacketsReceiver(asio::ip::tcp::socket* socket,
		SafeDeque<Packet>& safeDequeIncomingPackets,
		std::function<void()> onDisconnect)
		: m_socket(socket),
		m_safeDequeIncomingPackets(safeDequeIncomingPackets),
		m_onDisconnect(onDisconnect)
	{
	}

	void PacketsReceiver::startReceiving() {
		readHeader();
	}

	void PacketsReceiver::readHeader() {
		asio::async_read(*m_socket, asio::buffer(&m_temporaryPacket.header_mut(), Packet::sizeOfHeader()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::connection_reset && ec != asio::error::operation_aborted) {
						m_onDisconnect();
					}
				}
				else {
					if (m_temporaryPacket.size() > Packet::sizeOfHeader()) {
						m_temporaryPacket.body_mut().resize(m_temporaryPacket.size() - Packet::sizeOfHeader());
						readBody();
					}
					else {
						m_safeDequeIncomingPackets.push_back(m_temporaryPacket);
						readHeader();
					}
				}
			});
	}

	void PacketsReceiver::readBody() {
		asio::async_read(*m_socket, asio::buffer(m_temporaryPacket.body_mut().data(), m_temporaryPacket.body().size()),
			[this](std::error_code ec, std::size_t length) {
				if (ec) {
					if (ec != asio::error::connection_reset && ec != asio::error::operation_aborted) {
						m_onDisconnect();
					}
				}
				else {
					m_safeDequeIncomingPackets.push_back(m_temporaryPacket);
					readHeader();
				}
			});
	}
}

