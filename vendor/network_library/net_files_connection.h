#pragma once

#include "net_common.h"
#include "net_safe_deque.h"
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
	template<typename T>
	class files_connection : public std::enable_shared_from_this<files_connection<T>> {
	public:
		files_connection(
			asio::io_context& asioContext,
			asio::ip::tcp::socket socket,
			safe_deque<owned_file<T>>& incomingFilesQueue,
			CryptoPP::RSA::PrivateKey* myPrivateKey,
			std::function<void(std::error_code, net::file<T>)> onReceiveFileError,
			std::function<void(std::error_code, net::file<T>)> onSendFileError,
			std::function<void(const net::file<T>&, uint32_t)> onSendProgressUpdate,
			std::function<void(const net::file<T>&, uint32_t)> onReceiveProgressUpdate,
			std::function<void()> onAllFilesSent)
			: m_asioContext(asioContext),
			m_socket(std::move(socket)),
			m_filesSender(asioContext, m_socket, onSendProgressUpdate, onSendFileError, onAllFilesSent),
			m_filesReceiver(myPrivateKey, incomingFilesQueue, m_socket, onReceiveProgressUpdate, onReceiveFileError)
		{
			m_filesReceiver.startReceiving();
		}
		

		virtual ~files_connection() {}

		void sendFile(const net::file<T>& file) {
			m_filesSender.sendFile(file);
		}

		void disconnect() {
			if (m_socket.is_open()) {
				m_socket.close();
			}
		}

	private:
		filesSender<T> m_filesSender;
		filesReceiver<T> m_filesReceiver;
		asio::ip::tcp::socket m_socket;
		asio::io_context& m_asioContext;
	};
}
