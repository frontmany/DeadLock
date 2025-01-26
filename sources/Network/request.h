#pragma once

#include<string>
#include<sstream>

#include"photo.h"

class Packet {
public:
    virtual ~Packet() = default;
    virtual std::string serialize() { return ""; };
};


class SizePacket {
public:
    SizePacket() {}
    std::string serialize();
    static size_t deserialize(const std::string& str);
    void setData(const std::string& data) { m_serialized_data_object = data; }

private:
    std::string m_serialized_data_object;
};

enum class Response {
    EMPTY_RESPONSE,
    NEXT_QUERY_SIZE,
    AUTHORIZATION_SUCCESS,
    REGISTRATION_SUCCESS,
    AUTHORIZATION_FAIL,
    REGISTRATION_FAIL,
    CHAT_CREATE_SUCCESS,
    CHAT_CREATE_FAIL,
    USER_INFO_FOUND,
    USER_INFO_NOT_FOUND,
    USER_INFO_UPDATED,
    USER_INFO_NOT_UPDATED,
    FRIEND_STATE_CHANGED,
    MESSAGE,
    ALL_FRIENDS_STATES

};


namespace rpl {
    class UserInfoPacket {
    public:
        UserInfoPacket() : m_isHasPhoto(false), m_isOnline(false) {}
        static UserInfoPacket deserialize(const std::string& str);
        std::string serialize();

        const std::string& getLogin() const { return m_user_login; }
        void setLogin(const std::string& login) { m_user_login = login; }

        const std::string& getName() const { return m_user_name; }
        void setName(const std::string& name) { m_user_name = name; }

        const std::string& getLastSeen() const { return m_last_seen; }
        void setLastSeen(const std::string& lastSeen) { m_last_seen = lastSeen; }

        const bool getIsOnline() const { return m_isOnline; }
        void setIsOnline(const bool isOnline) { m_isOnline = isOnline; }

        const bool getIsHasPhoto() const { return m_isHasPhoto; }
        void setIsHasPhoto(const bool isHasPhoto) { m_isHasPhoto = isHasPhoto; }

        const Photo& getPhoto() const { return m_user_photo; }
        void setPhoto(const Photo& photo) { m_user_photo = photo; }

    private:
        std::string m_user_login;
        std::string m_user_name;
        std::string	m_last_seen;
        Photo       m_user_photo;
        bool		m_isOnline;
        bool		m_isHasPhoto;

    };

    class Message {
    public:
        Message() {}
        std::string serialize();
        static Message deserialize(const std::string& str);

        const rpl::UserInfoPacket& getFriendInfo() const { return m_friend_info; }
        void setFriendInfo(rpl::UserInfoPacket& info) { m_friend_info = info; }

        const rpl::UserInfoPacket& getMyInfo() const { return m_my_info; }
        void setMyInfo(rpl::UserInfoPacket& info) { m_my_info = info; }

        const std::string& getMessage() const { return m_message; }
        void setMessage(const std::string& message) { m_message = message; }

        const int getId() const { return m_id; }
        void setId(const int id) { m_id = id; }

    private:
        int m_id;
        rpl::UserInfoPacket m_friend_info;
        rpl::UserInfoPacket m_my_info;
        std::string m_message;
    };
}

namespace rcv {

    std::pair<Response, std::string> parseResponse(const std::string& query);

    class ChatSuccessPacket : public Packet {
    public:
        ChatSuccessPacket() {}
        static ChatSuccessPacket deserialize(const std::string& str);

        const rpl::UserInfoPacket& getUserInfoPacket() const { return m_packet; }
        void setUserInfoPacket(const rpl::UserInfoPacket& info) { m_packet = info; }

    private:
        rpl::UserInfoPacket m_packet;
    };

    class StatusPacket {
    public:
        StatusPacket() : m_status(Response::EMPTY_RESPONSE) {}
        static StatusPacket deserialize(const std::string& str);

        const Response getStatus() const { return m_status; }
        void setStatus(const Response status) { m_status = status; }

    private:
        Response m_status;
    };



    class FriendStatePacket : public Packet {
    public:
        FriendStatePacket() : m_isOnline(false) {}
        static FriendStatePacket deserialize(const std::string& str);

