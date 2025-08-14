#include "net_connectionsManager.h"


namespace net {
	ConnectionsManager::ConnectionsManager(asio::io_context& asioContext,
		asio::ip::tcp::socket&& filesSocket,
		asio::ip::tcp::socket&& messagesSocket,
		std::function<void()> errorCallback,
		std::function<void(asio::ip::tcp::socket socket)> onFilesSocketValidated,
		std::function<void(asio::ip::tcp::socket socket)> onMessagesSocketValidated,
		std::function<void()> messagesSocketReconnected,
		std::function<void()> filesSocketReconnected)
		: m_asio_context(asioContext),
		m_filesSocketReconnected(filesSocketReconnected),
		m_messagesSocketReconnected(messagesSocketReconnected),
		m_on_connect_error(std::move(errorCallback)),
		m_on_files_socket_validated(std::move(onFilesSocketValidated)),
		m_on_messages_socket_validated(std::move(onMessagesSocketValidated)),
		m_files_socket(std::move(filesSocket)),
		m_messages_socket(std::move(messagesSocket))
	{
		m_messages_socket_hand_shake_out = 0;
		m_messages_socket_hand_shake_in = 0;
		m_files_socket_hand_shake_out = 0;
		m_files_socket_hand_shake_in = 0;
	}

	void ConnectionsManager::connectFilesSocketToServer(std::string loginHash, const asio::ip::tcp::resolver::results_type& endpoint) {
		asio::async_connect(m_files_socket, endpoint,
			[loginHash, this](std::error_code ec, const asio::ip::tcp::endpoint& endpoint) {
				if (ec) {
					if (m_on_connect_error) {
						m_on_connect_error();
					}
					std::cerr << "Connection failed: " << ec.message() << std::endl;
				}
				else {
					std::cout << "Connected to: " << endpoint << std::endl;
					readFilesSocketValidation(std::move(loginHash));
				}
			});
	}

	void ConnectionsManager::connectMessagesSocketToServer(const asio::ip::tcp::resolver::results_type& endpoint) {
		asio::async_connect(m_messages_socket, endpoint,
			[this](std::error_code ec, const asio::ip::tcp::endpoint& endpoint) {
				if (ec) {
					if (m_on_connect_error) {
						m_on_connect_error();
					}
					std::cerr << "Connection failed: " << ec.message() << std::endl;
				}
				else {
					std::cout << "Connected to: " << endpoint << std::endl;
					readMessagesSocketValidation();
				}
			});
	}

