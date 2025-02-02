#pragma once

#include<iostream>
#include<vector>
#include<algorithm>
#include<thread>
#include<mutex>
#include<windows.h>
#include <QObject>
#include <QString>

#include"user.h"
#include"request.h"
#include"chat.h"

/*
class RequiredFromServerUserInfo {
public:
	RequiredFromServerUserInfo() : m_isHasPhoto(false), m_isOnline(false), m_isInfo(false) {}

	const std::string& getLogin() const { return m_user_login; }
	void setLogin(const std::string& login) { m_user_login = login; }

	const std::string& getName() const { return m_user_name; }
	void setName(const std::string& name) { m_user_name = name; }

	const std::string& getLastSeen() const { return m_last_seen; }
	void setLastSeen(const std::string& lastSeen) { m_last_seen = lastSeen; }

	const bool getIsOnline() const { return m_isOnline; }
	void setIsOnline(const bool isOnline) { m_isOnline = isOnline; }

	const bool getIsHasPhoto() const { return m_isHasPhoto; }
	void setIsHasPhoto(const bool isHasPhoto) { m_isHasPhoto = isHasPhoto; }

	const Photo& getPhoto() const { return m_user_photo; }
	void setPhoto(const Photo& photo) { m_user_photo = photo; }

	const bool isInfo() const { return m_isInfo; }
	void setIsInfoFlag(const bool isInfo) { m_isInfo = isInfo; }

private:
	bool		m_isInfo;
	std::string m_user_login;
	std::string m_user_name;
	std::string	m_last_seen;
	Photo       m_user_photo;
	bool		m_isOnline;
	bool		m_isHasPhoto;
};
*/



class MessagingAreaComponent;
class ChatsListComponent;
class ChatsWidget;

class ClientSide {
public:
	ClientSide(ChatsWidget* chatsWidget) : m_socket(-1), m_server_IpAddress(""), 
		m_server_port(-1), m_is_authorized(AuthorizationState::NOT_STATED),
		 m_chatsWidget(chatsWidget) {}

	void init();
	void connectTo(std::string ipAddress, int port);
	bool authorizeClient(std::string login, std::string password);
	bool registerClient(std::string login, std::string password, std::string name);
	Chat* createChatWith(const std::string& friendLogin);
	void sendMessage(Chat* chat, std::string message, std::string timeStamp, double id);
	void sendMessagesReadPacket(std::string friendLogin, std::vector<double> messagesReadIds);
	void updateStatusInChatsWidget(MessagingAreaComponent* messagingAreaComponent, ChatsListComponent* chatsListComponent, rcv::FriendStatePacket packet);

	const User& getMyInfo() const { return m_my_user_data; }
	void setMyInfo(const User user) { m_my_user_data = user; }
	void setChatsWidget(ChatsWidget* chatsWidget) { m_chatsWidget = chatsWidget; }

	std::vector<Chat*>& getMyChatsVec() { return m_vec_chats; }

	QJsonObject serialize() const;
	static ClientSide* deserialize(const QJsonObject& jsonObject, ChatsWidget* chatsWidget);

private:
	enum class AuthorizationState {
		NOT_STATED,
		AUTHORIZED,
		NOT_AUTHORIZED
	};

	std::mutex			m_mtx;
	int					m_socket;
	std::string			m_server_IpAddress;
	int					m_server_port;
	std::thread			m_receiverThread;
	std::vector<Chat*>	m_vec_chats;
	AuthorizationState	m_is_authorized;

	User m_my_user_data;
	ChatsWidget* m_chatsWidget;
	/*
	responses (shared tmp data, can be updated by createChat, or incoming message (not always))
	RequiredFromServerUserInfo	m_required_friend_info;
	*/

private:
	void receive();
	void sendPacket(Packet& packet);
};