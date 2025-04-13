#pragma once
#include <thread>
#include <mutex>
#include <thread>
#include <queue>
#include <string>
#include <unordered_map>
#include <memory>

#include "asio.hpp"


enum class OperationResult;
class      Message;
class      Database;
class      WorkerUI;
class      ResponseHandler;
class      Chat;
class      Photo;
class      PacketsBuilder;

class Client {
public:
    void processQueue();
    void processToSendQueue();

    Client();
    ~Client();

    void connectTo(const std::string& ipAddress, int port);
    void setWorkerUI(WorkerUI* workerImpl);
    void run();
    void stop();

    void save() const;
    bool load(const std::string& fileName);

    void authorizeClient(const std::string& login, const std::string& passwordHash);
    void registerClient(const std::string& login, const std::string& passwordHash, const std::string& name);

    void updateMyName(const std::string& newName);
    void updateMyPassword(const std::string& newPasswordHash);
    void updateMyPhoto(const Photo& newPhoto);

    void createChatWith(const std::string& friendLogin);
    void sendMessage(const std::string& friendLogin, const Message* message);
    void sendMessageReadConfirmation(const std::string& friendLogin, const Message* message);

    void broadcastMyStatus(const std::string& status);
    void checkIsLoginAvailable(const std::string& newLogin);
    void getAllFriendsStatuses();

    void requestUserInfoFromServer(const std::string& myLogin);
    
    void waitUntilUIReadyToUpdate();


    // GET && SET
    asio::io_context& getIoContext() { return m_io_context; }

    bool isNeedToSaveConfig() { return m_is_need_to_save_config; };
    void setIsNeedToSaveConfig(bool isNeedToSaveConfig) { m_is_need_to_save_config = isNeedToSaveConfig; }

    void setIsUIReadyToUpdate(bool isUIReadyToUpdate) { m_is_ui_ready_to_update.store(isUIReadyToUpdate); }

    void setMyLogin(const std::string& login) { m_my_login = login; }
    const std::string& getMyLogin() const { return m_my_login; }

    void setMyName(const std::string& name) { m_my_name = name; }
    const std::string& getMyName() const { return m_my_name; }

    void setPhoto(Photo* photo) { m_my_photo = photo; }
    const Photo* getPhoto() const { return m_my_photo; }

    void setIsHasPhoto(bool isHasPhoto) { m_is_has_photo = isHasPhoto; }
    const bool getIsHasPhoto() const { return m_is_has_photo; }

    std::unordered_map<std::string, Chat*>& getMyChatsMap() { return m_map_friend_login_to_chat; }

private:
    const std::vector<std::string> getFriendsLoginsVecFromMap();

    OperationResult sendPacket(const std::string& packet);
    void startAsyncReceive();
    void handleAsyncReceive(const std::error_code& ec, std::size_t bytes_transferred);

private:
    asio::io_context                 m_io_context;
    asio::ip::tcp::socket            m_socket;
    asio::ip::tcp::endpoint          m_endpoint;
    std::shared_ptr<asio::streambuf> m_buffer;
    std::thread                      m_io_contextThread;
    std::thread                      m_workerThread;
    std::thread                      m_packetsSenderThread;

    bool                    m_is_need_to_save_config;
    std::atomic<bool>       m_is_ui_ready_to_update;
    ResponseHandler*        m_response_handler;
    PacketsBuilder*         m_packets_builder;
    Database*               m_db;
    std::mutex              m_queue_mutex;

    bool        m_is_has_photo;
    std::string m_my_login;
    std::string m_my_name;
    Photo*      m_my_photo;

    std::queue<std::string>                m_packets_queue;
    std::queue<std::string>                m_packets_to_send_queue;


    std::unordered_map<std::string, Chat*> m_map_friend_login_to_chat;    
};