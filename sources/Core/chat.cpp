#include "chat.h"
#include "photo.h"
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
        jsonObject["has_photo"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, (m_is_friend_has_photo == true ? "1" : "0")));
        jsonObject["layout_index"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, std::to_string(m_index_at_layout)));
        jsonObject["public_key"] = QString::fromStdString(utility::serializePublicKey(m_public_key));
        
        if (m_friend_photo) {
            jsonObject["photo_path"] = QString::fromStdString(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_photo->getPhotoPath()));
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
                      const Database& db) 
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
        chat->m_is_friend_has_photo = (utility::AESDecrypt(chat->m_AESE_chat_configKey, jsonObject["has_photo"].toString().toStdString())) == "1";
        chat->m_index_at_layout = std::stoi(utility::AESDecrypt(chat->m_AESE_chat_configKey, jsonObject["layout_index"].toString().toStdString()));
        chat->m_public_key = utility::deserializePublicKey(jsonObject["public_key"].toString().toStdString());

        if (jsonObject.contains("photo_path")) {
            std::string encryptedPath = jsonObject["photo_path"].toString().toStdString();
            std::string path = (utility::AESDecrypt(chat->m_AESE_chat_configKey, encryptedPath));
            chat->m_friend_photo = new Photo(myPrivateKey, path);
            chat->m_friend_photo->loadBinaryDataFromPc();
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

void Chat::setPublicKey(CryptoPP::RSA::PublicKey key) {
    if (!utility::validatePublicKey(key)) {
        assert("Invalid public key provided");
    }

    m_public_key = key;
}

CryptoPP::RSA::PublicKey Chat::getPublicKey() {
    if (!utility::validatePublicKey(m_public_key)) {
        assert("Public key is not initialized or invalid");
    }

    return m_public_key;
}