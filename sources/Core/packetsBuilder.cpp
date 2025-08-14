#include"packetsBuilder.h"
#include"packetType.h"
#include"utility.h"



// ESSENTIALS
std::string PacketsBuilder::getLoginAndPasswordHashPacket(const std::string& loginHash, const std::string& passwordHash) {
    nlohmann::json jsonObject;
    jsonObject[LOGIN_HASH] = loginHash;
    jsonObject[PASSWORD_HASH] = passwordHash;

    return jsonObject.dump();
}

std::string PacketsBuilder::getBlobAndMessageReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& id) {
    nlohmann::json jsonObject;
    jsonObject[FRIEND_UID] = friendUID;
    jsonObject[MY_UID] = myUID;
    jsonObject[BLOB_ID] = id;

    return jsonObject.dump();
}


nlohmann::json PacketsBuilder::getFriendsUIDsArray(const std::vector<std::string>& friendsUIDsVec) {
    nlohmann::json friendsUIDsArray = nlohmann::json::array();
    for (const auto& friendUID : friendsUIDsVec) {
        friendsUIDsArray.push_back(friendUID);
    }

    return friendsUIDsArray;
}


//GET
std::string PacketsBuilder::getReconnectPacket(const std::string& myLoginHash, const std::string& passwordHash) {
    return getLoginAndPasswordHashPacket(myLoginHash, passwordHash);
}

std::string PacketsBuilder::getAuthorizationPacket(const std::string& myLoginHash, const std::string& passwordHash) {
    return getLoginAndPasswordHashPacket(myLoginHash, passwordHash);
}

std::string PacketsBuilder::getRegistrationPacket(const std::string& myLoginHash, const std::string& passwordHash) {
    return getLoginAndPasswordHashPacket(myLoginHash, passwordHash);
}

std::string PacketsBuilder::getAfterRegistrationInfoPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& myLogin, const std::string& myName)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);
    
    nlohmann::json jsonObject;
    jsonObject[LOGIN] = utility::AESEncrypt(key, myLogin);
    jsonObject[NAME] = utility::AESEncrypt(key, myName);
    jsonObject[PUBLIC_KEY] = utility::serializePublicKey(myPublicKey);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getCreateChatPacket(const std::string& myUID,
    const std::string& supposedFriendLoginHash)
{
    nlohmann::json jsonObject;
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
    jsonObject[MY_UID] = myUID;
    jsonObject[NEW_LOGIN_HASH] = newLoginHash;
    jsonObject[NEW_LOGIN] = utility::AESEncrypt(key, newLogin);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getLoadUserInfoPacket(const std::string& myUID, const std::string& friendUID) {
    nlohmann::json jsonObject;
    jsonObject[MY_UID] = myUID;
    jsonObject[FRIEND_UID] = friendUID;

    return jsonObject.dump();
}


std::string PacketsBuilder::getVerifyPasswordPacket(const std::string& myUID, const std::string& passwordHash) {
    nlohmann::json jsonObject;
    jsonObject[MY_UID] = myUID;
    jsonObject[PASSWORD_HASH] = passwordHash;

    return jsonObject.dump();
}

std::string PacketsBuilder::getLoadAllFriendsStatusesPacket(const std::string& myUID, const std::vector<std::string>& friendsUIDsVec) {
    nlohmann::json jsonObject;
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
    jsonObject[MY_UID] = myUID;
    jsonObject[SEARCH_TEXT] = utility::AESEncrypt(key, searchText);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getSendMeFilePacket(const std::string& myUID, const std::string& fileId) {
    nlohmann::json jsonObject;
    jsonObject[MY_UID] = myUID;
    jsonObject[FILE_ID] = fileId;

    return jsonObject.dump();
}

std::string PacketsBuilder::getPublicKeyPacket(const std::string& myUID, const CryptoPP::RSA::PublicKey& myPublicKey) {
    nlohmann::json jsonObject;
    jsonObject[MY_UID] = myUID;
    jsonObject[FILE_ID] = utility::serializePublicKey(myPublicKey);

    return jsonObject.dump();
}

std::string PacketsBuilder::getUpdateRequestPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& versionNumber) {
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[MY_UID] = myUID;
    jsonObject[VERSION_NUMBER] = utility::AESEncrypt(key, versionNumber);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}  

std::string PacketsBuilder::getStatusPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& status, const std::string& myUID, const std::vector<std::string>& friendsUIDsVec)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[MY_UID] = myUID;
    jsonObject[STATUS] = utility::AESEncrypt(key, status);
    jsonObject[FRIENDS_UIDS] = getFriendsUIDsArray(friendsUIDsVec);

    return jsonObject.dump();
}








//RPL
std::string PacketsBuilder::getMessagePacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& messageID, const std::string& message, const std::string& timestamp)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(friendPublicKey, key);

    nlohmann::json jsonObject;
    jsonObject[FRIEND_UID] = friendUID;
    jsonObject[MY_UID] = myUID;
    jsonObject[MESSAGE_ID] = messageID;
    jsonObject[MESSAGE] = utility::AESEncrypt(key, message);
    jsonObject[TIMESTAMP] = utility::AESEncrypt(key, timestamp);
    jsonObject[ENCRYPTED_KEY] = encryptedKey;

    return jsonObject.dump();
}

std::string PacketsBuilder::getMessageReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& messageId) {
    return getBlobAndMessageReadConfirmationPacket(friendPublicKey, myUID, friendUID, messageId);
}

std::string PacketsBuilder::getBlobReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& blobId) {
    return getBlobAndMessageReadConfirmationPacket(friendPublicKey, myUID, friendUID, blobId);
}

std::string PacketsBuilder::getTypingPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID,const std::string& friendUID, bool isTyping) {
    nlohmann::json jsonObject;
    jsonObject[FRIEND_UID] = friendUID;
    jsonObject[IS_TYPING] = isTyping;

    return jsonObject.dump();
}





