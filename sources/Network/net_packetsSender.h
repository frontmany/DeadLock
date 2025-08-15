#pragma once

#include "net_packet.h"
#include "net_safeDeque.h"
#include "asio.hpp"


namespace net {
	class PacketsConnection;

	class PacketsSender {
	public:
		PacketsSender() = default;

		PacketsSender(asio::io_context* asioContext,
			asio::ip::tcp::socket* socket, 
			std::function<void(std::error_code, Packet&&)> onSendPacketError,
			std::function<void()> onDisconnect,
			std::function<void(Packet&&)> onPacketSent
		);

		PacketsSender(const PacketsSender&) = delete;
		PacketsSender& operator=(const PacketsSender&) = delete;

		PacketsSender& operator=(PacketsSender&& other) noexcept;
		PacketsSender(PacketsSender&& other) noexcept;

		void sendPacket(const Packet& packet, std::optional<std::string> ifConfirmationPacketId = std::nullopt);

	private:
		void writeHeader();
		void writeBody();

	private:
		asio::ip::tcp::socket* m_socket = nullptr;
		asio::io_context* m_asioContext = nullptr;
		std::optional<std::string> m_ifConfirmationPacketId;

		SafeDeque<Packet> m_safeDequeOutgoingPackets;

		std::function<void(Packet&&)> m_onPacketSent;
		std::function<void(std::error_code, Packet&&)> m_onSendPacketError;
		std::function<void()> m_onDisconnect;
	};
}

