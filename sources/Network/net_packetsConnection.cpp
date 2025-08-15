#include "net_packetsConnection.h"
#include "net_safeDeque.h"



namespace net {
	PacketsConnection::PacketsConnection(asio::io_context& asioContext,
		asio::ip::tcp::socket socket,
		SafeDeque<Packet>& safeDequeIncomingPackets,
		std::function<void(std::error_code, Packet&&)> onSendError,
		std::function<void()> onDisconnect,
		std::function<void(Packet&&)> onPacketSent)
		: m_asioContext(asioContext),
		m_socket(std::move(socket)),
		m_packetsSender(&asioContext,&m_socket, onSendError, onDisconnect, onPacketSent),
		m_packetsReceiver(&m_socket, safeDequeIncomingPackets, onDisconnect)
	{
	}

	void PacketsConnection::startReceiving() {
		m_packetsReceiver.startReceiving();
	}

	void PacketsConnection::disconnect() {
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

	asio::ip::tcp::socket& PacketsConnection::socket() {
		return m_socket;
	}

	bool PacketsConnection::isConnected() {
		return m_socket.is_open();
	}

	void PacketsConnection::sendPacket(const Packet& packet) {
		m_packetsSender.sendPacket(packet);
	}

	void PacketsConnection::sendMessage(const Packet& packet, std::vector<CryptoPP::RSA::PublicKey> friendKeys) {
		m_packetsSender.send(packet);
	}
}

