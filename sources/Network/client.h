#pragma once
#include <asio.hpp>
#include <thread>
#include <mutex>
#include <thread>
#include <string>
#include <unordered_map>
#include <memory>

#include "sender.h"
#include "database.h"

#include "photo.h"
#include "chat.h"

#ifdef _WIN32
#define _WIN32_WINNT 0x0A00
#endif // !_WIN32


enum class OperationResult { NOT_STATED, FAIL, SUCCESS, REQUEST_TIMEOUT };
class Message;
class Database;
class WorkerUI;

class Client {
public:
    
    Client();
    ~Client() = default;
    void run();
    void connectTo(const std::string& ipAddress, int port);
    void setWorkerUI(WorkerUI* workerImpl);
    void sendPacket(const std::string& packet);
    bool isAuthorized();
    void close();
    void loadAvatarFromPC(const std::string& login);
    void save() const;
    bool load(const std::string& fileName);

    OperationResult authorizeClient(const std::string& login, const std::string& password);
    OperationResult registerClient(const std::string& login, const std::string& password, const std::string& name);
    OperationResult updateMyInfo(const std::string& login, const std::string& name, const std::string& password, bool isHasPhoto, Photo photo = Photo());
    OperationResult createChatWith(const std::string& friendLogin);
    OperationResult sendMessage(const std::string& friendLogin, const std::string& message, const std::string& id, const std::string timestamp);
    OperationResult sendFirstMessage(const std::string& friendLogin, const std::string& message, const std::string& id, const std::string timestamp);
    OperationResult sendMyStatus(const std::string& status);
    OperationResult checkIsLoginAvailable(const std::string& newLogin);
    OperationResult sendMessageReadConfirmation(const std::string& friendLogin, const std::vector<Message*>& messagesReadIdsVec);
    OperationResult getMyInfoFromServer(const std::string& myLogin);
    OperationResult getFriendsStatuses(std::vector<std::string> vecFriends);

    void setMyLogin(const std::string& login) { m_my_login = login; }
    const std::string& getMyLogin() const { return m_my_login; }

    void setMyName(const std::string& name) { m_my_name = name; }
    const std::string& getMyName() const { return m_my_name; }

    void setPhoto(const Photo& photo) { m_my_photo = photo; }
    const Photo& getPhoto() const { return m_my_photo; }

    void setPassword(const std::string& password) { m_my_password = password; }
    const std::string& getPassword() const { return m_my_password; }

    void setIsHasPhoto(bool isHasPhoto) { m_is_has_photo = isHasPhoto; }
    const bool getIsHasPhoto() const { return m_is_has_photo; }

    const SendStringsGenerator& getSender() const { return m_sender; }
    std::unordered_map<std::string, Chat*>& getMyChatsMap() { return m_map_friend_login_to_chat; }
    std::vector<std::string>& getVecToSendStatusTmp() { return m_vec_friends_logins_tmp; }

    std::string sh_packet_auth = "";
    bool chatsWidgetState = false;
    bool isStatuses = false;

private:
    void startAsyncReceive();
    void processChatCreateSuccess(const std::string& packet);
    void processFriendsStatusesSuccess(const std::string& packet);
    void processUserInfoSuccess(const std::string& packet);
    void handleAsyncReceive(const std::error_code& ec, std::size_t bytes_transferred);
    void handleResponse(const std::string& packet);
    OperationResult waitForResponse(int seconds, std::function<OperationResult()> checkFunction);

private:
    static constexpr size_t DEFAULT_BUFFER_SIZE = 20971520;

    const std::string       c_endPacket = "_+14?bb5HmR;%@`7[S^?!#sL8";
    asio::io_context        m_io_context;
    asio::ip::tcp::socket   m_socket;
    asio::ip::tcp::endpoint m_endpoint;
    std::string             m_accumulated_data;
    std::shared_ptr<asio::streambuf> m_buffer;
    bool                    m_isReceiving;
    bool                    m_is_loaded = false;
    std::thread             m_io_contextThread;

    std::mutex              m_mtx;
    SendStringsGenerator    m_sender;
    WorkerUI*               m_worker;
    Database                m_db;

    std::string m_my_login = "";
    std::string m_my_name = "";
    std::string m_my_password = "";
    bool m_is_has_photo;
    Photo m_my_photo;
    std::unordered_map<std::string, Chat*> m_map_friend_login_to_chat;
    std::vector<std::string>               m_vec_friends_logins_tmp;

    //shared variables (main thread await until state changed on FAIL, SUCCESS or REQUEST_TIMEOUT)
private:
    OperationResult     sh_is_authorized;
    OperationResult     sh_is_user_info;
    OperationResult     sh_is_info_updated;
    OperationResult     sh_chat_create;
    OperationResult     sh_is_status_send;
    OperationResult     sh_is_statuses;
    OperationResult     sh_is_message_send;
    OperationResult     sh_is_first_message_send;
    OperationResult     sh_is_message_read_confirmation_send;
    OperationResult     sh_new_login;
};