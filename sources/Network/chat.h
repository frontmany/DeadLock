#pragma once
#include<vector>
#include"request.h"

enum State {
	NOT_STATED,
	ALLOWED,
	FORBIDDEN,
	AUTHORIZED,
	NOT_AUTHORIZED
};

class Chat {
public:
	State getState() const;
	void setState(State value);

	req::ReceiverData getReceiver() const;
	void setReceiver(std::string receiverLogin);

	std::vector<std::string>& getSendMsgVec();
	std::vector<std::string>& getReceivedMsgVec();

	void setLastIncomeMsg(const std::string& msg);
	std::string& getLastIncomeMsg();

private:
	std::string m_last_incoming_msg;
	std::vector<std::string> m_vec_send_messages;
	std::vector<std::string> m_vec_received_messages;
	req::ReceiverData m_receiver_data;
	State isAllowed = NOT_STATED;
};