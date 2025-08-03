#include"packetsBuilder.h"
#include"message.h"
#include"queryType.h"
#include"utility.h"


//GET

const std::string PacketsBuilder::getReconnectPacket(const std::string& loginHash, const std::string& passwordHash) {
    return getAuthorizationPacket(loginHash, passwordHash);
}

const std::string PacketsBuilder::getAuthorizationPacket(const std::string& loginHash,
    const std::string& passwordHash)
{
    std::ostringstream oss;
    oss << get << '\n'
        << loginHash << '\n'
        << passwordHash << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getRegistrationPacket(const std::string& loginHash,
    const std::string& passwordHash) 
{
    std::ostringstream oss;
    oss << get << '\n'
        << loginHash << '\n'
        << passwordHash << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getAfterRegistrationSendMyInfoPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& login, const std::string& name) {
    std::ostringstream oss;

    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);
    

    oss << get << '\n'
        << encryptedKey << '\n'
        << utility::AESEncrypt(key, login) << '\n'
        << utility::AESEncrypt(key, name) << '\n';

    
    return oss.str();
}

const std::string PacketsBuilder::getCreateChatPacket(const std::string& myLoginHash,
    const std::string& friendLoginHash) 
{
    std::ostringstream oss;
    oss << get << '\n'
        << myLoginHash << '\n'
        << friendLoginHash << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyNamePacket(const CryptoPP::RSA::PublicKey& serverPublicKey,
    const std::string& loginHash,
    const std::string& newName,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;

    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    oss << get << '\n'
        << encryptedKey << '\n'
        << loginHash << '\n'
        << utility::AESEncrypt(key, newName) << '\n';

    oss << vecBegin << '\n';
    for (const auto& loginHash : friendsLoginsVec) {
        oss << loginHash << '\n';
    }
    oss << vecEnd;

    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyPasswordPacket(
    const std::string& loginHash,
    const std::string& newPasswordHash)
{
    std::ostringstream oss;
    oss << get << '\n'
        << loginHash << '\n'
        << newPasswordHash;


    return oss.str();
}

const std::string PacketsBuilder::getSerializedFriendsLoginHashesVec(const std::vector<std::string>& friendsLoginHashesVec)
{
    std::ostringstream oss;

    oss << vecBegin << '\n';
    for (const auto& loginHash : friendsLoginHashesVec) {
        oss << loginHash << '\n';
    }

    return oss.str();
}

const std::string  PacketsBuilder::getUpdateMyLoginPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& oldLoginHash, const std::string& newLoginHash, const std::string& newLogin, const std::vector<std::string>& friendsLoginHashesVec) {
    std::ostringstream oss;

    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    oss << get << '\n'
        << encryptedKey << '\n'
        << oldLoginHash << '\n'
        << newLoginHash << '\n'
        << utility::AESEncrypt(key, newLogin) << '\n';

    oss << vecBegin << '\n';
    for (const auto& loginHash : friendsLoginHashesVec) {
        oss << loginHash << '\n';
    }
    oss << vecEnd;

    return oss.str();
}

const std::string PacketsBuilder::getLoadUserInfoPacket(const std::string& loginHashToSearch, const std::string& loginHash) {
    std::ostringstream oss;
    oss << get << '\n'
        << loginHash << '\n'
        << loginHashToSearch << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getLoadMyInfoPacket(const std::string& loginHash, const CryptoPP::RSA::PublicKey& myNewPublicKey) {
    std::ostringstream oss;
    oss << get << '\n'
        << loginHash << '\n'
        << utility::serializePublicKey(myNewPublicKey);

    return oss.str();
}


const std::string PacketsBuilder::getVerifyPasswordPacket(const std::string& loginHash, const std::string& passwordHash) {
    std::ostringstream oss;

    oss << get << '\n'
        << loginHash << '\n'
        << passwordHash;

    return oss.str();
}

const std::string PacketsBuilder::getLoadAllFriendsStatusesPacket(const std::string& loginHash, const std::vector<std::string>& friendsLoginHashesVec) {
    std::ostringstream oss;

    oss << get << '\n'
        << loginHash << '\n'
        << vecBegin << '\n';

    for (const auto& friendLoginHash : friendsLoginHashesVec) {
        oss << friendLoginHash << '\n';
    }

    oss << vecEnd;

    return oss.str();
}

const std::string PacketsBuilder::getCheckIsNewLoginAvailablePacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& oldLoginHash, const std::string& newLogin) {
    std::ostringstream oss;

    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    oss << get << '\n'
        << encryptedKey << '\n'
        << oldLoginHash << '\n'
        << utility::AESEncrypt(key, newLogin);

    return oss.str();
}

const std::string PacketsBuilder::getFindUserPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myLoginHash, const std::string& searchText) {
    std::ostringstream oss;

    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    oss << get << '\n'
        << encryptedKey << '\n'
        << myLoginHash << '\n'
        << utility::AESEncrypt(key, searchText);

    return oss.str();
}

