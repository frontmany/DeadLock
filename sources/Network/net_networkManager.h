#pragma once

#include "net_common.h"
#include "net_packetsConnection.h"
#include "net_filesConnection.h"
#include "net_safeDeque.h"
#include "net_connectionsManager.h"
#include "net_packet.h"

#include "packetType.h"
#include "rsa.h"

class Blob;
class Message;
class Avatar;
class Database;
class Client;
class TasksManager;
struct InfoToWhom;

typedef std::shared_ptr<Blob> BlobPtr;
typedef std::shared_ptr<Message> MesasgePtr;
typedef std::shared_ptr<Avatar> AvatarPtr;
typedef std::shared_ptr<Database> DatabasePtr;

namespace net 
{
	class NetworkManager {
	public:
		NetworkManager(Client& client);
		virtual ~NetworkManager() = default;
		void runContextThread();
		void supplyMyPrivateKeyForFilesConnection(const CryptoPP::RSA::PrivateKey& myPrivateKey);
		void disconnectPacketsConnection();
		void disconnectFilesConnection();
		bool createPacketsConnection(const std::string& host, const uint16_t port);
		bool createFilesConnection(const std::string& loginHash, const std::string& host, const uint16_t port);
		void reconnectPacketsConnection();
		void reconnectFilesConnection(const std::string& loginHash);
		bool isPacketsConnectionConnected();
		bool isFilesConnectionConnected();
		void startProcessingIncomingPackets();
		void startProcessingIncomingBlobs();

		void sendPacket(const std::string& packetStr, PacketType type);
		void sendMesasge(MessagePtr message);
		void sendBlob(BlobPtr blob);
		void sendAvatar(AvatarPtr avatar);

	private:
		void onPacketsConnectionReconnected();
		void onFilesConnectionReconnected();
		void createPacketsConnection(asio::ip::tcp::socket packetsSocket);
		void createFilesConnection(asio::ip::tcp::socket filesSocket);

	private:
		Client& m_client;
		asio::io_context m_context;
		std::thread	m_contextThread;
		asio::executor_work_guard<asio::io_context::executor_type, void, void> m_workGuard;
		
		std::unique_ptr<TasksManager> m_tasksManager;
		std::unique_ptr<ConnectionsManager> m_connectionsManager;
		asio::ip::tcp::resolver::results_type m_serverEndpoint;
		std::shared_ptr<FilesConnection> m_filesConnection;
		std::shared_ptr<PacketsConnection> m_packetsConnection;
		SafeDeque<Packet> m_dequeIncomingPackets;
		SafeDeque<BlobPtr> m_dequeIncomingBlobs;
		bool m_isFilesConnectionConnected = false;
		bool m_isPacketsConnectionConnected = false;

		static constexpr const uint32_t SLEEP_MS = 15;
	};
}

