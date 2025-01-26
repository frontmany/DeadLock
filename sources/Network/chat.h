#pragma once

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

	void setId(int id) { m_id = id; }
	const bool getId() const { return m_id; }

private:
	std::string m_message;
	int m_id;
};

class Chat {
public:
	Chat() : m_chat_state(ChatState::NOT_STATED){}

	const std::string& getFriendLogin() const;
	void setFriendLogin(const std::string& friendLogin);

	std::vector<Msg*>& getSendMsgVec();
	std::vector<int>& getNotReadSendMsgVec() { return m_vec_not_read_send_messages_id; }

	std::vector<Msg*>& getReceivedMsgVec();
	std::vector<int>& getNotReadReceivedMsgVec() { return m_vec_not_read_received_messages_id; }

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

private:

	std::vector<Msg*> m_vec_send_messages;
	std::vector<int> m_vec_not_read_send_messages_id;

	std::vector<Msg*> m_vec_received_messages;
	std::vector<int> m_vec_not_read_received_messages_id;

	ChatState				 m_chat_state;
	std::string				 m_friend_last_seen;
	std::string				 m_friend_login;
	std::string				 m_friend_name;
	std::string				 m_last_incoming_message;
	bool					 m_is_friend_has_photo;
	Photo					 m_friend_photo;
};