const std::string PacketsBuilder::getSendMeFilePacket(const CryptoPP::RSA::PrivateKey& myPrivatKey, const std::string& encryptedKey, const std::string& myLoginHash, const std::string& friendLoginHash, const std::string& fileName, const std::string& fileId, const std::string& fileSize, const std::string& timestamp, const std::string& caption, const std::string& blobUID, const std::string& filesInBlobCount) {
    std::ostringstream oss;

    CryptoPP::SecByteBlock key = utility::RSADecryptKey(myPrivatKey, encryptedKey);

    oss << get << '\n'
        << encryptedKey << '\n'
        << myLoginHash << '\n'
        << fileId << '\n'
        << blobUID << "\n"
        << friendLoginHash << '\n'
        << utility::AESEncrypt(key, fileName) << '\n' //here
        << fileSize << '\n'
        << utility::AESEncrypt(key, timestamp) << '\n';

        if (caption != "") {
            oss << utility::AESEncrypt(key, caption) << '\n';
        }
        else {
            oss << '\n';
        }
        
        oss << filesInBlobCount;

    return oss.str();
}

const std::string PacketsBuilder::getPublicKeyPacket(const std::string& myLoginHash, const CryptoPP::RSA::PublicKey& myPublicKey) {
    std::ostringstream oss;

    oss << get << '\n'
        << myLoginHash << '\n'
        << utility::serializePublicKey(myPublicKey);

    return oss.str();
}

const std::string PacketsBuilder::getUpdateRequestPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& loginHash, const std::string& versionNumber) {
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    std::ostringstream oss;
    oss << get << '\n'
        << encryptedKey << '\n'
        << loginHash << '\n'
        << utility::AESEncrypt(key, versionNumber);

    return oss.str();
}  

//RPL
const std::string PacketsBuilder::getMessagePacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myLoginHash, const std::string& friendLoginHash, const Message* message)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(friendPublicKey, key);

    std::ostringstream oss;
    oss << rpl << '\n'
        << friendLoginHash << '\n'
        << myLoginHash << '\n'
        << encryptedKey << '\n'
        << message->getId() << '\n'
        << utility::AESEncrypt(key, message->getMessage()) << '\n'
        << utility::AESEncrypt(key, message->getTimestamp());

    return oss.str();
}

const std::string PacketsBuilder::getMessageReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey,const std::string& myLoginHash, const std::string& friendLoginHash, const Message* message)
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(friendPublicKey, key);

    std::ostringstream oss;
    oss << rpl << '\n'
        << friendLoginHash << '\n'
        << myLoginHash << '\n'
        << encryptedKey << '\n'
        << utility::AESEncrypt(key, message->getId());

    return oss.str();
}

const std::string PacketsBuilder::getTypingPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myLoginHash,const std::string& friendLoginHash, bool isTyping) 
{
    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);
    std::string encryptedKey = utility::RSAEncryptKey(friendPublicKey, key);

    std::ostringstream oss;
    oss << rpl << '\n'
        << friendLoginHash << '\n'
        << myLoginHash << '\n'
        << encryptedKey << '\n'
        << utility::AESEncrypt(key, (isTyping ? "1" : "0"));

    return oss.str();
}

//BROADCAST
const std::string PacketsBuilder::getStatusPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& status,
    const std::string& myLoginHash,
    const std::vector<std::string>& friendsLoginHashesVec)
{
    std::ostringstream oss;

    CryptoPP::SecByteBlock key;
    utility::generateAESKey(key);

    std::string encryptedKey = utility::RSAEncryptKey(serverPublicKey, key);

    oss << broadcast << '\n'
        << encryptedKey << '\n'
        << utility::AESEncrypt(key, status) << '\n'
        << myLoginHash << '\n'
        << vecBegin << '\n';

    for (const auto& loginHash : friendsLoginHashesVec) {
        oss << loginHash << '\n';
    }

    oss << vecEnd;

    return oss.str();
}