#pragma once

#include <memory>
#include <vector>
#include <string>

class Avatar;

typedef std::shared_ptr<Avatar> AvatarPtr;

struct AvatarTransferData {
	AvatarPtr avatar;
	std::string myUID;
	std::vector<std::string> friendsUIDsVec;
};
