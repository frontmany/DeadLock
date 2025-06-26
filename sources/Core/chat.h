#pragma once
#include<vector>
#include<QJsonObject>
#include"message.h"

#include "secblock.h"
#include "secblockfwd.h"
#include "rsa.h"

class Database;
class Photo;


class Chat {
public:
	Chat() : m_is_friend_has_photo(false), m_index_at_layout(100000), m_friend_photo(nullptr) {}
	~Chat() {
		for (auto msg : m_vec_messages) {
			delete msg;
		}
		delete m_friend_photo;
	}

	std::vector<Message*>& getMessagesVec() { return m_vec_messages; }
	const std::vector<Message*> getUnreadSendMessagesVec() const;
	const std::vector<Message*> getUnreadReceiveMessagesVec() const;

	void setFriendLogin(const std::string& friendLogin) { m_friend_login = friendLogin; }
	const std::string& getFriendLogin() const { return m_friend_login; }

	void setLastReceivedOrSentMessage(const std::string& lastReceivedOrSentMessage) { m_last_received_or_sent_message = lastReceivedOrSentMessage; }
	const std::string& getLastReceivedOrSentMessage() const { return m_last_received_or_sent_message; }

	void setFriendName(const std::string& name) { m_friend_name = name; }
	const std::string& getFriendName() const { return m_friend_name; }

	void setFriendLastSeen(const std::string& lastSeen) { m_friend_last_seen = lastSeen; }
	const std::string& getFriendLastSeen() const { return m_friend_last_seen; }

	void setIsFriendHasPhoto(const bool isHasPhoto) { m_is_friend_has_photo = isHasPhoto; }
	const bool getIsFriendHasPhoto() const { return m_is_friend_has_photo; }

	void setFriendPhoto(Photo* photo) { m_friend_photo = photo; }
	const Photo* getFriendPhoto() const { return m_friend_photo; }

	void setLayoutIndex(int index) { m_index_at_layout = index; }
	const int getLayoutIndex() const { return m_index_at_layout; }

	void setPublicKey(CryptoPP::RSA::PublicKey key);
	CryptoPP::RSA::PublicKey getPublicKey();

	CryptoPP::SecByteBlock& getChatConfigKey() { return m_AESE_chat_configKey; }

	QJsonObject serialize(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& myLogin, const Database& db);
	static Chat* deserialize(const CryptoPP::RSA::PrivateKey& myPrivateKey, const std::string& myLogin, const QJsonObject& jsonObject, const Database& db);

private:
	CryptoPP::SecByteBlock	  m_AESE_chat_configKey;
	CryptoPP::RSA::PublicKey  m_public_key;
	std::vector<Message*>	  m_vec_messages;
	std::string				  m_friend_last_seen;
	std::string				  m_friend_login;
	std::string				  m_friend_name;
	std::string				  m_last_received_or_sent_message;
	bool					  m_is_friend_has_photo;
	Photo*					  m_friend_photo;
	int						  m_index_at_layout;
};