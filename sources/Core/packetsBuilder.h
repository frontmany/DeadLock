#include<iostream>
#include<vector>

class Photo;
class Message;

class PacketsBuilder {
public:
	PacketsBuilder() {};
	~PacketsBuilder() = default;

	PacketsBuilder& operator=(const PacketsBuilder& other) { return *this; }

	//GET
	const std::string getAuthorizationPacket(const std::string& login, const std::string& passwordHash);
	const std::string getRegistrationPacket(const std::string& login, const std::string& name, const std::string& passwordHash);
	const std::string getCreateChatPacket(const std::string& myLogin, const std::string& friendLogin);
	const std::string getFindUserPacket(const std::string& myLogin, const std::string& text);

	const std::string getUpdateMyNamePacket(const std::string& login, const std::string& newName, const std::vector<std::string>& friendsLoginsVec);
	const std::string getUpdateMyPasswordPacket(const std::string& login, const std::string& newPasswordHash, const std::vector<std::string>& friendsLoginsVec);
	const std::string getUpdateMyLoginPacket(const std::string& login, const std::string& newLogin, const std::vector<std::string>& friendsLoginsVec);
	const std::string getUpdateMyPhotoPacket(const std::string& login, const Photo& photo, const std::vector<std::string>& friendsLoginsVec);

	const std::string getLoadUserInfoPacket(const std::string& login);
	const std::string getLoadAllFriendsStatusesPacket(const std::vector<std::string>& friendsLoginsVec);
	const std::string getVerifyPasswordPacket(const std::string& login, const std::string& passwordHash);
	const std::string getCheckIsNewLoginAvailablePacket(const std::string& newLogin);

	//new 
	const std::string getSendMeFilePacket(const std::string& myLogin, const std::string& friendLogin, const std::string& fileName, const std::string& fileId, const std::string& fileSize, const std::string& timestamp, const std::string& caption, const std::string& blobUID, uint32_t filesInBlobCount);

	//RPL
	const std::string getMessagePacket(const std::string& myLogin, const std::string& friendLogin, const Message* message);
	const std::string getMessageReadConfirmationPacket(const std::string& myLogin, const std::string& friendLogin, const Message* message);
	const std::string getTypingPacket(const std::string& myLogin, const std::string& friendLogin, bool isTyping);

	//BROADCAST
	const std::string getStatusPacket(const std::string& status, const std::string& myLogin, const std::vector<std::string>& friendsLoginsVec);

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