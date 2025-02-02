#include "chat.h"

const std::string& Chat::getFriendLogin() const { return m_friend_login; };
void Chat::setFriendLogin(const std::string& friendLogin) {m_friend_login  = friendLogin; };

const std::string& Chat::getLastIncomeMessage() const { return m_last_incoming_message; };
void Chat::setLastIncomeMsg(const std::string& message) { m_last_incoming_message = message; };

const std::string& Chat::getFriendName() const { return m_friend_name; }
void Chat::setFriendName(const std::string& name) {m_friend_name = name;}

void Chat::setFriendLastSeen(const std::string& lastSeen) { m_friend_last_seen = lastSeen; }
const std::string& Chat::getFriendLastSeen() const { return m_friend_last_seen; }

QJsonObject Chat::serialize() const {
    QJsonObject chatObject;
    chatObject["friend_login"] = QString::fromStdString(m_friend_login);
    chatObject["friend_name"] = QString::fromStdString(m_friend_name);
    chatObject["friend_last_seen"] = QString::fromStdString(m_friend_last_seen);
    chatObject["is_friend_has_photo"] = m_is_friend_has_photo;

    QJsonArray notReadReceivedArray;
    for (int id : m_vec_not_read_received_messages_id) {
        notReadReceivedArray.append(id);
    }
    chatObject["not_read_received_messages_id"] = notReadReceivedArray;

    QJsonArray notReadSendArray;
    for (int id : m_vec_not_read_send_messages_id) {
        notReadSendArray.append(id);
    }
    chatObject["not_read_send_messages_id"] = notReadSendArray;
    return chatObject;
}

Chat* Chat::deserialize(const QJsonObject& jsonObject) {
    Chat* chat = new Chat();
    chat->m_friend_login = jsonObject["friend_login"].toString().toStdString();
    chat->m_friend_name = jsonObject["friend_name"].toString().toStdString();
    chat->m_friend_last_seen = jsonObject["friend_last_seen"].toString().toLongLong();
    chat->m_is_friend_has_photo = jsonObject["is_friend_has_photo"].toBool();

    QJsonArray notReadReceivedArray = jsonObject["not_read_received_messages_id"].toArray();
    for (const auto& idValue : notReadReceivedArray) {
        chat->m_vec_not_read_received_messages_id.push_back(idValue.toInt());
    }

    QJsonArray notReadSendArray = jsonObject["not_read_send_messages_id"].toArray();
    for (const auto& idValue : notReadSendArray) {
        chat->m_vec_not_read_send_messages_id.push_back(idValue.toInt());
    }

    return chat;
}