        const std::string& getLogin() const { return m_friend_login; }
        void setLogin(std::string& login) { m_friend_login = login; }

        const std::string& getLastSeen() const { return m_last_seen; }
        void setLastSeen(std::string& lastSeen) { m_last_seen = lastSeen; }

        const bool getIsOnline() const { return m_isOnline; }
        void setIsOnline(bool isOnline) { m_isOnline = isOnline; }

    private:
        std::string  m_friend_login;
        std::string  m_last_seen;
        bool         m_isOnline;
    };

    class FriendsStatusesPacket : public Packet {
    public:
        FriendsStatusesPacket() {}
        static FriendsStatusesPacket deserialize(const std::string& str);

        std::vector<std::pair<std::string, std::string>>& getVecStatuses() { return m_vec_statuses; }

    private:
        std::vector<std::pair<std::string, std::string>> m_vec_statuses;
    };
}



enum class Query {
    NEXT_QUERY_SIZE,
    AUTHORIZATION,
    REGISTRATION,
    CREATE_CHAT,
    GET_USER_INFO,
    UPDATE_USER_INFO,
    MESSAGE,
    GET_FRIENDS_STATUSES,
    GET_ALL_FRIENDS_STATES

};

namespace snd {
    class UpdateUserInfoPacket : public Packet {
    public:
        UpdateUserInfoPacket() {}
        std::string serialize();

        const std::string& getLogin() const { return m_user_login; }
        void setLogin(const std::string& login) { m_user_login = login; }

        const std::string& getName() const { return m_user_name; }
        void setName(const std::string& name) { m_user_name = name; }

        const std::string& getPassword() const { return m_user_password; }
        void setPassword(const std::string& password) { m_user_password = password; }

        const Photo& getPhoto() const { return m_user_photo; }
        void setLogin(const Photo& photo) { m_user_photo = photo; }

    private:
        std::string m_user_login;
        std::string m_user_name;
        std::string m_user_password;
        Photo       m_user_photo;
    };


    class AuthorizationPacket : public Packet {
    public:
        AuthorizationPacket() {}
        std::string serialize();

        const std::string& getLogin() const { return m_login; }
        void setLogin(const std::string& login) { m_login = login; }

        const std::string& getPassword() const { return m_password; }
        void setPassword(const std::string& password) { m_password = password; }

    private:
        std::string m_login;
        std::string m_password;
    };


    class RegistrationPacket : public Packet {
    public:
        RegistrationPacket() {}
        std::string serialize();

        const std::string& getLogin() const { return m_login; }
        void setLogin(const std::string& login) { m_login = login; }

        const std::string& getPassword() const { return m_password; }
        void setPassword(const std::string& password) { m_password = password; }

        const std::string& getName() const { return m_name; }
        void setName(const std::string& name) { m_name = name; }

    private:
        std::string m_login;
        std::string m_password;
        std::string m_name;

    };


    class CreateChatPacket : public Packet {
    public:
        CreateChatPacket() {}
        std::string serialize();

        const std::string& getFriendLogin() const { return m_friend_login; }
        void setFriendLogin(const std::string& login) { m_friend_login = login; }

        const std::string& getMyLogin() const { return m_my_login; }
        void setMyLogin(const std::string& login) { m_my_login = login; }

    private:
        std::string m_my_login;
        std::string m_friend_login;
    };


    class GetUserInfoPacket : public Packet {
    public:
        GetUserInfoPacket() {}
        std::string serialize();

        const std::string& getLogin() const { return m_user_login; }
        void setLogin(const std::string& login) { m_user_login = login; }

    private:
        std::string m_user_login;
    };

    class GetFriendsStatusesPacket : public Packet {
    public:
        GetFriendsStatusesPacket() {}
        std::string serialize();
        
        std::vector<std::string>& getFriendsLoginsVec() { return m_vec_friends_logins; }
        void setFriendsLoginsVec(const std::vector<std::string>& friendsLoginsVec) { m_vec_friends_logins = friendsLoginsVec; }

    private:
        std::vector<std::string> m_vec_friends_logins;
    };

}

