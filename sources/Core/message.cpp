#include"message.h"

#include"utility.h"
#include <sstream>


Message::Message(const std::string& message,
    const std::string& timestamp,
    const std::string& id,
    bool isFromMe,
    bool isRead)
    : m_message(message),
    m_timestamp(timestamp),
    m_id(id),
    m_isFromMe(isFromMe),
    m_isRead(isRead),
    m_isNeedToRetry(false),
    m_isSending(false)
{
}

std::string Message::serialize() const {
    std::ostringstream oss;

    oss << utility::escape(m_message) << "|"
        << utility::escape(m_timestamp) << "|"
        << utility::escape(m_id) << "|"
        << (m_isFromMe ? "true" : "false") << "|"
        << (m_isRead ? "true" : "false") << "|"
        << ((m_isSending || m_isNeedToRetry) ? "true" : "false") << "|";

    return oss.str();
}

std::shared_ptr<Message> Message::deserialize(const std::string& data) {
    std::vector<std::string> tokens;
    bool escape = false;
    std::string current;
    for (char c : data) {
        if (escape) {
            current += c;
            escape = false;
        }
        else if (c == '\\') {
            escape = true;
        }
        else if (c == '|') {
            tokens.push_back(current);
            current.clear();
        }
        else {
            current += c;
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }

    const size_t REQUIRED_TOKENS = 6;
    if (tokens.size() < REQUIRED_TOKENS) {
        return nullptr;
    }

    std::shared_ptr<Message> msg = std::make_shared<Message>();
    size_t index = 0;

    try {
        msg->m_message = utility::unescape(tokens[index++]);
        msg->m_timestamp = utility::unescape(tokens[index++]);
        msg->m_id = utility::unescape(tokens[index++]);
        msg->m_isFromMe = (tokens[index++] == "true");
        msg->m_isRead = (tokens[index++] == "true");
        msg->m_isNeedToRetry = (tokens[index++] == "true");
        msg->m_isSending = false;
    }
    catch (...) {
        return nullptr;
    }

    return msg;
}


const std::string& Message::getMessage() const {
    return m_message;
}

void Message::setMessage(const std::string& message) {
    m_message = message;
}

const std::string& Message::getTimestamp() const {
    return m_timestamp;
}

void Message::setTimestamp(const std::string& timestamp) {
    m_timestamp = timestamp;
}

const std::string& Message::getId() const {
    return m_id;
}

void Message::setId(const std::string& id) {
    m_id = id;
}

bool Message::getIsFromMe() const {
    return m_isFromMe;
}

void Message::setIsFromMe(bool isFromMe) {
    m_isFromMe = isFromMe;
}

bool Message::getIsRead() const {
    return m_isRead;
}

void Message::setIsRead(bool isRead) {
    m_isRead = isRead;
}

bool Message::getIsNeedToRetry() const {
    return m_isNeedToRetry;
}

void Message::setIsNeedToRetry(bool isNeedToRetry) {
    m_isNeedToRetry = isNeedToRetry;
}

bool Message::getIsSending() const {
    return m_isSending;
}

void Message::setIsSending(bool isSending) {
    m_isSending = isSending;
}
