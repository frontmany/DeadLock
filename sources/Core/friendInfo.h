#pragma once
#include <string>
#include <vector>

#include"rsa.h"

class Message;
class Avatar;

typedef std::shared_ptr<Avatar> AvatarPtr;

struct FriendInfo {
	CryptoPP::RSA::PublicKey friendPublicKey;
	std::string	friendUID;
	std::string friendName;
	std::string	friendLastSeen;
	bool isFriendHasAvatar;
	AvatarPtr friendAvatar;
};