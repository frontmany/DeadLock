#include"request.h"

std::string SizePacket::serialize() {
    std::size_t length = m_serialized_data_object.size();
    std::string lengthStr = std::to_string(length);
    std::ostringstream oss;
    oss << "NEXT_QUERY_SIZE" << "\n"
        << lengthStr;
    return oss.str();
}

size_t SizePacket::deserialize(const std::string& str) {
    std::istringstream iss(str);
    std::string sizeStr;
    std::getline(iss, sizeStr);
    std::size_t value = std::stoul(sizeStr);
    return value;
}

using namespace snd;

std::string GetFriendsStatusesPacket::serialize() {
    std::ostringstream oss;
    oss << "GET_FRIENDS_STATUSES" << "\n";
    for (const auto& elem : m_vec_friends_logins) {
        oss << elem << " ";
    }
    return oss.str();
}

std::string AuthorizationPacket::serialize() {
    std::ostringstream oss;
    oss << "AUTHORIZATION" << "\n" 
        << m_login << '\n'
        << m_password;
    return oss.str();
}

std::string RegistrationPacket::serialize() {
    std::ostringstream oss;
    oss << "REGISTRATION" << "\n"
        << m_login << '\n'
        << m_password << '\n'
        << m_name;
    return oss.str();
}

std::string CreateChatPacket::serialize() {
    std::ostringstream oss;
    oss << "CREATE_CHAT" << "\n"
        << m_my_login << "\n"
        << m_friend_login;
    return oss.str();
}

std::string GetUserInfoPacket::serialize() {
    std::ostringstream oss;
    oss << "GET_USER_INFO" << "\n"
        << m_user_login;
    return oss.str();
}

std::string UpdateUserInfoPacket::serialize() {
    std::ostringstream oss;
    oss << "UPDATE_USER_INFO" << "\n"
        << m_user_login << '\n'
        << m_user_name << '\n'
        << m_user_password << '\n'
        << m_user_photo.serialize();
    return oss.str();
}

using namespace rpl;
Message Message::deserialize(const std::string& str) {
    std::istringstream iss(str);
    Message  message;

    std::string idStr;
    std::getline(iss, idStr);
    message.m_id = std::stoi(idStr);

    std::getline(iss, message.m_message);

    std::ostringstream remainingStream1;
    std::ostringstream remainingStream2;
    std::string line;

    bool fl = true;
    while (std::getline(iss, line)) {
        if (line != ":") {
            if (fl) {
                remainingStream1 << line;
                remainingStream1 << "\n";
            }
            else {
                remainingStream2 << line;
                remainingStream2 << "\n";
            }
        }
        else {
            fl = false;
        }

    }
    std::string remainingPart1 = remainingStream1.str();
    UserInfoPacket packSender = UserInfoPacket::deserialize(remainingPart1);

    std::string remainingPart2 = remainingStream2.str();
    UserInfoPacket packReceiver = UserInfoPacket::deserialize(remainingPart2);

    //swap buffers
    message.setFriendInfo(packSender);
    message.setMyInfo(packReceiver);

    return message;
}

std::string Message::serialize() {
    std::ostringstream oss;
    oss << "MESSAGE" << "\n"
        << m_id << '\n' 
        << m_message << '\n'
        << m_my_info.serialize() << '\n'
        << ":" << '\n'
        << m_friend_info.serialize();
    return oss.str();
}


std::string UserInfoPacket::serialize() {
    std::ostringstream oss;
    oss << "USER_INFO_FOUND" << "\n"
        << m_user_login << '\n'
        << m_user_name << '\n'
        << m_last_seen << '\n';

    std::string photo_serialized_str = m_user_photo.serialize();
    oss << photo_serialized_str << '\n';

    oss << (m_isOnline ? "true" : "false") << '\n'
        << (m_isHasPhoto ? "true" : "false");

    return oss.str();
}

