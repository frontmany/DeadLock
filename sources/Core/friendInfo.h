#pragma once
#include <string>
#include <vector>

#include"rsa.h"

class Message;
class Photo;

class FriendInfo {
public:
	void setFriendLogin(const std::string& friendLogin);
	const std::string& getFriendLogin() const;

	void setLastReceivedOrSentMessage(const std::string& lastReceivedOrSentMessage);
	const std::string& getLastReceivedOrSentMessage() const;

	void setFriendName(const std::string& name);
	const std::string& getFriendName() const;

	void setFriendLastSeen(const std::string& lastSeen);
	const std::string& getFriendLastSeen() const;

	void setIsFriendHasPhoto(const bool isHasPhoto);
	const bool getIsFriendHasPhoto() const;

	void setFriendPhoto(Photo* photo);
	const Photo* getFriendPhoto() const;

	void setLayoutIndex(int index);
	const int getLayoutIndex() const;

	void setPublicKey(const CryptoPP::RSA::PublicKey& key);
	const CryptoPP::RSA::PublicKey& getPublicKey();

private:
	std::vector<Message*>	 m_vec_messages;
	std::string				 m_friend_last_seen;
	std::string				 m_friend_login;
	std::string				 m_friend_name;
	std::string				 m_last_received_or_sent_message;
	bool					 m_is_friend_has_photo;
	Photo*					 m_friend_photo;
	int						 m_index_at_layout;
	CryptoPP::RSA::PublicKey m_public_key;
};