	void ConnectionsManager::writeFilesSocketValidation(std::string loginHash) {
		if (m_isReconnectingFilesSocket) {
			asio::async_write(*m_tmp_files_socket_to_reconnect,
				asio::buffer(&m_files_socket_hand_shake_out, sizeof(uint64_t)),
				[loginHash, this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
						return;
					}

					uint32_t login_length = static_cast<uint32_t>(loginHash.size());
					asio::async_write(*m_tmp_files_socket_to_reconnect,
						asio::buffer(&login_length, sizeof(uint32_t)),
						[loginHash, this](std::error_code ec, std::size_t length) {
							if (ec) {
								m_on_connect_error();
							}
							else {
								asio::async_write(*m_tmp_files_socket_to_reconnect,
									asio::buffer(loginHash.data(), loginHash.size()),
									[loginHash, this](std::error_code ec, std::size_t length) {
										if (ec) {
											m_on_connect_error();
											return;
										}
										else {
											m_filesSocketReconnected();
											m_isReconnectingFilesSocket = false;
											m_tmp_files_socket_to_reconnect = nullptr;
										}

									});
							}
						});
				});
		}
		else {
			asio::async_write(m_files_socket,
				asio::buffer(&m_files_socket_hand_shake_out, sizeof(uint64_t)),
				[loginHash, this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
						return;
					}

					uint32_t login_length = static_cast<uint32_t>(loginHash.size());
					asio::async_write(m_files_socket,
						asio::buffer(&login_length, sizeof(uint32_t)),
						[loginHash, this](std::error_code ec, std::size_t length) {
							if (ec) {
								m_on_connect_error();
							}
							else {
								asio::async_write(m_files_socket,
									asio::buffer(loginHash.data(), loginHash.size()),
									[loginHash, this](std::error_code ec, std::size_t length) {
										if (ec) {
											m_on_connect_error();
											return;
										}
										else {
											m_on_files_socket_validated(std::move(m_files_socket));
										}

									});
							}
						});
				});
		}
	}

	void ConnectionsManager::writeMessagesSocketValidation() {
		if (m_isReconnectingMessagesSocket) {
			asio::async_write(*m_tmp_messages_socket_to_reconnect, asio::buffer(&m_messages_socket_hand_shake_out, sizeof(uint64_t)),
				[this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
					}
					else {
						m_messagesSocketReconnected();
						m_isReconnectingMessagesSocket = false;
						m_tmp_messages_socket_to_reconnect = nullptr;
					}
				});
		}
		else {
			asio::async_write(m_messages_socket, asio::buffer(&m_messages_socket_hand_shake_out, sizeof(uint64_t)),
				[this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
					}
					else {
						m_on_messages_socket_validated(std::move(m_messages_socket));
					}
				});
		}
	}

	void ConnectionsManager::readFilesSocketValidation(std::string loginHash) {
		if (m_isReconnectingFilesSocket) {
			asio::async_read(*m_tmp_files_socket_to_reconnect, asio::buffer(&m_files_socket_hand_shake_in, sizeof(uint64_t)),
				[loginHash, this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
					}
					else {
						m_files_socket_hand_shake_out = scramble(m_files_socket_hand_shake_in);
						m_files_socket_hand_shake_out++;
						writeFilesSocketValidation(std::move(loginHash));
					}
				});
		}
		else {
			asio::async_read(m_files_socket, asio::buffer(&m_files_socket_hand_shake_in, sizeof(uint64_t)),
				[loginHash, this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
					}
					else {
						m_files_socket_hand_shake_out = scramble(m_files_socket_hand_shake_in);
						m_files_socket_hand_shake_out++;
						writeFilesSocketValidation(std::move(loginHash));
					}
				});
		}
	}

	void ConnectionsManager::readMessagesSocketValidation() {
		if (m_isReconnectingMessagesSocket) {
			asio::async_read(*m_tmp_messages_socket_to_reconnect, asio::buffer(&m_messages_socket_hand_shake_in, sizeof(uint64_t)),
				[this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
					}
					else {
						m_messages_socket_hand_shake_out = scramble(m_messages_socket_hand_shake_in);
						writeMessagesSocketValidation();
					}
				});
		}
		else {
			asio::async_read(m_messages_socket, asio::buffer(&m_messages_socket_hand_shake_in, sizeof(uint64_t)),
				[this](std::error_code ec, std::size_t length) {
					if (ec) {
						m_on_connect_error();
					}
					else {
						m_messages_socket_hand_shake_out = scramble(m_messages_socket_hand_shake_in);
						writeMessagesSocketValidation();
					}
				});
		}
		
	}

	void ConnectionsManager::reconnectMessagesSocket(asio::ip::tcp::socket& socket, const asio::ip::tcp::resolver::results_type& endpoint) {
		m_tmp_messages_socket_to_reconnect = &socket;
		m_isReconnectingMessagesSocket = true;
		
		asio::async_connect(*m_tmp_messages_socket_to_reconnect, endpoint,
			[this](std::error_code ec, const asio::ip::tcp::endpoint& endpoint) {
				if (ec) {
					if (m_on_connect_error) {
						m_on_connect_error();
					}
					std::cerr << "Connection failed: " << ec.message() << std::endl;
				}
				else {
					std::cout << "Connected to: " << endpoint << std::endl;
					readMessagesSocketValidation();
				}
			});
	}

	void ConnectionsManager::reconnectFilesSocket(asio::ip::tcp::socket& socket, std::string loginHash, const asio::ip::tcp::resolver::results_type& endpoint) {
		m_tmp_files_socket_to_reconnect = &socket;
		m_isReconnectingFilesSocket = true;

		asio::async_connect(socket, endpoint,
			[loginHash, this](std::error_code ec, const asio::ip::tcp::endpoint& endpoint) {
				if (ec) {
					if (m_on_connect_error) {
						m_on_connect_error();
					}
					std::cerr << "Connection failed: " << ec.message() << std::endl;
				}
				else {
					std::cout << "Connected to: " << endpoint << std::endl;
					readFilesSocketValidation(std::move(loginHash));
				}
			});
	}

	void ConnectionsManager::disconnect() {
		if (m_messages_socket.is_open()) {
			asio::post(m_asio_context, [this]() { m_messages_socket.close(); });
		}
		if (m_files_socket.is_open()) {
			asio::post(m_asio_context, [this]() { m_files_socket.close(); });
		}
	}

	uint64_t ConnectionsManager::scramble(uint64_t inputNumber) {
		uint64_t out = inputNumber ^ 0xDEADBEEFC;
		out = (out & 0xF0F0F0F0F) >> 4 | (out & 0x0F0F0F0F0F) << 4;
		return out ^ 0xC0DEFACE12345678;
	}
}