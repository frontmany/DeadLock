#include "net_networkManager.h"
#include "net_tasksManager.h"
#include "infoToWhom.h"
#include "database.h"
#include "blob.h"
#include "client.h"

namespace net {
	NetworkManager::NetworkManager(Client& client)
		: m_client(client),
		m_workGuard(asio::make_work_guard(m_context))
	{
		m_connectionsManager = std::make_unique<ConnectionsManager>(
			m_context,
			asio::ip::tcp::socket(m_context),
			asio::ip::tcp::socket(m_context),
			[this]() { m_client.onConnectionDown(); },
			[this](asio::ip::tcp::socket socket) {
				createFilesConnection(std::move(socket));
			},
			[this](asio::ip::tcp::socket socket) {
				createPacketsConnection(std::move(socket));
			},
			[this]() { onPacketsConnectionReconnected(); },
			[this]() { onFilesConnectionReconnected(); }
		);

		m_tasksManager = std::make_unique<TasksManager>();
	}

	void NetworkManager::runContextThread() {
		m_context.restart();

		m_contextThread = std::thread([this]() {
			std::cout << "Context started\n";
			m_context.run(); 
		});
	}

	void NetworkManager::supplyMyPrivateKeyForFilesConnection(const CryptoPP::RSA::PrivateKey& myPrivateKey) {
		m_filesConnection->supplyMyPrivateKeyForFilesReceiver(myPrivateKey);
	}

	void NetworkManager::reconnectPacketsConnection() {
		m_connectionsManager->reconnectMessagesSocket(m_packetsConnection->socket(), m_serverEndpoint);
	}

	void NetworkManager::reconnectFilesConnection(const std::string& loginHash) {
		m_connectionsManager->reconnectFilesSocket(m_filesConnection->socket(), loginHash, m_serverEndpoint);
	}

	bool NetworkManager::createPacketsConnection(const std::string& host, const uint16_t port) {
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

	bool NetworkManager::createFilesConnection(const std::string& loginHash, const std::string& host, const uint16_t port) {
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

	bool NetworkManager::isPacketsConnectionConnected() {
		return m_isPacketsConnectionConnected;
	}

	bool NetworkManager::isFilesConnectionConnected() {
		return m_isFilesConnectionConnected;
	}

	void NetworkManager::onPacketsConnectionReconnected() {
		m_isPacketsConnectionConnected = true;
		m_packetsConnection->startReceiving();
	}
	
	void NetworkManager::onFilesConnectionReconnected() {
		m_isFilesConnectionConnected = true;
		m_filesConnection->startReceiving();
	}

	void NetworkManager::disconnectPacketsConnection() {
		try {
			if (m_filesConnection->isConnected()) {
				m_filesConnection->disconnect();
			}

			m_isPacketsConnectionConnected = false;
		}
		catch (const std::system_error& e) {
			setlocale(LC_ALL, "ru");
			std::cerr << "System error on disconnect my: " << e.what() << "\n";
			m_isPacketsConnectionConnected = false;
		}
		catch (const std::exception& e) {
			std::cerr << "Error on disconnect my: " << e.what() << "\n";
			m_isPacketsConnectionConnected = false;
		}
		catch (...) {
			std::cerr << "Unknown error on disconnect my\n";
			m_isPacketsConnectionConnected = false;
		}
	}

	void NetworkManager::disconnectFilesConnection() {
		try {
			if (m_packetsConnection->isConnected()) {
				m_packetsConnection->disconnect();
			}

			if (m_filesConnection->isConnected()) {
				m_filesConnection->disconnect();
			}

			m_isFilesConnectionConnected = false;
		}
		catch (const std::system_error& e) {
			setlocale(LC_ALL, "ru");
			std::cerr << "System error on disconnect my: " << e.what() << "\n";
			m_isFilesConnectionConnected = false;
		}
		catch (const std::exception& e) {
			std::cerr << "Error on disconnect my: " << e.what() << "\n";
			m_isFilesConnectionConnected = false;
		}
		catch (...) {
			std::cerr << "Unknown error on disconnect my\n";
			m_isFilesConnectionConnected = false;
		}
	}

	void NetworkManager::sendPacket(const std::string& packetStr, PacketType type) {
		Packet packet;
		packet.header.type = static_cast<uint32_t>(type);
		packet << packetStr;
		m_packetsConnection->sendPacket(packet);
	}

	void NetworkManager::sendBlob(BlobPtr blob, const std::vector<CryptoPP::RSA::PublicKey>& friendKeys) {
		m_filesConnection->sendBlob(blob, friendKeys);
	}

	void sendAvatar(AvatarPtr avatar, const CryptoPP::RSA::PublicKey& serverKey) {
		m_filesConnection->sendAvatar(avatar, serverKey);
	}


	void NetworkManager::startProcessingIncomingPackets() 
	{
		while (true) {
			if (!m_dequeIncomingPackets.empty()) {
				Packet packet = m_dequeIncomingPackets.pop_front();
				m_client.onPacket(std::move(packet));
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MS));
		}
	}

	void NetworkManager::startProcessingIncomingBlobs()
	{
		while (true) {
			if (!m_dequeIncomingBlobs.empty()) {
				BlobPtr blob = m_dequeIncomingBlobs.pop_front();
				m_client.onBlob(blob);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP_MS));
		}
	}


	void NetworkManager::createPacketsConnection(asio::ip::tcp::socket messagesSocket) {
		m_packetsConnection = std::make_shared<PacketsConnection>(
			m_context,
			std::move(messagesSocket),
			m_dequeIncomingPackets,
			[this](std::error_code ec, Packet&& unsentPacket) {m_client.onSendPacketError(ec, std::move(unsentPacket)); },
			[this](){m_client.onConnectionDown(); }
		);

		m_packetsConnection->startReceiving();
		m_isPacketsConnectionConnected = true;
	}

	void NetworkManager::createFilesConnection(asio::ip::tcp::socket filesSocket) {
		m_filesConnection = std::make_shared<FilesConnection>(
			m_context,
			std::move(filesSocket),
			m_dequeIncomingBlobs,
			[this](std::error_code ec, InfoToWhom&& info) {m_client.onReceiveRequestedFileError(ec, std::move(info)); },
			[this](std::error_code ec, InfoToWhom&& info) {m_client.onSendFileError(ec, info); },
			[this](InfoToWhom&& info, uint32_t progressPercent) {m_client.onSendFileProgressUpdate(info, progressPercent); },
			[this](InfoToWhom&& info, uint32_t progressPercent) {m_client.onRequestedFileProgressUpdate(info, progressPercent); },
			[this]( uint32_t progressPercent) {m_client.onDeadlockNewVersionProgressUpdate(progressPercent); }
		);

		m_filesConnection->startReceiving();
		m_isFilesConnectionConnected = true;
	}
}