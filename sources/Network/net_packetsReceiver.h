#pragma once
#include "net_packet.h"
#include "net_safeDeque.h"

namespace net {
	class PacketsReceiver {
	public:
		PacketsReceiver() = default;

		PacketsReceiver(asio::ip::tcp::socket* socket,
			SafeDeque<Packet>& safeDequeIncomingPackets,
			std::function<void()> onDisconnect
		);

		PacketsReceiver(const PacketsReceiver&) = delete;
		PacketsReceiver& operator=(const PacketsReceiver&) = delete;

		void startReceiving();

	private:
		void readHeader();
		void readBody();

	private:
		asio::ip::tcp::socket* m_socket;
		Packet m_temporaryPacket;
		SafeDeque<Packet>& m_safeDequeIncomingPackets;

		std::function<void()> m_onClientDisconnected;
		std::function<void()> m_onDisconnect;
	};
}

