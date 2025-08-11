#include"friendInfo.h"
#include"utility.h"

void FriendInfo::setFriendUID(const std::string& friendUID) { m_friendUID = friendUID; }
const std::string& FriendInfo::getFriendUID() const { return m_friendUID; }

void FriendInfo::setFriendName(const std::string& name) { m_friend_name = name; }
const std::string& FriendInfo::getFriendName() const { return m_friend_name; }

void FriendInfo::setFriendLastSeen(const std::string& lastSeen) { m_friend_last_seen = lastSeen; }
const std::string& FriendInfo::getFriendLastSeen() const { return m_friend_last_seen; }

void FriendInfo::setIsFriendHasAvatar(const bool isHasAvatar) { m_is_friend_has_avatar = isHasAvatar; }
const bool FriendInfo::getIsFriendHasAvatar() const { return m_is_friend_has_avatar; }

void FriendInfo::setFriendAvatar(Avatar* avatar) { m_friend_avatar = avatar; }
Avatar* FriendInfo::getFriendAvatar() const { return m_friend_avatar; }

void FriendInfo::setLayoutIndex(int index) { m_index_at_layout = index; }
const int FriendInfo::getLayoutIndex() const { return m_index_at_layout; }

void FriendInfo::setPublicKey(const CryptoPP::RSA::PublicKey& key) {
    if (!utility::validatePublicKey(key)) {
        assert("Invalid public key provided");
    }

    m_public_key = key;
}

const CryptoPP::RSA::PublicKey& FriendInfo::getPublicKey() {
    if (!utility::validatePublicKey(m_public_key)) {
        assert("Public key is not initialized or invalid");
    }

    return m_public_key;
}