#include<winsock2.h>
#include<ws2tcpip.h>

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

void ClientSide::receive() {
    bool isReceivingNextPacketSize = true;
    int bufferSize = 40;
    while (true) {
        char* buffer = new char[bufferSize];
        int byteCount = recv(m_socket, buffer, 40, 0);

        if (byteCount > 0 && isReceivingNextPacketSize) {
            std::string bufferStr(buffer, byteCount);
            auto pair = rcv::parseResponse(bufferStr);
            bufferSize = std::stoi(pair.second);
            isReceivingNextPacketSize = false;
            continue;
        }

        if (byteCount > 0 && !isReceivingNextPacketSize) {
            std::string receivedString(buffer, bufferSize);
            auto pair = rcv::parseResponse(receivedString);

            if (pair.first == Response::AUTHORIZATION_SUCCESS) {
                m_is_authorized = AuthorizationState::AUTHORIZED;
            }
            else if (pair.first == Response::AUTHORIZATION_FAIL) {
                m_is_authorized = AuthorizationState::NOT_AUTHORIZED;
            }
            else if (pair.first == Response::REGISTRATION_SUCCESS) {
                m_is_authorized = AuthorizationState::AUTHORIZED;
            }
            else if (pair.first == Response::REGISTRATION_FAIL) {
                m_is_authorized = AuthorizationState::NOT_AUTHORIZED;
            }
            else if (pair.first == Response::CHAT_CREATE_SUCCESS) {
                rcv::ChatSuccessPacket pack = rcv::ChatSuccessPacket::deserialize(pair.second);
                Chat* chat = m_vec_chats.back();
                chat->setFriendLastSeen(pack.getUserInfoPacket().getLastSeen());
                chat->setFriendLogin(pack.getUserInfoPacket().getLogin());
                chat->setFriendName(pack.getUserInfoPacket().getName());
                chat->setFriendLastSeen(pack.getUserInfoPacket().getLastSeen());
                chat->setIsFriendHasPhoto(pack.getUserInfoPacket().getIsHasPhoto());
                chat->setFriendPhoto(pack.getUserInfoPacket().getPhoto());
                chat->setChatState(ChatState::ALLOWED);
            }
            else if (pair.first == Response::CHAT_CREATE_FAIL) {
                m_vec_chats.back()->setChatState(ChatState::FORBIDDEN);
            }
            else if (pair.first == Response::USER_INFO_FOUND) {
                rpl::UserInfoPacket packet = rpl::UserInfoPacket::deserialize(pair.second);
                /*
                m_required_friend_info.setLogin(packet.getLogin());
                m_required_friend_info.setIsHasPhoto(packet.getIsHasPhoto());
                m_required_friend_info.setIsInfoFlag(true);
                m_required_friend_info.setIsOnline(packet.getIsOnline());
                m_required_friend_info.setLastSeen(packet.getLastSeen());
                m_required_friend_info.setPhoto(packet.getPhoto());
                m_required_friend_info.setName(packet.getName());
                */
            }
            else if (pair.first == Response::USER_INFO_NOT_FOUND) {
                //m_required_friend_info.setIsInfoFlag(false);
            }
            else if (pair.first == Response::FRIEND_STATE_CHANGED) {
                rcv::FriendStatePacket packet = rcv::FriendStatePacket::deserialize(pair.second);
                auto& friendsVec = m_my_user_data.getUserFriendsVec();

                auto it = std::find_if(friendsVec.begin(), friendsVec.end(), [&packet](const auto& pairFriend) {
                    return packet.getLogin() == pairFriend.first;
                    });

                it->second = packet.getLastSeen(); 
                
            }
            else if (pair.first == Response::ALL_FRIENDS_STATES) {
                rcv::FriendsStatusesPacket packet = rcv::FriendsStatusesPacket::deserialize(pair.second);
                auto& friendsVec = m_my_user_data.getUserFriendsVec();
                friendsVec = packet.getVecStatuses();
            }
            else if (pair.first == Response::MESSAGE) {
                rpl::Message message = rpl::Message::deserialize(pair.second);
                auto it = std::find_if(m_vec_chats.begin(), m_vec_chats.end(), [&message](Chat* chat) {
                    return message.getFriendInfo().getLogin() == chat->getFriendLogin();
                    });

                if (it == m_vec_chats.end()) {
                    Chat* newChat = new Chat;

                    Msg* msg = new Msg;
                    msg->setId(message.getId());
                    msg->setMessage(message.getMessage());

                    newChat->setFriendLogin(message.getFriendInfo().getLogin());
                    newChat->setFriendLastSeen(message.getFriendInfo().getLastSeen());
                    newChat->setFriendName(message.getFriendInfo().getName());
                    newChat->setIsFriendHasPhoto(message.getFriendInfo().getIsHasPhoto());
                    newChat->setLastIncomeMsg(message.getMessage());
                    newChat->setFriendPhoto(message.getFriendInfo().getPhoto());
                    newChat->getReceivedMsgVec().push_back(msg);
                    newChat->getNotReadReceivedMsgVec().push_back(msg->getId());
                    m_vec_awaited_chats.push_back(newChat);
                }
                else {
                    Chat* foundChat = *it;

                    Msg* msg = new Msg;
                    msg->setId(message.getId());
                    msg->setMessage(message.getMessage());

                    foundChat->getReceivedMsgVec().push_back(msg);
                    foundChat->getNotReadReceivedMsgVec().push_back(msg->getId());
                    foundChat->setLastIncomeMsg(message.getMessage());
                }
            }
        }
        else {
            std::cerr << "recv failed with error: " << WSAGetLastError() << std::endl;
            isReceivingNextPacketSize = true;
            delete buffer;
            break; 
        }

        isReceivingNextPacketSize = true;
        delete buffer;
    }
}

