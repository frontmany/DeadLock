#pragma once
#include <thread>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <unordered_map>
#include <memory>

#include "net.h"


enum class OperationResult;
enum class QueryType : uint32_t;

class      Message;
class      Database;
class      WorkerUI;
class      ResponseHandler;
class      Chat;
class      Photo;
class      PacketsBuilder;

class Client : public net::client_interface<QueryType> {
public:
    Client();
    ~Client();

    void connectTo(const std::string& ipAddress, int port);
    void setWorkerUI(WorkerUI* workerImpl);
    void run();
    void initDatabase(const std::string& login);
    void stop();

    bool autoLoginAndLoad();
    bool undoAutoLogin();

    void save() const;
    bool load(const std::string& fileName);
    void updateConfigName(const std::string& newLogin);
    void updateInConfigFriendLogin(const std::string& oldLogin, const std::string& newLogin);

    void authorizeClient(const std::string& login, const std::string& passwordHash);
    void registerClient(const std::string& login, const std::string& passwordHash, const std::string& name);

    void updateMyName(const std::string& newName);
    void updateMyLogin(const std::string& newLogin);
    void updateMyPassword(const std::string& newPasswordHash);
    void updateMyPhoto(const Photo& newPhoto);

    void createChatWith(const std::string& friendLogin);
    void verifyPassword(const std::string& passwordHash);
    void checkIsNewLoginAvailable(const std::string& newLogin);
    void requestFriendInfoFromServer(const std::string& myLogin);
    void findUser(const std::string& text);
    void typingNotify(const std::string& friendLogin, bool isTyping);


    void sendMessage(const std::string& friendLogin, const Message* message);
    void sendMessageReadConfirmation(const std::string& friendLogin, const Message* message);

    void broadcastMyStatus(const std::string& status);
    void getAllFriendsStatuses();

    void deleteFriendChatInConfig(const std::string& friendLogin);
    void deleteFriendMessagesInDatabase(const std::string& friendLogin);
    void deleteFriendFromChatsMap(const std::string& friendLogin);
    
    void waitUntilUIReadyToUpdate();


    // GET && SET
    bool isAutoLogin() { return m_is_auto_login; };
    void setIsNeedToAutoLogin(bool  isNeedToAutoLogin) { m_is_auto_login = isNeedToAutoLogin; }

    bool isUndoAutoLogin() { return m_is_undo_auto_login; };
    void setNeedToUndoAutoLogin(bool  isNeedToUndoAutoLogin) { m_is_undo_auto_login = isNeedToUndoAutoLogin; }

    void setIsUIReadyToUpdate(bool isUIReadyToUpdate) { m_is_ui_ready_to_update.store(isUIReadyToUpdate); }

    void setMyPasswordHash(const std::string& passwordHash) { m_my_password_hash = passwordHash; }
    const std::string& getMyPasswordHash() const { return m_my_password_hash; }

    void setMyLogin(const std::string& login) { m_my_login = login; }
    const std::string& getMyLogin() const { return m_my_login; }

    void setMyName(const std::string& name) { m_my_name = name; }
    const std::string& getMyName() const { return m_my_name; }

    void setPhoto(Photo* photo) { m_my_photo = photo; }
    const Photo* getPhoto() const { return m_my_photo; }

    void setIsHasPhoto(bool isHasPhoto) { m_is_has_photo = isHasPhoto; }
    const bool getIsHasPhoto() const { return m_is_has_photo; }

    std::unordered_map<std::string, Chat*>& getMyChatsMap() { return m_map_friend_login_to_chat; }

    void setIsHidden(bool isHidden) { m_is_hidden = isHidden; }
    bool getIsHidden() { return m_is_hidden; }

private:
    const std::vector<std::string> getFriendsLoginsVecFromMap();
    void sendPacket(const std::string& packet, QueryType type);
    void processIncomingMessagesQueue();

private:
    std::thread             m_worker_thread;

    bool                    m_is_auto_login;
    bool                    m_is_undo_auto_login;
    bool                    m_is_hidden;

    std::atomic<bool>       m_is_ui_ready_to_update;

    ResponseHandler*        m_response_handler;
    PacketsBuilder*         m_packets_builder;
    Database*               m_db;
    std::mutex              m_queue_mutex;

    bool        m_is_has_photo;
    std::string m_my_login;
    std::string m_my_password_hash;
    std::string m_my_name;
    Photo*      m_my_photo;

    std::unordered_map<std::string, Chat*> m_map_friend_login_to_chat;    
};