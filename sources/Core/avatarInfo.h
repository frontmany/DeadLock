#pragma once

#include <string>
#include <vector>
#include <memory>

class Avatar;

typedef std::shared_ptr<Avatar> AvatarPtr;

struct AvatarInfo {
	std::string	friendUID;
	AvatarPtr friendAvatar;
};