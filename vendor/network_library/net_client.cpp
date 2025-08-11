#include "net_client.h"
#include "blob.h"

namespace net {
	ClientInterface::ClientInterface() 
		:
		m_workGuard(asio::make_work_guard(m_context))
	{
		m_connectionsManager = std::make_unique<ConnectionsManager>(
			m_context,
			asio::ip::tcp::socket(m_context),
			asio::ip::tcp::socket(m_context),
			[this]() { onConnectionDown(); },
			[this](asio::ip::tcp::socket socket) {
				createFilesConnection(std::move(socket));
			},
			[this](asio::ip::tcp::socket socket) {
				createMessagesConnection(std::move(socket));
			},
			[this]() { onMessagesConnectionReconnected(); },
			[this]() { onFilesConnectionReconnected(); }
		);
	}

	void ClientInterface::runContextThread() {
		m_context.restart();

		m_context_thread = std::thread([this]() {
			std::cout << "Context started\n";
			m_context.run(); 
		});
	}

	void ClientInterface::reconnectMessagesConnection() {
		m_connectionsManager->reconnectMessagesSocket(m_connection->socket(), m_serverEndpoint);
	}

	void ClientInterface::reconnectFilesConnection(const std::string& loginHash) {
		m_connectionsManager->reconnectFilesSocket(m_files_connection->socket(), loginHash, m_serverEndpoint);
	}

	bool ClientInterface::createConnection(const std::string& host, const uint16_t port) {
		try {
			asio::ip::tcp::resolver resolver(m_context);
			m_serverEndpoint = resolver.resolve(host, std::to_string(port));
			m_connectionsManager->connectMessagesSocketToServer(m_serverEndpoint);

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
			m_connectionsManager->connectFilesSocketToServer(loginHash, endpoint);

			return true;
		}
		catch (std::exception& e) {
			std::cerr << "Client exception: " << e.what() << "\n";
			return false;
		}
	}

	bool ClientInterface::isConnected() {
		return m_is_connected;
	}

	void ClientInterface::onMessagesConnectionReconnected() {
		m_is_connected = true;
		m_connection->startReceiving();
	}
	
	void ClientInterface::onFilesConnectionReconnected() 
	{
		m_files_connection->startReceiving();
	}

	void ClientInterface::disconnect() {
		try {
			if (m_connection->isConnected()) {
				m_connection->disconnect();
			}

			if (m_files_connection->isConnected()) {
				m_files_connection->disconnect();
			}

			m_is_connected = false;
		}
		catch (const std::system_error& e) {
			setlocale(LC_ALL, "ru");
			std::cerr << "System error on disconnect my: " << e.what() << "\n";
			m_is_connected = false;
		}
		catch (const std::exception& e) {
			std::cerr << "Error on disconnect my: " << e.what() << "\n";
			m_is_connected = false;
		}
		catch (...) {
			std::cerr << "Unknown error on disconnect my\n";
			m_is_connected = false;
		}
	}

	void ClientInterface::send(const net::Message& msg)
	{
		m_connection->send(msg);
	}

	void sendBlob(BlobPtr blob)
	{
		m_files_connection->sendFile(file);
	}


	void ClientInterface::update(size_t maxMessagesCount) {
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
		m_connection = std::make_shared<Connection>(
			m_context,
			std::move(messagesSocket),
			m_safe_deque_of_incoming_messages,
			[this](std::error_code ec, net::Message unsentMessage) {onSendMessageError(ec, std::move(unsentMessage)); },
			[this](){onConnectionDown(); }
		);

		m_connection->startReceiving();
		m_is_connected = true;
	}

	void ClientInterface::createFilesConnection(asio::ip::tcp::socket filesSocket) {
		m_files_connection = std::make_shared<FilesConnection>(
			m_context,
			std::move(filesSocket),
			m_safe_deque_of_incoming_files,
			&m_my_private_key,
			[this](std::error_code ec, std::optional<File> unreadFile) {onReceiveFileError(ec, unreadFile); },
			[this](std::error_code ec, File unsentFile) {onSendFileError(ec, unsentFile); },
			[this](File file, uint32_t progressPercent) {onSendFileProgressUpdate(file, progressPercent); },
			[this](File file, uint32_t progressPercent) {onReceiveFileProgressUpdate(file, progressPercent); },
			[this]() {onAllFilesSent(); }
		);

		m_files_connection->startReceiving();
	}
}