#pragma once
#include "net_common.h"
#include "net_packetsReceiver.h"
#include "net_packetsSender.h"
#include "net_packet.h"

#include "rsa.h"

namespace net {
	class PacketsConnection : public std::enable_shared_from_this<PacketsConnection> {
	public:

		PacketsConnection(asio::io_context& asioContext,
			asio::ip::tcp::socket socket,
			SafeDeque<Packet>& safeDequeIncomingPackets,
			std::function<void(std::error_code, Packet&&)> onSendError,
			std::function<void()> onDisconnect,
			std::function<void(Packet&&)> onPacketSent
		);

		~PacketsConnection() = default;

		asio::ip::tcp::socket& socket();
		void disconnect();
		bool isConnected();
		void startReceiving();
		void sendPacket(const Packet& packet);

	private:
		asio::ip::tcp::socket m_socket;
		asio::io_context& m_asioContext;
		PacketsReceiver m_packetsReceiver;
		PacketsSender m_packetsSender;

	};
}
