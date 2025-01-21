#include"clientSide.h"

void ClientSide::init() {
    WSADATA wsaData;
    int wsaError;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaError = WSAStartup(wVersionRequested, &wsaData);
    if (wsaError != 0) {
        throw std::runtime_error("The Winsock dll not found!");
    }
    
    m_socket = INVALID_SOCKET;
    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET) {
        std::string errorStr = "Error at socket: " + WSAGetLastError();
        throw std::runtime_error(errorStr);
    }
}


void ClientSide::connectTo(std::string ipAddress, int port) {
    m_server_IpAddress = ipAddress;
    m_server_port = port;

    sockaddr_in clientService;
    clientService.sin_family = AF_INET;

    // Преобразование std::string в WCHAR*
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, m_server_IpAddress.c_str(), (int)m_server_IpAddress.size(), NULL, 0);
    std::wstring wIpAddress(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, m_server_IpAddress.c_str(), (int)m_server_IpAddress.size(), &wIpAddress[0], size_needed);

    // Конвертация wstring в LPCWSTR
    const WCHAR* wIpAddressPtr = wIpAddress.c_str();

    // Используем InetPton с WCHAR*
    InetPtonW(AF_INET, wIpAddressPtr, &clientService.sin_addr.s_addr);
    clientService.sin_port = htons(port);

    if (connect(m_socket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
        std::string errorStr = "connect() failed: " + std::to_string(WSAGetLastError());
        throw std::runtime_error(errorStr);
        WSACleanup();
    }

    m_receiverThread = std::thread(&ClientSide::receive, this);
}


bool ClientSide::authorizeClient(std::string login, std::string password) {
    using namespace req;
    SenderData data(login);
    data.password = password;
    data.name = "";
    data.needsRegistration = true;
    std::string msg = data.serialize();
    sendToServer(msg);

    std::string s = "GET_USER_INFO" + login;
    sendToServer(s);

    while (m_isAuthorized == NOT_STATED) {
        continue;
    }
    if (m_isAuthorized == AUTHORIZED) {
        return true;
    }
    else {
        return false;
    }
}


bool ClientSide::registerClient(std::string login, std::string password, std::string name) {
    using namespace req;
 
    m_my_data.login = login;
    m_my_data.password = password;
    m_my_data.name = name;
    m_my_data.needsRegistration = true;
    std::string msg = m_my_data.serialize();

    sendToServer(msg);
    while (m_isAuthorized == NOT_STATED) {
        continue;
    }
    if (m_isAuthorized == AUTHORIZED) {
        return true;
    }
    else {
        return false;
    }
}

bool ClientSide::createChatWith(std::string receiverLogin) {
    using namespace req;
    Packet pack;
    pack.isNewChat = true;
    ReceiverData rd(receiverLogin);
    rd.login = receiverLogin;
    pack.receiver = rd;
    std::string msg = pack.serialize();
    Chat newChat;
    m_vec_chats.push_back(newChat);
    send(m_socket, msg.c_str(), msg.length(), 0);
    while (m_vec_chats.back().getState() == NOT_STATED) {
        continue;
    }
    if (m_vec_chats.back().getState() == ALLOWED) {
        m_vec_chats.back().setReceiver(receiverLogin);
        return true;
    }
    else {
        m_vec_chats.erase(m_vec_chats.end() - 1);
        return false;
    }
}


std::vector<Chat>::iterator ClientSide::getChat(std::string receiverLogin) {
    auto it = std::find_if(m_vec_chats.begin(), m_vec_chats.end(), [&receiverLogin](const Chat& chat) {
        return chat.getReceiver().login == receiverLogin;
        });
    if (it == m_vec_chats.end()) {
        return m_vec_chats.end();
    }
    else {
        return it;
    }
}


req::Packet ClientSide::getUserDataByLogin(std::string login) {
    std::string s = "GET_NAME" + login;
    sendToServer(s);

    while (m_currentResponse != ServerResponse::NAME_FOUND) {
        if (m_currentResponse == ServerResponse::NAME_NOT_FOUND) {
            return req::Packet();
        }
    }

    return m_packet_tmp;
    
}


void ClientSide::receive() {
    char buffer[200];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int byteCount = recv(m_socket, buffer, sizeof(buffer), 0);

        if (byteCount > 0) {
            std::string receivedString(buffer, byteCount); 

            if (receivedString == "AUTHORIZATION_SUCCESS") {
                m_isAuthorized = AUTHORIZED;
                continue;
            }
            if (receivedString == "REGISTRATION_SUCCESS") {
                m_isAuthorized = AUTHORIZED;
                continue;
            }
            if (receivedString == "AUTHORIZATION_FAIL" || receivedString == "REGISTRATION_FAIL") {
                m_isAuthorized = NOT_AUTHORIZED;
                continue;
            }
            if (receivedString == "CHAT_CREATE_SUCCESS") {
                Chat& ch = m_vec_chats.back();
                ch.setState(ALLOWED);
                continue;
            }
            if (receivedString == "CHAT_CREATE_FAIL") {
                Chat& ch = m_vec_chats.back();
                ch.setState(FORBIDDEN);
                continue;
            }
            if (receivedString.substr(0, 15) == "USER_INFO_FOUND") {
                req::Packet data = req::Packet::deserialize(receivedString.substr(16));
                m_my_data = data.sender;
                continue;
            }
            if (receivedString.substr(0, 15) == "USER_INFO_NOT_FOUND") {
                continue;
            }

            if (receivedString.substr(0, 14) == "NAME_NOT_FOUND") {
                m_currentResponse = ServerResponse::NAME_NOT_FOUND;
            }

            if (receivedString.substr(0, 10) == "NAME_FOUND") {
                using namespace req;
                m_packet_tmp = Packet::deserialize(receivedString.substr(11));

            }

            else {
                req::Packet pack = req::Packet::deserialize(receivedString);
                auto it = getChat(pack.sender.login);
                if (it != m_vec_chats.end()) {
                    std::vector<std::string>& receivedMsgs = it->getReceivedMsgVec();
                    receivedMsgs.emplace_back(pack.msg);
                    it->setLastIncomeMsg(pack.msg);
                }
                else {
                    Chat ch;
                    ch.setState(ALLOWED);
                    ch.setReceiver(pack.sender.login);
                    ch.getReceivedMsgVec().emplace_back(pack.msg);
                    ch.setLastIncomeMsg(pack.msg);
                    m_vec_chats.push_back(ch);
                }
            }
        }
        else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            break; 
        }
    }
}

void ClientSide::sendMessage(req::Packet pack) {
    std::string msg = pack.serialize();
    int byteCount = send(m_socket, msg.c_str(), sizeof(msg.c_str()), 0);
    if (byteCount <= 0) {
        throw std::runtime_error("message not send!");
        WSACleanup();
    }
}

void ClientSide::sendToServer(std::string msg) {
    int byteCount = send(m_socket, msg.c_str(), msg.length(), 0);
    if (byteCount <= 0) {
        throw std::runtime_error("message not send!");
        WSACleanup();
    }
}
