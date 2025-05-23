#include"chat.h"
#include"photo.h"
#include"database.h"

QJsonObject Chat::serialize(const std::string& myLogin, const Database& db) const {
    db.saveMessages(myLogin, m_friend_login, m_vec_messages);
    QJsonObject jsonObject;
    jsonObject["friend_last_seen"] = QString::fromStdString(m_friend_last_seen);
    jsonObject["friend_login"] = QString::fromStdString(m_friend_login);
    jsonObject["friend_name"] = QString::fromStdString(m_friend_name);
    jsonObject["is_friend_has_photo"] = m_is_friend_has_photo;
    jsonObject["friend_photo"] = QString::fromStdString(m_friend_photo->getPhotoPath());
    jsonObject["index_at_layout"] = QString::fromStdString(std::to_string(m_index_at_layout));
    return jsonObject;
}

Chat* Chat::deserialize(const std::string& myLogin, const QJsonObject& jsonObject, const Database& db) {
    Chat* chat = new Chat();
    chat->m_friend_login = jsonObject["friend_login"].toString().toStdString();
    db.loadMessages(myLogin, chat->m_friend_login, chat->m_vec_messages);
    chat->m_friend_name = jsonObject["friend_name"].toString().toStdString();
    chat->m_is_friend_has_photo = jsonObject["is_friend_has_photo"].toBool();

    std::string photoPath = jsonObject["friend_photo"].toString().toStdString();
    Photo* photo = new Photo(photoPath);
    chat->m_friend_photo = photo;

    chat->m_index_at_layout = jsonObject["index_at_layout"].toString().toInt();
    return chat;
}

const std::vector<Message*> Chat::getUnreadSendMessagesVec() const {
    std::vector<Message*> vec;
    for (auto message : m_vec_messages) {
        if (message->getIsSend() && !message->getIsRead()) {
            vec.push_back(message);
        }
    }
    return vec;
}

const std::vector<Message*> Chat::getUnreadReceiveMessagesVec() const {
    std::vector<Message*> vec;
    for (auto message : m_vec_messages) {
        if (!message->getIsSend() && !message->getIsRead()) {
            vec.push_back(message);
        }
    }
    return vec;
}