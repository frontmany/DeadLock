#include"friendInfo.h"
#include"utility.h"

void FriendInfo::setFriendLogin(const std::string& friendLogin) { m_friend_login = friendLogin; }
const std::string& FriendInfo::getFriendLogin() const { return m_friend_login; }

void FriendInfo::setLastReceivedOrSentMessage(const std::string& lastReceivedOrSentMessage) { m_last_received_or_sent_message = lastReceivedOrSentMessage; }
const std::string& FriendInfo::getLastReceivedOrSentMessage() const { return m_last_received_or_sent_message; }

void FriendInfo::setFriendName(const std::string& name) { m_friend_name = name; }
const std::string& FriendInfo::getFriendName() const { return m_friend_name; }

void FriendInfo::setFriendLastSeen(const std::string& lastSeen) { m_friend_last_seen = lastSeen; }
const std::string& FriendInfo::getFriendLastSeen() const { return m_friend_last_seen; }

void FriendInfo::setIsFriendHasPhoto(const bool isHasPhoto) { m_is_friend_has_photo = isHasPhoto; }
const bool FriendInfo::getIsFriendHasPhoto() const { return m_is_friend_has_photo; }

void FriendInfo::setFriendPhoto(Photo* photo) { m_friend_photo = photo; }
const Photo* FriendInfo::getFriendPhoto() const { return m_friend_photo; }

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