void ClientSide::sendMessage(Chat* chat, std::string msg) {
    rpl::Message message;
    message.setId(m_messages_id_counter);
    m_messages_id_counter++;
    message.setMessage(msg);

    rpl::UserInfoPacket myInfo;
    myInfo.setIsHasPhoto(m_my_user_data.getIsHasPhoto());
    myInfo.setIsOnline(true);
    myInfo.setLastSeen(m_my_user_data.getLastSeen());
    myInfo.setLogin(m_my_user_data.getLogin());
    myInfo.setName(m_my_user_data.getName());
    myInfo.setPhoto(m_my_user_data.getPhoto());
    message.setMyInfo(myInfo);

    rpl::UserInfoPacket friendInfo;
    friendInfo.setIsHasPhoto(chat->getIsFriendHasPhoto());
    friendInfo.setIsOnline(false); // doesnt matter
    friendInfo.setLastSeen(chat->getFriendLastSeen());
    friendInfo.setLogin(chat->getFriendLogin());
    friendInfo.setName(chat->getFriendName());
    friendInfo.setPhoto(chat->getFriendPhoto());
    message.setMyInfo(friendInfo);
    message.setFriendInfo(friendInfo);

    std::string serializedMessage = message.serialize();
    SizePacket sizePacket;
    sizePacket.setData(serializedMessage);
    std::string serializedSizeMessage = sizePacket.serialize();
    send(m_socket, serializedSizeMessage.c_str(), strlen(serializedSizeMessage.c_str()), 0);
    send(m_socket, serializedMessage.c_str(), strlen(serializedMessage.c_str()), 0);
}

bool ClientSide::authorizeClient(std::string login, std::string password) {
    snd::AuthorizationPacket pack;
    pack.setLogin(login);
    pack.setPassword(password);
    sendPacket(pack);
 
    while (m_is_authorized == AuthorizationState::NOT_STATED) {
        continue;
    }
    if (m_is_authorized == AuthorizationState::AUTHORIZED) {
        return true;
    }
    else {
        return false;
    }
}

bool ClientSide::registerClient(std::string login, std::string password, std::string name) {
    snd::RegistrationPacket pack;
    pack.setLogin(login);
    pack.setName(name);
    pack.setPassword(password);
    sendPacket(pack);

    while (m_is_authorized == AuthorizationState::NOT_STATED) {
        continue;
    }
    if (m_is_authorized == AuthorizationState::AUTHORIZED) {
        return true;
    }
    else {
        return false;
    }
}

Chat* ClientSide::createChatWith(const std::string& friendLogin) {
    Chat* chat = new Chat;
    m_vec_chats.push_back(chat);

    snd::CreateChatPacket packet;
    packet.setMyLogin(m_my_user_data.getLogin());
    packet.setFriendLogin(friendLogin);
    sendPacket(packet);

    while (m_vec_chats.back()->getChatState() == ChatState::NOT_STATED) {}

    if (m_vec_chats.back()->getChatState() == ChatState::ALLOWED) {
        return chat;
    }
    else if (m_vec_chats.back()->getChatState() == ChatState::FORBIDDEN) {
        throw std::runtime_error("can not create chat");
    }
}

void ClientSide::sendPacket(Packet& packet) {
    std::string serializedPacket = packet.serialize();
    SizePacket sizePacket;
    sizePacket.setData(serializedPacket);
    std::string serializedSizePacket = sizePacket.serialize();
    send(m_socket, serializedSizePacket.c_str(), strlen(serializedSizePacket.c_str()), 0);
    send(m_socket, serializedPacket.c_str(), strlen(serializedPacket.c_str()), 0);
}