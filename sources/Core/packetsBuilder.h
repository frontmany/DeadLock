#include<iostream>
#include<vector>

#include "rsa.h"

class Photo;
class Message;


class PacketsBuilder {
public:
	PacketsBuilder() {};
	~PacketsBuilder() = default;

	PacketsBuilder& operator=(const PacketsBuilder& other) { return *this; }

	//GET
	const std::string getReconnectPacket(const std::string& loginHash, const std::string& passwordHash);
	const std::string getAuthorizationPacket(const std::string& loginHash, const std::string& passwordHash);
	const std::string getRegistrationPacket(const std::string& loginHash, const std::string& passwordHash);
	const std::string getAfterRegistrationSendMyInfoPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& login, const std::string& name);
	const std::string getCreateChatPacket(const std::string& myLoginHash, const std::string& friendLoginHash);
	const std::string getFindUserPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& myLoginHash, const std::string& searchText);

	const std::string getUpdateMyNamePacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& loginHash, const std::string& newName, const std::vector<std::string>& friendsLoginsVec);
	const std::string getUpdateMyPasswordPacket(const std::string& loginHash, const std::string& newPasswordHash);
	const std::string getUpdateMyLoginPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& oldLoginHash, const std::string& newLoginHash, const std::string& newLogin, const std::vector<std::string>& friendsLoginHashesVec);
	const std::string getUpdateMyPhotoPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& loginHash, const Photo& photo, const std::vector<std::string>& friendsLoginsVec);

	const std::string getLoadUserInfoPacket(const std::string& loginHashToSearch, const std::string& loginHash);
	const std::string getLoadMyInfoPacket(const std::string& loginHash, const CryptoPP::RSA::PublicKey& myNewPublicKey);
	const std::string getLoadAllFriendsStatusesPacket(const std::string& loginHash, const std::vector<std::string>& friendsLoginHashesVec);
	const std::string getVerifyPasswordPacket(const std::string& login, const std::string& passwordHash);
	const std::string getCheckIsNewLoginAvailablePacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& oldLoginHash, const std::string& newLogin);
	const std::string getSendMeFilePacket(const CryptoPP::RSA::PrivateKey& myPrivatKey, const std::string& encryptedKey, const std::string& myLoginHash, const std::string& friendLoginHash, const std::string& fileName, const std::string& fileId, const std::string& fileSize, const std::string& timestamp, const std::string& caption, const std::string& blobUID, const std::string& filesInBlobCount);
	const std::string getPublicKeyPacket(const std::string& myLoginHash, const CryptoPP::RSA::PublicKey& myPublicKey);
	const std::string getUpdateRequestPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& loginHash, const std::string& versionNumber);

	//RPL
	const std::string getMessagePacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myLoginHash, const std::string& friendLoginHash, const Message* message);
	const std::string getMessageReadConfirmationPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myLoginHash, const std::string& friendLoginHash, const Message* message);
	const std::string getTypingPacket(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& myLoginHash, const std::string& friendLoginHash, bool isTyping);

	//BROADCAST
	const std::string getStatusPacket(const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& status, const std::string& myLoginHash, const std::vector<std::string>& friendsLoginHashesVec);

private:
	static constexpr const char* get = "GET";
	static constexpr const char* rpl = "RPL";
	static constexpr const char* broadcast = "BROADCAST";

	static constexpr const char* messageBegin = "MESSAGE_BEGIN";
	static constexpr const char* messageEnd = "MESSAGE_END";

	static constexpr const char* photoBegin = "PHOTO_BEGIN";
	static constexpr const char* photoEnd = "PHOTO_END";

	static constexpr const char* vecBegin = "VEC_BEGIN";
	static constexpr const char* vecEnd = "VEC_END";
};