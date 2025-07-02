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
        jsonObject["friend_last_seen"] = QString::fromStdString(base64_encode(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_last_seen)));
        jsonObject["friend_login"] = QString::fromStdString(base64_encode(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_login)));
        jsonObject["friend_name"] = QString::fromStdString(base64_encode(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_name)));
        jsonObject["last_message"] = QString::fromStdString(base64_encode(utility::AESEncrypt(m_AESE_chat_configKey, m_last_received_or_sent_message)));
        jsonObject["has_photo"] = QString::fromStdString(base64_encode(utility::AESEncrypt(m_AESE_chat_configKey, (m_is_friend_has_photo == true ? "1" : "0"))));
        jsonObject["layout_index"] = QString::fromStdString(base64_encode(utility::AESEncrypt(m_AESE_chat_configKey, std::to_string(m_index_at_layout))));
        
        if (m_friend_photo) {
            jsonObject["photo_path"] = QString::fromStdString(base64_encode(utility::AESEncrypt(m_AESE_chat_configKey, m_friend_photo->getPhotoPath())));
        }

        std::string encryptedm_AESE_chat_configKey = utility::RSAEncryptKey(myPublicKey, m_AESE_chat_configKey);
        jsonObject["chat_config_key"] = QString::fromStdString(base64_encode(encryptedm_AESE_chat_configKey));

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
        chat->m_AESE_chat_configKey = utility::RSADecryptKey(myPrivateKey, base64_decode(jsonObject["chat_config_key"].toString().toStdString()));
        chat->m_friend_login = utility::AESDecrypt(chat->m_AESE_chat_configKey, base64_decode(jsonObject["friend_login"].toString().toStdString()));
        chat->m_friend_name = utility::AESDecrypt(chat->m_AESE_chat_configKey, base64_decode(jsonObject["friend_name"].toString().toStdString()));
        chat->m_friend_last_seen = utility::AESDecrypt(chat->m_AESE_chat_configKey, base64_decode(jsonObject["friend_last_seen"].toString().toStdString()));
        chat->m_last_received_or_sent_message = utility::AESDecrypt(chat->m_AESE_chat_configKey, base64_decode(jsonObject["last_message"].toString().toStdString()));
        chat->m_is_friend_has_photo = (utility::AESDecrypt(chat->m_AESE_chat_configKey, base64_decode(jsonObject["has_photo"].toString().toStdString())) == "1");
        chat->m_index_at_layout = std::stoi(utility::AESDecrypt(chat->m_AESE_chat_configKey, base64_decode(jsonObject["layout_index"].toString().toStdString())));

        if (jsonObject.contains("photo_path")) {
            std::string encryptedPath = base64_decode(jsonObject["photo_path"].toString().toStdString());
            std::string path = (utility::AESDecrypt(chat->m_AESE_chat_configKey, encryptedPath));
            chat->m_friend_photo = new Photo(myPrivateKey, path);
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