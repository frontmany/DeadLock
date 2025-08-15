#include<iostream>
#include<vector>

#include "rsa.h"
#include "json.hpp"

class PacketsBuilder {
public:
	PacketsBuilder() {};
	~PacketsBuilder() = default;

    //GET
    static std::string getReconnectPacket(const std::string& myLoginHash, const std::string& passwordHash);
    static std::string getAuthorizationPacket(const std::string& myLoginHash, const std::string& passwordHash);
    static std::string getRegistrationPacket(const std::string& myLoginHash, const std::string& passwordHash);
    static std::string getAfterRegistrationInfoPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& myLogin, const std::string& myName);
    static std::string getCreateChatPacket(const std::string& myUID, const std::string& supposedFriendLoginHash);
    static std::string getUpdateMyNamePacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& newName, const std::vector<std::string>& friendsUIDsVec);
    static std::string getUpdateMyPasswordPacket(const std::string& myUID, const std::string& newPasswordHash);
    static std::string getUpdateMyLoginPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& newLoginHash, const std::string& newLogin);
    static std::string getLoadUserInfoPacket(const std::string& myUID, const std::string& friendUID);
    static std::string getVerifyPasswordPacket(const std::string& myUID, const std::string& passwordHash);
    static std::string getLoadAllFriendsStatusesPacket(const std::string& myUID, const std::vector<std::string>& friendsUIDsVec);
    static std::string getCheckIsNewLoginAvailablePacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& newLogin);
    static std::string getFindUserPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& searchText);
    static std::string getSendMeFilePacket(const std::string& myUID, const std::string& fileId);
    static std::string getPublicKeyPacket(const std::string& myUID, const CryptoPP::RSA::PublicKey& myPublicKey);
    static std::string getUpdateRequestPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myUID, const std::string& versionNumber);
    static std::string getStatusPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& status, const std::string& myUID, const std::vector<std::string>& friendsUIDsVec);

    //RPL
    static std::string getMessagePacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& messageID, const std::string& message, const std::string& timestamp);
    static std::string getMessageReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& messageId);
    static std::string getBlobReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& blobId);
    static std::string getTypingPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, bool isTyping);

private:
    static std::string getLoginAndPasswordHashPacket(const std::string& loginHash, const std::string& passwordHash);
    static std::string getBlobAndMessageReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myUID, const std::string& friendUID, const std::string& id, bool isBlobReadConfirmation = false);
    static nlohmann::json getFriendsUIDsArray(const std::vector<std::string>& friendsUIDsVec);

private:
    // Top-level command markers
    static constexpr const char* GET = "GET";
    static constexpr const char* RPL = "RPL";

    // JSON field keys
    static constexpr const char* LOGIN_HASH = "loginHash";
    static constexpr const char* PASSWORD_HASH = "passwordHash";
    static constexpr const char* LOGIN = "login";
    static constexpr const char* NAME = "name";
    static constexpr const char* ENCRYPTED_KEY = "encryptedKey";
    static constexpr const char* PUBLIC_KEY = "publicKey";
    static constexpr const char* MY_UID = "myUID";
    static constexpr const char* SUPPOSED_FRIEND_LOGIN_HASH = "supposedFriendLoginHash";
    static constexpr const char* NEW_NAME = "newName";
    static constexpr const char* FRIENDS_UIDS = "friendsUIDs";
    static constexpr const char* NEW_PASSWORD_HASH = "newPasswordHash";
    static constexpr const char* NEW_LOGIN_HASH = "newLoginHash";
    static constexpr const char* NEW_LOGIN = "newLogin";
    static constexpr const char* FRIEND_UID = "friendUID";
    static constexpr const char* SEARCH_TEXT = "searchText";
    static constexpr const char* FILE_ID = "fileId";
    static constexpr const char* VERSION_NUMBER = "versionNumber";
    static constexpr const char* MESSAGE_ID = "messageId";
    static constexpr const char* BLOB_ID = "blobId";
    static constexpr const char* MESSAGE = "message";
    static constexpr const char* TIMESTAMP = "timestamp";
    static constexpr const char* IS_TYPING = "isTyping";
    static constexpr const char* STATUS = "status";
};