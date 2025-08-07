#pragma once
#include "net_common.h"
#include "net_safeDeque.h"
#include "net_message.h"
#include "net_file.h"
#include "net_filesSender.h"
#include "net_filesReceiver.h"

#include "queryType.h"
#include "utility.h"

#include <fstream>              
#include <filesystem>          
#include <system_error>          
#include <string>              
#include <locale>                
#include <codecvt>   

namespace net {
	class FilesConnection : public std::enable_shared_from_this<FilesConnection> {
	public:
		FilesConnection(
			asio::io_context& asioContext,
			asio::ip::tcp::socket socket,
			SafeDeque<File>& incomingFilesQueue,
			CryptoPP::RSA::PrivateKey* myPrivateKey,
			std::function<void(std::error_code, std::optional<File>)> onReceiveFileError,
			std::function<void(std::error_code, File)> onSendFileError,
			std::function<void(const File&, uint32_t)> onSendProgressUpdate,
			std::function<void(const File&, uint32_t)> onReceiveProgressUpdate,
			std::function<void()> onAllFilesSent
		);

		~FilesConnection() = default;

		void disconnect();
		bool isConnected();
		void startReceiving();
		void sendFile(const File& file);



	private:

		FilesSender m_filesSender;
		FilesReceiver m_filesReceiver;
		asio::ip::tcp::socket m_socket;
		asio::io_context& m_asioContext;
	};
}
