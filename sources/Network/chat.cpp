#include "chat.h"

const std::string& Chat::getFriendLogin() const { return m_friend_login; };
void Chat::setFriendLogin(const std::string& friendLogin) {m_friend_login  = friendLogin; };

const std::string& Chat::getLastIncomeMessage() const { return m_last_incoming_message; };
void Chat::setLastIncomeMsg(const std::string& message) { m_last_incoming_message = message; };

const std::string& Chat::getFriendName() const { return m_friend_name; }
void Chat::setFriendName(const std::string& name) {m_friend_name = name;}

void Chat::setFriendLastSeen(const std::string& lastSeen) { m_friend_last_seen = lastSeen; }
const std::string& Chat::getFriendLastSeen() const { return m_friend_last_seen; }