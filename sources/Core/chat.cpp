#include "chat.h"
#include "avatar.h"
#include "database.h"
#include "utility.h"

QJsonObject Chat::serialize(const CryptoPP::RSA::PublicKey& myPublicKey, 
                          const std::string& myLogin, 
                          const Database& db) 
{
    try {
        db.saveMessages(myPublicKey, myLogin, m_friend_login, m_vec_messages);

        utility::generateAESKey(m_AESE_chat_configKey);

        QJsonObject jsonObject;
        jsonObject["friend_last_seen"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_last_seen));
        jsonObject["friend_login"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_login));
        jsonObject["friend_name"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_name));
        if (m_last_received_or_sent_message != "") {
            jsonObject["last_message"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, m_last_received_or_sent_message));
        }
        jsonObject["has_avatar"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, (m_is_friend_has_avatar == true ? "1" : "0")));
        jsonObject["layout_index"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, std::to_string(m_index_at_layout)));
        
        if (m_friend_avatar) {
            jsonObject["avatar_path"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_avatar->getPhotoPath()));
        }

        jsonObject["chat_config_key"] = QString::fromStdString(utility::RSAEncryptKey(myPublicKey, m_AESE_chat_configKey));

        return jsonObject;
    } 
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Encryption failed: " + std::string(e.what()));
    }
}

Chat* Chat::deserialize(const CryptoPP::RSA::PrivateKey& myPrivateKey,
                      const std::string& myLogin,
                      const QJsonObject& jsonObject,
                      const Database& db, CryptoPP::SecByteBlock avatarsKey)
{
    try {
        Chat* chat = new Chat();
        chat->m_AESE_chat_configKey = utility::RSADecryptKey(myPrivateKey, jsonObject["chat_config_key"].toString().toStdString());
        chat->m_friend_login = utility::AESDecrypt(chat->m_AESE_chat_configKey, jsonObject["friend_login"].toString().toStdString());
        chat->m_friend_name = utility::AESDecrypt(chat->m_AESE_chat_configKey, jsonObject["friend_name"].toString().toStdString());
        chat->m_friend_last_seen = utility::AESDecrypt(chat->m_AESE_chat_configKey, jsonObject["friend_last_seen"].toString().toStdString());
        if (jsonObject.contains("last_message")) {
            std::string lastMessage = jsonObject["last_message"].toString().toStdString();
            chat->m_last_received_or_sent_message = utility::AESDecrypt(chat->m_AESE_chat_configKey, lastMessage);
        }
        chat->m_is_friend_has_avatar = (utility::AESDecrypt(chat->m_AESE_chat_configKey, jsonObject["has_avatar"].toString().toStdString())) == "1";
        chat->m_index_at_layout = std::stoi(utility::AESDecrypt(chat->m_AESE_chat_configKey, jsonObject["layout_index"].toString().toStdString()));

        if (jsonObject.contains("avatar_path")) {
            std::string encryptedPath = jsonObject["avatar_path"].toString().toStdString();
            std::string path = (utility::AESDecrypt(chat->m_AESE_chat_configKey, encryptedPath));
            chat->m_friend_avatar = new Avatar(avatarsKey, path);
        }

        db.loadMessages(myPrivateKey, myLogin, chat->m_friend_login, chat->m_vec_messages);

        return chat;
    } 
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Decryption failed: " + std::string(e.what()));
    }
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