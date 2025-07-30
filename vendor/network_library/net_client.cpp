#include "net_client.h"
#include "net_validator.h"

namespace net {
	ClientInterface::ClientInterface()
		: m_validator(std::make_unique<Validator>(
			m_context,
			asio::ip::tcp::socket(m_context),
			asio::ip::tcp::socket(m_context),
			[this]() { onConnectionError(); },
			[this](asio::ip::tcp::socket socket) { createFilesConnection(std::move(socket)); },
			[this](asio::ip::tcp::socket socket) { createMessagesConnection(std::move(socket)); }
		))
	{
	}

	void ClientInterface::runContextThread() {
		m_context_thread = std::thread([this]() {m_context.run(); });
	}

	bool ClientInterface::createConnection(const std::string& host, const uint16_t port) {
		try {
			asio::ip::tcp::resolver resolver(m_context);
			asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(host, std::to_string(port));
			m_validator->connectMessagesSocketToServer(endpoint);

			return true;
		}
		catch (std::exception& e) {
			std::cerr << "Client exception: " << e.what() << "\n";
			return false;
		}
	}

	bool ClientInterface::createFilesConnection(const std::string& loginHash, const std::string& host, const uint16_t port) {
		try {
			asio::ip::tcp::resolver resolver(m_context);
			asio::ip::tcp::resolver::results_type endpoint = resolver.resolve(host, std::to_string(port));
			m_validator->connectFilesSocketToServer(std::move(loginHash), endpoint);

			return true;
		}
		catch (std::exception& e) {
			std::cerr << "Client exception: " << e.what() << "\n";
			return false;
		}
	}

	void ClientInterface::disconnect() {
		if (m_context.stopped()) {
			return;
		}

		try {
			m_context.stop();

			if (m_messages_connection) {
				m_messages_connection->disconnect();

			}

			if (m_files_connection) {
				m_files_connection->disconnect();
			}

			if (m_context_thread.joinable()) {
				m_context_thread.join();
			}
		}
		catch (const std::system_error& e) {
			std::cerr << "System error on disconnect: " << e.what() << "\n";
		}
		catch (const std::exception& e) {
			std::cerr << "Error on disconnect: " << e.what() << "\n";
		}
		catch (...) {
			std::cerr << "Unknown error on disconnect\n";
		}
	}

	void ClientInterface::send(const net::Message& msg)
	{
		m_messages_connection->send(msg);
	}

	void ClientInterface::sendFile(const File& file)
	{
		m_files_connection->sendFile(file);
	}


	void ClientInterface::update(size_t maxMessagesCount = std::numeric_limits<size_t>::max()) {
		size_t processedMessages = 0;

		while (true) {
			if (!m_safe_deque_of_incoming_files.empty()) {
				File file = m_safe_deque_of_incoming_files.pop_front();
				onFile(std::move(file));
			}

			if (!m_safe_deque_of_incoming_messages.empty() && processedMessages < maxMessagesCount) {
				net::Message msg = m_safe_deque_of_incoming_messages.pop_front();
				onMessage(std::move(msg));
				processedMessages++;
			}

			std::this_thread::yield();
		}
	}

	void ClientInterface::setServerPublicKey(CryptoPP::RSA::PublicKey serverPublicKey) {
		m_server_public_key = serverPublicKey;
	}

	const CryptoPP::RSA::PublicKey& ClientInterface::getServerPublicKey() {
		return m_server_public_key;
	}

	void ClientInterface::createMessagesConnection(asio::ip::tcp::socket messagesSocket) {
		m_messages_connection = std::make_shared<Connection>(
			m_context,
			std::move(messagesSocket),
			m_safe_deque_of_incoming_messages,
			[this](std::error_code ec, net::Message unsentMessage) {onSendMessageError(ec, std::move(unsentMessage)); },
			[this](){onConnectionError(); }
		);

		is_messages_socket_validated = true;
	}

	void ClientInterface::createFilesConnection(asio::ip::tcp::socket filesSocket) {
		m_files_connection = std::make_shared<FilesConnection>(
			m_context,
			std::move(filesSocket),
			m_safe_deque_of_incoming_files,
			&m_my_private_key,
			[this](std::error_code ec, File unreadFile) {onReceiveFileError(ec, unreadFile); },
			[this](std::error_code ec, File unsentFile) {onSendFileError(ec, unsentFile); },
			[this](File file, uint32_t progressPercent) {onSendFileProgressUpdate(file, progressPercent); },
			[this](File file, uint32_t progressPercent) {onReceiveFileProgressUpdate(file, progressPercent); },
			[this]() {onAllFilesSent(); }
		);
	}
}