#pragma once

#include "net_packet.h"
#include "net_safeDeque.h"
#include "asio.hpp"


namespace net {
	class Connection;

	class PacketsSender {
	public:
		PacketsSender() = default;

		PacketsSender(asio::io_context* asioContext,
			asio::ip::tcp::socket* socket, 
			std::function<void(std::error_code, Packet)> onSendMessageError,
			std::function<void()> onDisconnect,
			std::function<void(uint32_t)> onPacketSent
		);

		PacketsSender(const PacketsSender&) = delete;
		PacketsSender& operator=(const PacketsSender&) = delete;

		PacketsSender& operator=(PacketsSender&& other) noexcept;
		PacketsSender(PacketsSender&& other) noexcept;

		void send(const Message& msg);

	private:
		void writeHeader();
		void writeBody();

	private:
		asio::ip::tcp::socket* m_socket = nullptr;
		asio::io_context* m_asioContext = nullptr;

		SafeDeque<Packet> m_safeDequeOutgoingMessages;

		std::function<void(uint32_t)> m_onPacketSent;
		std::function<void(std::error_code, Message)> m_onSendMessageError;
		std::function<void()> m_onDisconnect;
	};
}

