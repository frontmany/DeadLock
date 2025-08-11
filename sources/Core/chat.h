#pragma once
#include<vector>

#include "rsa.h"
#include "json.hpp"

class Database;
class Message;
class Blob;
class FilesBlob;
class Avatar;

typedef std::shared_ptr<Message> MessagePtr;
typedef std::shared_ptr<Avatar> AvatarPtr;
typedef std::shared_ptr<Blob> BlobPtr;
typedef std::shared_ptr<Database> DatabasePtr;

class Chat {
public:
	Chat();
	~Chat() = default;

    // Messages operations
    void addMessage(MessagePtr newMessage);
    void markMessageAsRead(const std::string& messageUID);

    const std::vector<MessagePtr>& getMessagesVec() const;
    const std::vector<MessagePtr>& getUnreadReceivedMessagesVec() const;

    const std::vector<BlobPtr>& getBlobsVec() const;
    const std::vector<BlobPtr>& getUnreadReceivedBlobsVec() const;

    // Friend info accessors
    void setFriendUID(const std::string& friendUID);
    const std::string& getFriendUID() const;

    void setPublicKey(const CryptoPP::RSA::PublicKey& publicKey);
    const CryptoPP::RSA::PublicKey& getPublicKey() const;

    void setMessageToShowInChatsList(const std::string& message);
    const std::string& getMessageToShowInChatsList() const;

    void setFriendName(const std::string& name);
    const std::string& getFriendName() const;

    void setFriendLastSeen(const std::string& lastSeen);
    const std::string& getFriendLastSeen() const;

    // Avatar management
    void setIsFriendHasAvatar(bool isHasAvatar);
    bool getIsFriendHasAvatar() const;

    void setFriendAvatar(AvatarPtr avatar);
    AvatarPtr getFriendAvatar() const;

    // Layout management
    void setLayoutIndex(int index);
    int getLayoutIndex() const;

    // Serialization
    nlohmann::json serialize(const CryptoPP::RSA::PublicKey& myPublicKey,
        const std::string& myUID,
        DatabasePtr db);

    static std::shared_ptr<Chat> deserialize(const CryptoPP::RSA::PrivateKey& myPrivateKey,
        const std::string& myUID,
        const nlohmann::json& jsonObject,
        DatabasePtr db,
        const CryptoPP::SecByteBlock& avatarsKey);


private:
	std::string	m_friendUID;
	std::string	m_friendName;
	std::string m_friendLastSeen;
	bool m_isFriendHasAvatar;
	AvatarPtr m_friendAvatar;
	CryptoPP::RSA::PublicKey m_friendPublicKey;

	int	m_indexAtLayout;
	std::string	m_messageToShowInChatsList;

	std::vector<BlobPtr> m_vecBlobs;
    std::vector<BlobPtr> m_vecUnreadReceivedBlobs;

	std::vector<MessagePtr> m_vecMessages;
	std::vector<MessagePtr> m_vecUnreadReceivedMessages;
};