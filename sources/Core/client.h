#pragma once
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <unordered_map>
#include <memory>

#include "net.h"
#include "theme.h"
#include "queryType.h"

class      fileWrapper;
class      Message;
class      Database;
class      WorkerUI;
class      ResponseHandler;
class      ConfigManager;
class      Chat;
class      Photo;
class      PacketsBuilder;


class Client : public net::client_interface<QueryType> {
public:
    Client();
    ~Client();

    void connectTo(const std::string& ipAddress, int port);
    void setWorkerUI(WorkerUI* workerImpl);
    void startProcessingIncomingPackets();
    void initDatabase(const std::string& login);
    bool waitForConnectionWithTimeout(int timeoutMs);
    void stop();

    Database* getDatabase() { return m_db; }

    void generateMyKeyPair();
    void sendPublicKeyToServer();

    void authorizeClient(const std::string& loginHash, const std::string& passwordHash);
    void registerClient(const std::string& loginHash, const std::string& passwordHash, const std::string& nameCipher);
    void afterRegistrationSendMyInfo();

    void updateMyName(const std::string& newName);
    void updateMyLogin(const std::string& newLogin);
    void updateMyPassword(const std::string& newPasswordHash);
    void updateMyPhoto(const Photo& newPhoto);

    void requestFile(const fileWrapper& fileWrapper);
    void createChatWith(const std::string& friendLogin);
    void verifyPassword(const std::string& passwordHash);
    void checkIsNewLoginAvailable(const std::string& newLogin);

    void requestUserInfoFromServer(const std::string& loginHashToSearch, const std::string& loginHash);
    void requestMyInfoFromServerAndResetKeys(const std::string& loginHash);
    void requestUpdate();

    void findUser(const std::string& searchText);
    void typingNotify(const std::string& friendLogin, bool isTyping);

    void sendFilesMessage(Message& filesMessage);
    void sendMessage(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& friendLogin, const Message* message);
    void sendMessageReadConfirmation(const std::string& friendLogin, const Message* message);
    void retrySendFilesMessage(Message& filesMessage);

    void broadcastMyStatus(const std::string& status);
    void getAllFriendsStatuses();


    void deleteFriendMessagesInDatabase(const std::string& friendLogin);
    void deleteFriendFromChatsMap(const std::string& friendLoginHash);
    
    void waitUntilUIReadyToUpdate();
    void skipLines(std::istream& iss, int count);

    void onMessage(net::message<QueryType> message) override;
    void onFile(net::file<QueryType> file) override;


    // errors
    void onSendMessageError(std::error_code ec, net::message<QueryType> unsentMessage) override;
    void onSendFileError(std::error_code ec, net::file<QueryType> unsentFile) override;
    void onReceiveMessageError(std::error_code ec) override;
    void onReceiveFileError(std::error_code ec, net::file<QueryType> unreadFile) override;
    void onConnectError(std::error_code ec) override;
    void onAllFilesSent() override;

    void onSendFileProgressUpdate(const net::file<QueryType>& file, uint32_t progressPercent) override;
    void onReceiveFileProgressUpdate(const net::file<QueryType>& file, uint32_t progressPercent) override;


    // GET && SET
    void setIsUIReadyToUpdate(bool isUIReadyToUpdate) { m_is_ui_ready_to_update.store(isUIReadyToUpdate); }

    const std::string& getServerIpAddress() const {return m_server_ipAddress; }
    void setServerIpAddress(const std::string& ipAddress) { m_server_ipAddress = ipAddress; }

    int geServerPort() const {return m_server_port; }
    void setServerPort(int port) {m_server_port = port; }

    std::unordered_map<std::string, Chat*>& getMyHashChatsMap() { return m_map_friend_loginHash_to_chat; }

    void setIsHidden(bool isHidden) { m_is_hidden = isHidden; }
    bool getIsHidden() { return m_is_hidden; }

    void setIsLoggedIn(bool isLoggedIn) { m_is_logged_in = isLoggedIn; }
    bool getIsLoggedIn() { return m_is_logged_in; }

    void setIsAbleToClose(bool isAbleToClose) { m_is_able_to_close = isAbleToClose; }
    bool getIsAbleToClose() { return m_is_able_to_close; }

    void setPublicKey(const CryptoPP::RSA::PublicKey& key);
    const CryptoPP::RSA::PublicKey& getPublicKey() const;

    void setPrivateKey(const CryptoPP::RSA::PrivateKey& key);
    const CryptoPP::RSA::PrivateKey& getPrivateKey() const;

    void setConfigManager(std::shared_ptr<ConfigManager> configManager) { m_config_manager = configManager; }
    std::shared_ptr<ConfigManager> getConfigManager() { return m_config_manager; }

    void setServerEncryptionPart(const std::string& encryptionPart);
    const std::string& getServerEncryptionPart() const { return m_server_encryption_part; }
    std::string getSpecialServerKey() const;

    void setIsFirstAuthentication(bool isFirstAuthentication) { m_is_first_authentication = isFirstAuthentication; }
    bool getIsFirstAuthentication() { return m_is_first_authentication; }

    void setIsPassedAuthentication(bool isPassedAuthentication) { m_is_passed_authentication = isPassedAuthentication; }
    bool getIsPassedAuthentication() { return m_is_passed_authentication; }

    std::optional<Chat*> findChat(const std::string& loginHash) const ;

private:
    const std::vector<std::string> getFriendsLoginHashesVecFromMap();
    void sendPacket(const std::string& packet, QueryType type);

    void onNetworkError();
    void onServerDown();

private:
    std::thread             m_worker_thread;

    bool                    m_is_hidden;
    bool                    m_is_first_authentication;
    bool                    m_is_passed_authentication;
    bool                    m_is_error;
    bool                    m_is_logged_in;
    bool                    m_is_able_to_close;
    std::atomic<bool>       m_is_ui_ready_to_update;

    ResponseHandler*        m_response_handler;
    PacketsBuilder*         m_packets_builder;
    Database*               m_db;

    std::shared_ptr<ConfigManager> m_config_manager;

    std::string m_server_encryption_part = "";
    std::string m_server_ipAddress = "";
    int m_server_port = 0;

    std::unordered_map<std::string, Chat*> m_map_friend_loginHash_to_chat;    
};