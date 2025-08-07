#pragma once

#include "net_common.h"
#include "net_safeDeque.h"
#include "net_message.h"
#include "net_file.h"

#include <fstream>              
#include <filesystem>          
#include <system_error>          
#include <string>              
#include <locale>                
#include <codecvt>   

namespace net {
	class Validator {
	public:
		Validator(asio::io_context& asioContext,
			asio::ip::tcp::socket&& filesSocket,
			asio::ip::tcp::socket&& messagesSocket,
			std::function<void()> errorCallback,
			std::function<void(asio::ip::tcp::socket socket)> onFilesSocketValidated,
			std::function<void(asio::ip::tcp::socket socket)> onMessagesSocketValidated,
			std::function<void()> filesSocketReconnected,
			std::function<void()> messagesSocketReconnected
		);

		Validator() = default;
		~Validator() = default;
		Validator(const Validator&) = delete;
		Validator& operator=(const Validator&) = delete;



		void connectFilesSocketToServer(std::string loginHash, const asio::ip::tcp::resolver::results_type& endpoint);

		void connectMessagesSocketToServer(const asio::ip::tcp::resolver::results_type& endpoint);

		void writeFilesSocketValidation(std::string loginHash);

		void writeMessagesSocketValidation();

		void readFilesSocketValidation(std::string loginHash);

		void readMessagesSocketValidation();

		void disconnect();

		void reconnectMessagesSocket(asio::ip::tcp::socket& socket, const asio::ip::tcp::resolver::results_type& endpoint);
		void reconnectFilesSocket(asio::ip::tcp::socket& socket, std::string loginHash, const asio::ip::tcp::resolver::results_type& endpoint);

		uint64_t scramble(uint64_t inputNumber);

	private:
		bool m_isReconnectingFilesSocket = false;
		bool m_isReconnectingMessagesSocket = false;

		asio::ip::tcp::socket* m_tmp_files_socket_to_reconnect = nullptr;
		asio::ip::tcp::socket* m_tmp_messages_socket_to_reconnect = nullptr;



		asio::ip::tcp::socket m_files_socket;
		asio::ip::tcp::socket m_messages_socket;
		asio::io_context& m_asio_context;

		uint64_t m_messages_socket_hand_shake_out;
		uint64_t m_messages_socket_hand_shake_in;

		uint64_t m_files_socket_hand_shake_out;
		uint64_t m_files_socket_hand_shake_in;

		std::function<void()> m_on_connect_error;
		std::function<void()> m_filesSocketReconnected;
		std::function<void()> m_messagesSocketReconnected;
		std::function<void(asio::ip::tcp::socket filesSocket)> m_on_files_socket_validated;
		std::function<void(asio::ip::tcp::socket messagesSocket)> m_on_messages_socket_validated;
	};
}
