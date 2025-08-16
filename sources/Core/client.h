#pragma once
#include <thread>
#include <string>
#include <unordered_map>
#include <memory>

#include "theme.h"
#include "packetType.h"
#include "database.h"
#include "fileLocationInfo.h"

#include "net_tasksManager.h"


class Blob;
class Message;
class WorkerUI;
class ConfigManager;
class Chat;
class Avatar;
class PacketsBuilder;
class AvatarInfo;
class KeysManager;

namespace net {
    class NetworkManager;
    class Packet;
}

typedef std::shared_ptr<KeysManager> KeysManagerPtr;
typedef std::shared_ptr<net::NetworkManager> NetworkManagerPtr;
typedef std::shared_ptr<ConfigManager> ConfigManagerPtr;
typedef std::shared_ptr<Message> MessagePtr;
typedef std::shared_ptr<Blob> BlobPtr;
typedef std::shared_ptr<Chat> ChatPtr;

using namespace net;

class Client {
public:
    Client();
    ~Client() = default;

    void connectTo(const std::string& ipAddress, int port);
    void setWorkerUI(WorkerUI* workerImpl);
    void startProcessingIncomingPackets();
    void startProcessingIncomingBlobs();
    void initDatabase(const std::string& login);
    bool waitForConnectionWithTimeout(int timeoutMs);
    bool tryReconnect();

    const Database& getDatabase() const;

    void generateMyKeyPair();

    void authorizeClient(const std::string& loginHash, const std::string& passwordHash);
    void registerClient(const std::string& loginHash, const std::string& passwordHash, const std::string& nameCipher);
    void sendMyInfoAfterRegistration();

    void updateMyName(const std::string& newName);
    void updateMyLogin(const std::string& newLogin);
    void updateMyPassword(const std::string& newPasswordHash);
    void updateMyAvatar(AvatarPtr newAvatar);

    void requestFile(const std::string& fileId);
    void createChatWith(const std::string& friendLogin);
    void verifyPassword(const std::string& passwordHash);
    void checkIsNewLoginAvailable(const std::string& newLogin);

    void requestUserInfoFromServer(const std::string& loginHashToSearch, const std::string& loginHash);
    void requestMyInfoFromServerAndResetKeys(const std::string& loginHash);
    void requestUpdate();

    void findUser(const std::string& searchText);
    void typingNotify(const std::string& friendLogin, bool isTyping);

    void sendBlob(BlobPtr blob);
    void sendMessage(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& friendLogin, MessagePtr message);
    void sendMessageReadConfirmation(const std::string& friendLogin, const std::string& messageId);
    void sendBlobReadConfirmation(const std::string& friendUID, const std::string& blobId);
    void retrySendBlob(Blob& blob);

    void broadcastMyStatus(const std::string& status);
    void getAllFriendsStatuses();


    void deleteMessagesInDatabaseWith(const std::string& friendLogin);
    void deleteFriendFromChatsMap(const std::string& friendLoginHash);
    
    void waitUntilUIReadyToUpdate();


    /*
    void onMessage(net::Message message) override;
    void onFile(net::File file) override;
    void onSendMessageError(std::error_code ec, net::Message unsentMessage) override;
    void onSendFileError(std::error_code ec, net::File unsentFile) override;
    void onConnectionDown() override;
    void onReceiveFileError(std::error_code ec, std::optional<net::File> unreadFile) override;
    void onAllFilesSent() override;
    void onSendFileProgressUpdate(const net::File& file, uint32_t progressPercent) override;
    void onReceiveFileProgressUpdate(const net::File& file, uint32_t progressPercent) override;
    */

    void onPacket(net::Packet&& packet);
    void onAvatar(AvatarInfo&& avatarInfo);
    void onBlob(BlobPtr blob);

    void onPacketSent(net::Packet&& packet);
    void onAvatarSent();
    void onSendFileProgressUpdate(FileLocationInfo&& info, uint32_t progressPercent);
    void onRequestedFileProgressUpdate(FileLocationInfo&& info, uint32_t progressPercent);
    void onDeadlockNewVersionProgressUpdate(uint32_t progressPercent);
    void onReceiveRequestedFileError(std::error_code ec, FileLocationInfo&& info);
    void onSendFileError(std::error_code ec, FileLocationInfo&& info);
    void onSendPacketError(std::error_code ec, const net::Packet& unsentPacket);
    void onConnectionDown();


    // GET && SET
    void setIsUIReadyToUpdate(bool isUIReadyToUpdate);
    bool getIsUIReadyToUpdate() const;

    const std::string& getServerIpAddress() const;
    void setServerIpAddress(const std::string& ipAddress);

    int getServerPort() const;
    void setServerPort(int port);

    std::unordered_map<std::string, ChatPtr>& getMyChatsMap();

    void setIsAbleToClose(bool isAbleToClose);
    bool getIsAbleToClose() const;

    void setConfigManager(ConfigManagerPtr configManager);
    ConfigManagerPtr getConfigManager() const;

    void setIsFirstAuthentication(bool isFirstAuthentication);
    bool getIsFirstAuthentication() const;

    void setIsPassedAuthentication(bool isPassedAuthentication);
    bool getIsPassedAuthentication() const;

    ChatPtr findChat(const std::string& loginHash) const;

private:
    std::vector<std::string> getFriendsUIDsVecFromMap();

private:
    std::thread m_packetsProcessingThread;
    std::thread m_blobsProcessingThread;
    bool m_isFirstAuthentication;
    bool m_isPassedAuthentication;
    bool m_isConnectionDown;
    bool m_isAbleToClose;
    std::atomic<bool> m_isUIReadyToUpdate;

    TasksManager m_tasksManager;
    ConfigManagerPtr m_configManager;
    KeysManagerPtr m_keysManager;
    ResponseHandler m_responseHandler;
    NetworkManagerPtr m_networkManager;
    Database m_db;

    std::string m_serverIpAddress;
    int m_serverPort;

    std::unordered_map<std::string, ChatPtr> m_mapChats;    

    static constexpr const char* MESSAGE_ID = "messageId";
    static constexpr const char* BLOB_ID = "blobId";
};