UserInfoPacket UserInfoPacket::deserialize(const std::string& str) {
    std::istringstream iss(str);
    std::string line;
    UserInfoPacket packet;

    std::string lineType; // reads USER_INFO_FOUND
    std::getline(iss, lineType);
    std::getline(iss, packet.m_user_login);
    std::getline(iss, packet.m_user_name);
    std::getline(iss, packet.m_last_seen);

    std::string photo_serialized_str;
    std::getline(iss, photo_serialized_str);
    packet.m_user_photo = Photo::deserialize(photo_serialized_str);

    std::getline(iss, line);
    packet.m_isOnline = (line == "true");

    std::getline(iss, line);
    packet.m_isHasPhoto = (line == "true");

    return packet;
}


using namespace rcv;
ChatSuccessPacket ChatSuccessPacket::deserialize(const std::string& str) {
    ChatSuccessPacket pack;
    pack.setUserInfoPacket(UserInfoPacket::deserialize(str));
    return pack;
}

std::pair<Response, std::string> rcv::parseResponse(const std::string& response) {
    std::istringstream iss(response);

    std::string responseTypeStr;
    std::getline(iss, responseTypeStr);
    StatusPacket packet = StatusPacket::deserialize(responseTypeStr);

    std::ostringstream remainingStream;
    std::string line;
    while (std::getline(iss, line)) {
        remainingStream << line;
        remainingStream << "\n";
    }

    std::string remainingPart = remainingStream.str();
    if (!remainingPart.empty() && remainingPart.back() == '\n') {
        remainingPart.pop_back();
    }

    return std::make_pair(packet.getStatus(), remainingPart);

}



StatusPacket StatusPacket::deserialize(const std::string& str) {
    Response response = Response::EMPTY_RESPONSE;

    if (str == "EMPTY_RESPONSE") {
        response = Response::EMPTY_RESPONSE;
    }
    else if (str == "AUTHORIZATION_SUCCESS") {
        response = Response::AUTHORIZATION_SUCCESS;
    }
    else if (str == "REGISTRATION_SUCCESS") {
        response = Response::REGISTRATION_SUCCESS;
    }
    else if (str == "AUTHORIZATION_FAIL") {
        response = Response::AUTHORIZATION_FAIL;
    }
    else if (str == "REGISTRATION_FAIL") {
        response = Response::REGISTRATION_FAIL;
    }
    else if (str == "CHAT_CREATE_SUCCESS") {
        response = Response::CHAT_CREATE_SUCCESS;
    }
    else if (str == "CHAT_CREATE_FAIL") {
        response = Response::CHAT_CREATE_FAIL;
    }
    else if (str == "USER_INFO_FOUND") {
        response = Response::USER_INFO_FOUND;
    }
    else if (str == "USER_INFO_NOT_FOUND") {
        response = Response::USER_INFO_NOT_FOUND;
    }
    else if (str == "USER_INFO_UPDATED") {
        response = Response::USER_INFO_UPDATED;
    }
    else if (str == "USER_INFO_NOT_UPDATED") {
        response = Response::USER_INFO_NOT_UPDATED;
    }
    else if (str == "FRIEND_STATE_CHANGED") {
        response = Response::FRIEND_STATE_CHANGED;
    }
    else if (str == "ALL_FRIENDS_STATES") {
        response = Response::ALL_FRIENDS_STATES;
    }
    else if (str == "NEXT_QUERY_SIZE") {
        response = Response::NEXT_QUERY_SIZE;
    }
    else {
        printf("receiving error");
    }
    StatusPacket packet;
    packet.setStatus(response); 
    return packet;
}

FriendStatePacket FriendStatePacket::deserialize(const std::string& str) {
    std::istringstream iss(str);
    std::string line;
    FriendStatePacket packet;
    std::getline(iss, packet.m_friend_login);
    std::getline(iss, packet.m_last_seen);
    return packet;
}

FriendsStatusesPacket FriendsStatusesPacket::deserialize(const std::string& str) {
    FriendsStatusesPacket pack;
    std::istringstream iss(str);
    std::string token;

    while (std::getline(iss, token, ',')) {
        std::string first, second;
        std::istringstream pairStream(token);

        if (std::getline(pairStream, first, ':') && std::getline(pairStream, second)) {
            pack.getVecStatuses().emplace_back(first, second);
        }
    }

    return pack;
}

