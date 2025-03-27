#pragma once
#include<vector>
#include<QJsonObject>


#include"message.h"

class Database;
class Photo;

class Chat {
public:
	Chat() : m_is_friend_has_photo(false), m_index_at_layout(10000) {}

	std::vector<Message*>& getMessagesVec() { return m_vec_messages; }
	const std::vector<Message*> getUnreadSendMessagesVec() const;
	const std::vector<Message*> getUnreadReceiveMessagesVec() const;

	void setFriendLogin(const std::string& friendLogin) { m_friend_login = friendLogin; }
	const std::string& getFriendLogin() const { return m_friend_login; }

	void setLastIncomeMsg(const std::string& lastIncomeMessage) { m_last_incoming_message = lastIncomeMessage; }
	const std::string& getLastMessage() const { return m_last_incoming_message; }

	void setFriendName(const std::string& name) { m_friend_name = name; }
	const std::string& getFriendName() const { return m_friend_name; }

	void setFriendLastSeen(const std::string& lastSeen) { m_friend_last_seen = lastSeen; }
	const std::string& getFriendLastSeen() const { return m_friend_last_seen; }

	void setIsFriendHasPhoto(const bool isHasPhoto) { m_is_friend_has_photo = isHasPhoto; }
	const bool getIsFriendHasPhoto() const { return m_is_friend_has_photo; }

	void setFriendPhoto(Photo* photo) { m_friend_photo = photo; }
	const Photo* getFriendPhoto() const { return m_friend_photo; }

	void setLayoutIndex(int index) { m_index_at_layout = index; }
	const int getLayoutIndex() const { return m_index_at_layout; }

	QJsonObject serialize(const Database& db) const;
	static Chat* deserialize(const QJsonObject& jsonObject, const Database& db);

private:
	std::vector<Message*>	 m_vec_messages;
	std::string				 m_friend_last_seen;
	std::string				 m_friend_login;
	std::string				 m_friend_name;
	std::string				 m_last_incoming_message;
	bool					 m_is_friend_has_photo;
	Photo*					 m_friend_photo;
	int						 m_index_at_layout;
};