#pragma once
#include <thread>
#include <string>
#include <unordered_map>
#include <memory>

#include "theme.h"
#include "packetType.h"
#include "database.h"


class Blob;
class Message;
class WorkerUI;
class ConfigManager;
class Chat;
class Avatar;
class PacketsBuilder;
class KeysManager;

typedef std::shared_ptr<KeysManager> KeysManagerPtr;
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

    const Database& getDatabase() { return m_db; }

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
    void retrySendFilesMessage(Message& filesMessage);

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

    void onPacketSent(PacketType type);
    void onMessageReadConfirmationSent(const std::string& messageId);
    void onBlobReadConfirmationSent(const std::string& blobId);

    void onPacket(const Packet& packet);
    void onBlob(BlobPtr blob);
    void onSendFileProgressUpdate(const InfoToWhom& info, uint32_t progressPercent);
    void onRequestedFileProgressUpdate(const InfoToWhom& info, uint32_t progressPercent);
    void onDeadlockNewVersionProgressUpdate(uint32_t progressPercent);
    void onReceiveRequestedFileError(std::error_code ec, const InfoToWhom& info);
    void onSendFileError(std::error_code ec, const InfoToWhom& info);
    void onSendPacketError(std::error_code ec, const Packet& unsentPacket);
    void onConnectionDown();


    // GET && SET
    void setIsUIReadyToUpdate(bool isUIReadyToUpdate) { m_is_ui_ready_to_update.store(isUIReadyToUpdate); }

    const std::string& getServerIpAddress() const {return m_server_ipAddress; }
    void setServerIpAddress(const std::string& ipAddress) { m_server_ipAddress = ipAddress; }

    int geServerPort() const {return m_server_port; }
    void setServerPort(int port) {m_server_port = port; }

    std::unordered_map<std::string, Chat*>& getMyChatsMap() { return m_map_friend_loginHash_to_chat; }

    void setIsAbleToClose(bool isAbleToClose) { m_is_able_to_close = isAbleToClose; }
    bool getIsAbleToClose() { return m_is_able_to_close; }

    void setConfigManager(ConfigManagerPtr configManager) { m_config_manager = configManager; }
    ConfigManagerPtr getConfigManager() { return m_config_manager; }

    void setIsFirstAuthentication(bool isFirstAuthentication) { m_is_first_authentication = isFirstAuthentication; }
    bool getIsFirstAuthentication() { return m_is_first_authentication; }

    void setIsPassedAuthentication(bool isPassedAuthentication) { m_is_passed_authentication = isPassedAuthentication; }
    bool getIsPassedAuthentication() { return m_is_passed_authentication; }

    ChatPtr findChat(const std::string& loginHash) const;

private:
    std::vector<std::string> getFriendsUIDsVecFromMap();
    void sendPacket(const std::string& packet, PacketType type);

private:
    std::thread m_packetsProcessingThread;
    std::thread m_blobsProcessingThread;
    bool m_isFirstAuthentication;
    bool m_isPassedAuthentication;
    bool m_isConnectionDown;
    bool m_isAbleToClose;
    std::atomic<bool> m_isUIReadyToUpdate;

    ConfigManagerPtr m_configManager;
    KeysManagerPtr m_keysManagerPtr;
    ResponseHandler m_responseHandler;
    NetworkManager m_networkManager;
    Database m_db;

    std::string m_serverIpAddress;
    int m_serverPort;

    std::unordered_map<std::string, ChatPtr> m_mapChats;    
};