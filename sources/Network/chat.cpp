#include "chat.h"

State Chat::getState() const { return isAllowed; };
void Chat::setState(State value) { isAllowed = value; };

req::ReceiverData Chat::getReceiver() const { return m_receiver_data; };
void Chat::setReceiver(std::string receiverLogin) { m_receiver_data = receiverLogin; };

std::vector<std::string>& Chat::getSendMsgVec() { return m_vec_send_messages; };
std::vector<std::string>& Chat::getReceivedMsgVec() { return m_vec_received_messages; };

void Chat::setLastIncomeMsg(const std::string& msg) { m_last_incoming_msg = msg; };
std::string& Chat::getLastIncomeMsg() { return m_last_incoming_msg; };