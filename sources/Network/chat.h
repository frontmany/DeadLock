#pragma once

#include<QJsonObject>
#include<QJsonArray>
#include<vector>
#include"request.h"
#include"photo.h"

enum class ChatState {
	NOT_STATED,
	ALLOWED,
	FORBIDDEN
};

class Msg {
public:
	Msg() {}

	const std::string& getMessage() const { return m_message; }
	void setMessage(const std::string& message) { m_message = message; }

	const std::string& getTimestamp() const { return m_timestamp; }
	void setTimestamp(const std::string& timestamp) { m_timestamp = timestamp; }

	void setId(double id) { m_id = id; }
	const double getId() const { return m_id; }


	void setIsSend(bool isSend) { m_is_send = isSend; }
	const bool getIsSend() const { return m_is_send; }

	std::string serialize() const {
		return m_message + "|" + m_timestamp + "|" + std::to_string(m_id) + "|" + (m_is_send ? "1" : "0");
	}

	static Msg deserialize(const std::string& data) {
		Msg msg;
		size_t pos1 = data.find('|');
		size_t pos2 = data.find('|', pos1 + 1);
		size_t pos3 = data.find('|', pos2 + 1);

		msg.m_message = data.substr(0, pos1);
		msg.m_timestamp = data.substr(pos1 + 1, pos2 - pos1 - 1);
		msg.m_id = std::stod(data.substr(pos2 + 1, pos3 - pos2 - 1));
		msg.m_is_send = (data.substr(pos3 + 1) == "1");

		return msg;
	}

private:
	std::string m_message;
	std::string m_timestamp;
	bool m_is_send;
	double m_id;
};

class Chat {
public:
	Chat() : m_chat_state(ChatState::NOT_STATED), m_is_friend_has_photo(false){}

	const std::string& getFriendLogin() const;
	void setFriendLogin(const std::string& friendLogin);

	std::vector<double>& getNotReadSendMsgVec() { return m_vec_not_read_send_messages_id; }
	std::vector<double>& getNotReadReceivedMsgVec() { return m_vec_not_read_received_messages_id; }

	void setLastIncomeMsg(const std::string& lastIncomeMessage);
	const std::string& getLastIncomeMessage() const;

	void setFriendName(const std::string& name);
	const std::string& getFriendName() const;

	void setFriendLastSeen(const std::string& lastSeen);
	const std::string& getFriendLastSeen() const;

	const ChatState getChatState() const { return m_chat_state; }
	void setChatState(const ChatState chatState) { m_chat_state = chatState; }

	void setIsFriendHasPhoto(const bool isHasPhoto) { m_is_friend_has_photo = isHasPhoto; }
	const bool getIsFriendHasPhoto() const { return m_is_friend_has_photo; }

	void setFriendPhoto(const Photo& photo) { m_friend_photo = photo; }
	const Photo& getFriendPhoto() const { return m_friend_photo; }

	QJsonObject serialize() const;
	static Chat* deserialize(const QJsonObject& jsonObject);

private:
	std::vector<double>	m_vec_not_read_received_messages_id;
	std::vector<double>	m_vec_not_read_send_messages_id;

	ChatState				 m_chat_state;
	std::string				 m_friend_last_seen;
	std::string				 m_friend_login;
	std::string				 m_friend_name;
	std::string				 m_last_incoming_message;
	bool					 m_is_friend_has_photo;
	Photo					 m_friend_photo;
};