#include"packetsBuilder.h"
#include"message.h"
#include"packetType.h"
#include"utility.h"



//GET
std::string PacketsBuilder::getLoginAndPasswordHashPacket(const std::string& loginHash, const std::string& passwordHash) {
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[LOGIN_HASH] = loginHash;
    jsonObject[PASSWORD_HASH] = passwordHash;

    return jsonObject.dump();
}

nlohmann::json PacketsBuilder::getFriendsUIDsArray(const std::vector<std::string>& friendsUIDsVec) {
    nlohmann::json friendsUIDsArray = nlohmann::json::array();
    for (const auto& friendUID : friendsUIDsVec) {
        friendsUIDsArray.push_back(friendUID);
    }

    return friendsUIDsArray;
}

std::string PacketsBuilder::getReconnectPacket(const std::string& myLoginHash, const std::string& passwordHash) {
    return getLoginAndPasswordHashPacket(myLoginHash, passwordHash);
}

std::string PacketsBuilder::getAuthorizationPacket(const std::string& myLoginHash, const std::string& passwordHash) {
    return getLoginAndPasswordHashPacket(myLoginHash, passwordHash);
}

std::string PacketsBuilder::getRegistrationPacket(const std::string& myLoginHash, const std::string& passwordHash) {
    return getLoginAndPasswordHashPacket(myLoginHash, passwordHash);
}

std::string PacketsBuilder::getSendMyNameAndLoginPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& login, const std::string& name)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);
    
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[LOGIN] = utility::AESEncrypt(key, login);
    jsonObject[NAME] = utility::AESEncrypt(key, name);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getCreateChatPacket(const std::string& myUID,
    const std::string& supposedFriendLoginHash)
{
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[SUPPOSED_FRIEND_LOGIN_HASH] = supposedFriendLoginHash;

    return jsonObject.dump();
}

std::string PacketsBuilder::getUpdateMyNamePacket(const CryptoPP::RSA::PublicKey& serverPublicKey,
    const std::string& myUID,
    const std::string& newName,
    const std::vector<std::string>& friendsUIDsVec)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[NEW_NAME] = utility::AESEncrypt(key, newName);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;
    jsonObject[FRIENDS_UIDS] = getFriendsUIDsArray(friendsUIDsVec);

    return jsonObject.dump();
}

std::string PacketsBuilder::getUpdateMyPasswordPacket(
    const std::string& myUID,
    const std::string& newPasswordHash)
{
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[NEW_PASSWORD_HASH] = newPasswordHash;

    return jsonObject.dump();
}


std::string  PacketsBuilder::getUpdateMyLoginPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& newLoginHash, const std::string& newLogin) 
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[NEW_LOGIN_HASH] = newLoginHash;
    jsonObject[NEW_LOGIN] = utility::AESEncrypt(key, newLogin);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}









std::string PacketsBuilder::getLoadUserInfoPacket(const std::string& myUID, const std::string& friendUID) {
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[FRIEND_UID] = friendUID;

    return jsonObject.dump();
}


std::string PacketsBuilder::getVerifyPasswordPacket(const std::string& myUID, const std::string& passwordHash) {
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[PASSWORD_HASH] = passwordHash;

    return jsonObject.dump();
}

std::string PacketsBuilder::getLoadAllFriendsStatusesPacket(const std::string& myUID, const std::vector<std::string>& friendsUIDsVec) {
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[FRIENDS_UIDS] = getFriendsUIDsArray(friendsUIDsVec);

    return jsonObject.dump();
}

std::string PacketsBuilder::getCheckIsNewLoginAvailablePacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& newLogin)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[NEW_LOGIN] = utility::AESEncrypt(key, newLogin);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getFindUserPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& searchText) 
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[SEARCH_TEXT] = utility::AESEncrypt(key, searchText);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getSendMeFilePacket(const std::string& myUID, const std::string& fileId) {
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[FILE_ID] = fileId;

    return jsonObject.dump();
}

std::string PacketsBuilder::getPublicKeyPacket(const std::string& myUID, const CryptoPP::RSA::PublicKey& myPublicKey) {
    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[FILE_ID] = utility::serializePublicKey(myPublicKey);

    return jsonObject.dump();
}

std::string PacketsBuilder::getUpdateRequestPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& versionNumber) {
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[GET] = GET;
    jsonObject[MY_UID] = myUID;
    jsonObject[VERSION_NUMBER] = utility::AESEncrypt(key, versionNumber);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}  









//RPL
std::string PacketsBuilder::getMessagePacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, MessagePtr message)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(friendPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[RPL] = RPL;
    jsonObject[FRIEND_UID] = friendUID;
    jsonObject[MY_UID] = myUID;
    jsonObject[MESSAGE_ID] = message->getId();
    jsonObject[MESSAGE] = utility::AESEncrypt(key, message->getMessage());
    jsonObject[TIMESTAMP] = utility::AESEncrypt(key, message->getTimestamp());
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getMessageReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& messageId) {
    nlohmann::json jsonObject;
    jsonObject[RPL] = RPL;
    jsonObject[FRIEND_UID] = friendUID;
    jsonObject[MY_UID] = myUID;
    jsonObject[MESSAGE_ID] = messageId;

    return jsonObject.dump();
}

std::string PacketsBuilder::getTypingPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID,const std::string& friendUID, bool isTyping) {
    nlohmann::json jsonObject;
    jsonObject[RPL] = RPL;
    jsonObject[FRIEND_UID] = friendUID;
    jsonObject[IS_TYPING] = isTyping;

    return jsonObject.dump();
}









//BROADCAST
std::string PacketsBuilder::getStatusPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& status, const std::string& myUID, const std::vector<std::string>& friendsUIDsVec)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[BROADCAST] = BROADCAST;
    jsonObject[MY_UID] = myUID;
    jsonObject[STATUS] = utility::AESEncrypt(key, status);
    jsonObject[FRIENDS_UIDS] = getFriendsUIDsArray(friendsUIDsVec);

    return jsonObject.dump();
}