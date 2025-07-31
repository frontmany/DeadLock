#pragma once

#include "net_common.h"
#include "net_connection.h"
#include "net_filesConnection.h"

#include "net_safeDeque.h"
#include "net_validator.h"
#include "net_message.h"
#include "net_file.h"
#include "rsa.h"

namespace net 
{
	class ClientInterface {
	public:
		ClientInterface();
		virtual ~ClientInterface() = default;

		void runContextThread();

		bool createConnection(const std::string& host, const uint16_t port);
		bool createFilesConnection(const std::string& loginHash, const std::string& host, const uint16_t port);
		
		void stop();
		void disconnect();

		bool isConnected();
		void send(const net::Message& msg);
		void sendFile(const File& file);

		void update(size_t maxMessagesCount = std::numeric_limits<size_t>::max());

		void setServerPublicKey(CryptoPP::RSA::PublicKey serverPublicKey);
		const CryptoPP::RSA::PublicKey& getServerPublicKey();

	protected:
		virtual void onMessage(net::Message message) = 0;
		virtual void onFile(File file) = 0;
		virtual void onAllFilesSent() = 0;

		virtual void onSendFileProgressUpdate(const File& file, uint32_t progressPercent) = 0;
		virtual void onReceiveFileProgressUpdate(const File& file, uint32_t progressPercent) = 0;
		
		virtual void onReceiveFileError(std::error_code ec, std::optional<File> unsentFile) = 0;
		virtual void onSendMessageError(std::error_code ec, net::Message unsentMessage) = 0;
		virtual void onSendFileError(std::error_code ec, File unsentFile) = 0;
		virtual void onConnectionDown() = 0;

	private:
		void createMessagesConnection(asio::ip::tcp::socket messagesSocket);

		void createFilesConnection(asio::ip::tcp::socket filesSocket);

	private:
		bool m_is_connected = false;
		std::unique_ptr<Validator> m_validator;
		std::thread	m_context_thread;
		asio::io_context m_context;
		std::shared_ptr<Connection> m_connection;
		std::shared_ptr<FilesConnection> m_files_connection;
		SafeDeque<net::Message> m_safe_deque_of_incoming_messages;
		SafeDeque<File> m_safe_deque_of_incoming_files;


	protected:
		CryptoPP::RSA::PublicKey  m_server_public_key;
		CryptoPP::RSA::PrivateKey m_my_private_key;
		CryptoPP::RSA::PublicKey  m_my_public_key;
	};
}

