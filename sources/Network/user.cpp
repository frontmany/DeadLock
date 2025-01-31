#include "user.h"
#include "request.h"

std::vector<std::pair<std::string, std::string>>& User::getUserFriendsVec() {
	return m_vec_user_friends; 
}