#include "chat.h"
#include "avatar.h"
#include "database.h"
#include "utility.h"
#include"message.h"

nlohmann::json Chat::serialize(const CryptoPP::RSA::PublicKey& myPublicKey,
    const std::string& myUID, DatabasePtr db)
{
    try {
        db->saveMessages(myPublicKey, myUID, m_friendUID, m_vecMessages);
        db->saveBlobs(myPublicKey, myUID, m_friendUID, m_vecBlobs);

        CryptoPP::SecByteBlock chatConfigKey;
        utility::generateAESKey(chatConfigKey);

        nlohmann::json jsonObject;
        jsonObject["friendUID"] = utility::AESEncrypt(chatConfigKey, m_friendUID);
        jsonObject["friendName"] = utility::AESEncrypt(chatConfigKey, m_friendName);
        jsonObject["friendLastSeen"] = utility::AESEncrypt(chatConfigKey, m_friendLastSeen);
        jsonObject["isFriendHasAvatar"] = m_isFriendHasAvatar;

        if (m_isFriendHasAvatar) {
            jsonObject["avatarPath"] = utility::AESEncrypt(chatConfigKey, m_friendAvatar->getPath());
        }

        jsonObject["friendPublicKey"] = utility::serializePublicKey(m_friendPublicKey);
        jsonObject["indexAtLayout"] = utility::AESEncrypt(chatConfigKey, std::to_string(m_indexAtLayout));

        if (!m_messageToShowInChatsList.empty()) {
            jsonObject["messageToShowInChatsList"] = utility::AESEncrypt(chatConfigKey, m_messageToShowInChatsList);
        }

        jsonObject["chatConfigKey"] = utility::RSAEncryptKey(myPublicKey, chatConfigKey);

        return jsonObject;
    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Encryption failed: " + std::string(e.what()));
    }
}

std::shared_ptr<Chat> Chat::deserialize(const CryptoPP::RSA::PrivateKey& myPrivateKey,
    const std::string& myUID,
    const nlohmann::json& jsonObject,
    DatabasePtr db,
    const CryptoPP::SecByteBlock& avatarsKey)
{
    try {
        auto chat = std::make_shared<Chat>();

        CryptoPP::SecByteBlock chatConfigKey = utility::RSADecryptKey(
            myPrivateKey,
            jsonObject["chatConfigKey"].get<std::string>()
        );

        chat->m_friendUID = utility::AESDecrypt(
            chatConfigKey,
            jsonObject["friendUID"].get<std::string>()
        );

        chat->m_friendName = utility::AESDecrypt(
            chatConfigKey,
            jsonObject["friendName"].get<std::string>()
        );

        chat->m_friendLastSeen = utility::AESDecrypt(
            chatConfigKey,
            jsonObject["friendLastSeen"].get<std::string>()
        );

        chat->m_isFriendHasAvatar = jsonObject["isFriendHasAvatar"].get<bool>();

        if (chat->m_isFriendHasAvatar) {
            std::string avatarPath = utility::AESDecrypt(
                chatConfigKey,
                jsonObject["avatarPath"].get<std::string>()
            );
            chat->m_friendAvatar = std::make_shared<Avatar>(avatarsKey, avatarPath);
        }

        chat->m_friendPublicKey = utility::deserializePublicKey(
            jsonObject["friendPublicKey"].get<std::string>()
        );

        chat->m_indexAtLayout = std::stoi(utility::AESDecrypt(
            chatConfigKey,
            jsonObject["indexAtLayout"].get<std::string>()
        ));

        if (jsonObject.contains("messageToShowInChatsList")) {
            chat->m_messageToShowInChatsList = utility::AESDecrypt(
                chatConfigKey,
                jsonObject["messageToShowInChatsList"].get<std::string>()
            );
        }

        db->loadMessages(myPrivateKey, myUID, chat->m_friendUID, chat->m_vecMessages);
        db->loadBlobs(myPrivateKey, myUID, chat->m_friendUID, chat->m_vecBlobs);

        return chat;
    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Chat deserialization failed: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error in chat deserialization: " + std::string(e.what()));
    }
}

Chat::Chat() :
    m_isFriendHasAvatar(false),
    m_indexAtLayout(100000)
{
}

// Message operations
void Chat::addMessage(MessagePtr newMessage) {
    m_vecMessages.push_back(newMessage);
    
    if (!newMessage->getIsFromMe() && !newMessage->getIsRead()) {
        m_vecUnreadReceivedMessages.push_back(newMessage);
    }
}

void Chat::markMessageAsRead(const std::string& messageUID) {
    auto it = std::find_if(m_vecMessages.begin(), m_vecMessages.end(),
        [&messageUID](const MessagePtr& msg) {
            return msg->getId() == messageUID;
        });

    if (it != m_vecMessages.end()) {
        (*it)->setIsRead(true);
    }

    m_vecUnreadReceivedMessages.erase(
        std::remove_if(m_vecUnreadReceivedMessages.begin(),
            m_vecUnreadReceivedMessages.end(),
            [&messageUID](const MessagePtr& msg) {
                return msg->getId() == messageUID;
            }),
        m_vecUnreadReceivedMessages.end()
    );
}

const std::vector<MessagePtr>& Chat::getMessagesVec() const {
    return m_vecMessages;
}

const std::vector<MessagePtr>& Chat::getUnreadReceivedMessagesVec() const {
    return m_vecUnreadReceivedMessages;
}

const std::vector<BlobPtr>& Chat::getBlobsVec() const {
    return m_vecBlobs;
}

const std::vector<BlobPtr>& Chat::getUnreadReceivedBlobsVec() const {
    return m_vecUnreadReceivedBlobs;
}

// Friend info accessors
void Chat::setFriendUID(const std::string& friendUID) {
    m_friendUID = friendUID;
}

const std::string& Chat::getFriendUID() const {
    return m_friendUID;
}

void Chat::setPublicKey(const CryptoPP::RSA::PublicKey& publicKey) {
    m_friendPublicKey = publicKey;
}

const CryptoPP::RSA::PublicKey& Chat::getPublicKey() const {
    return m_friendPublicKey;
}

void Chat::setMessageToShowInChatsList(const std::string& message) {
    m_messageToShowInChatsList = message;
}

const std::string& Chat::getMessageToShowInChatsList() const {
    return m_messageToShowInChatsList;
}

void Chat::setFriendName(const std::string& name) {
    m_friendName = name;
}

const std::string& Chat::getFriendName() const {
    return m_friendName;
}

void Chat::setFriendLastSeen(const std::string& lastSeen) {
    m_friendLastSeen = lastSeen;
}

const std::string& Chat::getFriendLastSeen() const {
    return m_friendLastSeen;
}

// Avatar management
void Chat::setIsFriendHasAvatar(bool isHasAvatar) {
    m_isFriendHasAvatar = isHasAvatar;
}

bool Chat::getIsFriendHasAvatar() const {
    return m_isFriendHasAvatar;
}

void Chat::setFriendAvatar(AvatarPtr avatar) {
    m_friendAvatar = avatar;
}

AvatarPtr Chat::getFriendAvatar() const {
    return m_friendAvatar;
}

// Layout management
void Chat::setLayoutIndex(int index) {
    m_indexAtLayout = index;
}

int Chat::getLayoutIndex() const {
    return m_indexAtLayout;
}