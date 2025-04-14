#include"message.h"

#include <sstream>
#include <string>

std::string Message::serialize() const {
    std::ostringstream oss;
    oss << m_message << "|"
        << m_timestamp << "|"
        << m_id << "|"
        << (m_is_from_me ? "true" : "false") << "|"
        << (m_is_read ? "true" : "false");
    return oss.str();
}

Message* Message::deserialize(const std::string& data) {
    std::istringstream iss(data);
    std::string message, timestamp, id, isFromMeStr, isReadStr;

    std::getline(iss, message, '|');
    std::getline(iss, timestamp, '|');
    std::getline(iss, id, '|');
    std::getline(iss, isFromMeStr, '|');
    std::getline(iss, isReadStr);

    bool isFromMe = (isFromMeStr == "true");
    bool isRead = (isReadStr == "true");

    return new Message(std::move(message), std::move(timestamp), id, isFromMe, isRead);
}
