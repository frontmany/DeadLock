#include<iostream>
#include<vector>

class Photo;
class Message;
enum class QueryType;

class PacketsBuilder {
public:
	PacketsBuilder() {};
	~PacketsBuilder() = default;
	const std::string& getEndPacketString();

	PacketsBuilder& operator=(const PacketsBuilder& other) {
		return *this;
	}

	//GET
	const std::string getAuthorizationPacket(const std::string& login, const std::string& passwordHash);
	const std::string getRegistrationPacket(const std::string& login, const std::string& name, const std::string& passwordHash);
	const std::string getCreateChatPacket(const std::string& myLogin, const std::string& friendLogin);
	const std::string getUpdateMyNamePacket(const std::string& login, const std::string& newName, const std::vector<std::string>& friendsLoginsVec);
	const std::string getUpdateMyPasswordPacket(const std::string& login, const std::string& newPasswordHash, const std::vector<std::string>& friendsLoginsVec);
	const std::string getUpdateMyPhotoPacket(const std::string& login, const Photo& photo, const std::vector<std::string>& friendsLoginsVec);
	const std::string getLoadUserInfoPacket(const std::string& login);
	const std::string getLoadAllFriendsStatusesPacket(const std::vector<std::string>& friendsLoginsVec);

	//RPL
	const std::string getMessagePacket(const std::string& myLogin, const std::string& friendLogin, const Message* message);
	const std::string getMessageReadConfirmationPacket(const std::string& myLogin, const std::string& friendLogin, const Message* message);

	//BROADCAST
	const std::string getStatusPacket(const std::string& status, const std::string& myLogin, const std::vector<std::string>& friendsLoginsVec);

private:
	const std::string parseTypeToStr(QueryType type);

private:
	const std::string endPacket = "_+14?bb5HmR;%@`7[S^?!#sL8";

	const std::string get = "GET";
	const std::string rpl = "RPL";
	const std::string broadcast = "BROADCAST";

	const std::string messageBegin = "MESSAGE_BEGIN";
	const std::string messageEnd = "MESSAGE_END";

	const std::string photoBegin = "PHOTO_BEGIN";
	const std::string photoEnd = "PHOTO_END";

	const std::string vecBegin = "VEC_BEGIN";
	const std::string vecEnd = "VEC_END";
};