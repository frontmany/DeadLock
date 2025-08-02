#pragma once
#include<vector>
#include<QJsonObject>
#include"message.h"

#include "secblock.h"
#include "secblockfwd.h"
#include "rsa.h"

class Database;
class Avatar;


class Chat {
public:
	Chat() : m_is_friend_has_avatar(false), m_index_at_layout(100000), m_friend_avatar(nullptr) {}
	~Chat() {
		for (auto msg : m_vec_messages) {
			delete msg;
		}
		delete m_friend_avatar;
	}

	std::vector<Message*>& getMessagesVec() { return m_vec_messages; }
	const std::vector<Message*> getUnreadSendMessagesVec() const;
	const std::vector<Message*> getUnreadReceiveMessagesVec() const;

	void setFriendLogin(const std::string& friendLogin) { m_friend_login = friendLogin; }
	const std::string& getFriendLogin() const { return m_friend_login; }

	void setPublicKey(const CryptoPP::RSA::PublicKey& publicKey) { m_publicKey = publicKey; }
	const CryptoPP::RSA::PublicKey& getPublicKey() const { return m_publicKey; }
	
	void setLastReceivedOrSentMessage(const std::string& lastReceivedOrSentMessage) { m_last_received_or_sent_message = lastReceivedOrSentMessage; }
	const std::string& getLastReceivedOrSentMessage() const { return m_last_received_or_sent_message; }

	void setFriendName(const std::string& name) { m_friend_name = name; }
	const std::string& getFriendName() const { return m_friend_name; }

	void setFriendLastSeen(const std::string& lastSeen) { m_friend_last_seen = lastSeen; }
	const std::string& getFriendLastSeen() const { return m_friend_last_seen; }

	void setIsFriendHasAvatar(const bool isHasAvatar) { m_is_friend_has_avatar = isHasAvatar; }
	const bool getIsFriendHasAvatar() const { return m_is_friend_has_avatar; }

	void setFriendAvatar(Avatar* avatar) { m_friend_avatar = avatar; }
	const Avatar* getFriendAvatar() const { return m_friend_avatar; }

	void setLayoutIndex(int index) { m_index_at_layout = index; }
	const int getLayoutIndex() const { return m_index_at_layout; }

	CryptoPP::SecByteBlock& getChatConfigKey() { return m_AESE_chat_configKey; }

	QJsonObject serialize(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& myLogin, const Database& db);
	static Chat* deserialize(const CryptoPP::RSA::PrivateKey& myPrivateKey, const std::string& myLogin, const QJsonObject& jsonObject, const Database& db, CryptoPP::SecByteBlock avatarsKey);

private:
	CryptoPP::SecByteBlock	  m_AESE_chat_configKey;
	CryptoPP::RSA::PublicKey& m_publicKey;
	std::vector<Message*>	  m_vec_messages;
	std::string				  m_friend_last_seen;
	std::string				  m_friend_login;
	std::string				  m_friend_name;
	std::string				  m_last_received_or_sent_message;
	bool					  m_is_friend_has_avatar;
	Avatar*					  m_friend_avatar;
	int						  m_index_at_layout;
};