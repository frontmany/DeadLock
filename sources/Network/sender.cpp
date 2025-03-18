#include"sender.h"
#include"photo.h"
#include"message.h"


std::string SendStringsGenerator::parseTypeToStr(QueryType type) const {
    switch (type) {
    case QueryType::AUTHORIZATION:              return "AUTHORIZATION";
    case QueryType::REGISTRATION:               return "REGISTRATION";
    case QueryType::CREATE_CHAT:                return "CREATE_CHAT";
    case QueryType::UPDATE_MY_INFO:             return "UPDATE_MY_INFO";
    case QueryType::MESSAGE:                    return "MESSAGE";
    case QueryType::FIRST_MESSAGE:              return "FIRST_MESSAGE";
    case QueryType::STATUS:                     return "STATUS";
    case QueryType::MESSAGES_READ_CONFIRMATION: return "MESSAGES_READ_CONFIRMATION";
    case QueryType::LOAD_FRIEND_INFO:           return "LOAD_FRIEND_INFO";
    case QueryType::LOAD_ALL_FRIENDS_STATUSES:  return "LOAD_ALL_FRIENDS_STATUSES";
    }
}

std::string SendStringsGenerator::get_loadFriendInfo_QueryStr(const std::string& login) const {
    return get + '\n' + parseTypeToStr(QueryType::LOAD_FRIEND_INFO) + '\n' + login + '\n' + endPacket;
}

std::string SendStringsGenerator::get_authorization_QueryStr(const std::string& login, const std::string& password) const {
    return get + '\n' + parseTypeToStr(QueryType::AUTHORIZATION) + '\n' + login + '\n' + password + '\n' + endPacket;
}

std::string SendStringsGenerator::get_registration_QueryStr(const std::string& login, const std::string& name, const std::string& password) const {
    return get + '\n' + parseTypeToStr(QueryType::REGISTRATION) + '\n' + login + '\n' + name + '\n' + password + '\n' + endPacket;
}

std::string SendStringsGenerator::get_message_ReplyStr(const std::string& myLogin, const std::string& friendLogin, const std::string& message, const std::string& id, const std::string& timestamp) const {
    return rpl + '\n' + friendLogin + '\n' + parseTypeToStr(QueryType::MESSAGE) + '\n' + myLogin + '\n' + messageBegin + '\n'
        + message + '\n' + messageEnd + '\n' + id + '\n' + timestamp + '\n' + endPacket;
}

std::string SendStringsGenerator::get_first_message_ReplyStr(const std::string& myLogin, const std::string& myName, bool isHAsPhoto, Photo ph, const std::string& friendLogin, const std::string& message, const std::string& id, const std::string& timestamp) const {
    return rpl + '\n' + friendLogin + '\n' + parseTypeToStr(QueryType::FIRST_MESSAGE) + '\n' + myLogin + '\n' + messageBegin + '\n'
        + message + '\n' + messageEnd + '\n' + id + '\n' + timestamp + '\n' + myName + '\n' + (isHAsPhoto? "true" : "false") + '\n' + ph.serialize() + '\n' + endPacket;
}

std::string SendStringsGenerator::get_messageReadConfirmation_ReplyStr(const std::string& myLogin, const std::string& friendLogin, const std::vector<Message*>& readMessagesVec) const {
    std::string replyStr = rpl + '\n' + friendLogin + '\n' + parseTypeToStr(QueryType::MESSAGES_READ_CONFIRMATION) + '\n' + myLogin + '\n' + vecBegin + '\n';
    for (auto message : readMessagesVec) {
        replyStr += message->getId() + '\n';
    }
    replyStr += vecEnd + '\n';
    replyStr += endPacket;

    return replyStr;
}

std::string SendStringsGenerator::get_status_ReplyStr(const std::string& status, const std::string& myLogin, std::vector<std::string>& friendsLoginsVec) const {
    std::string replyStr = broadcast + '\n' + parseTypeToStr(QueryType::STATUS) + '\n' + status + '\n' + myLogin + '\n';
    replyStr += vecBegin + '\n';
    for (auto login : friendsLoginsVec) {
        replyStr += login + '\n';
    }
    replyStr += vecEnd + '\n';
    replyStr += endPacket;

    return replyStr;
}

std::string SendStringsGenerator::get_createChat_QueryStr(const std::string& myLogin, const std::string& friendLogin) const {
    return get + '\n' + parseTypeToStr(QueryType::CREATE_CHAT) + '\n' + myLogin + '\n'
        + friendLogin + '\n' + endPacket;
}

std::string SendStringsGenerator::get_updateMyInfo_QueryStr(std::string login, std::string name, std::string password, bool isHasPhoto, Photo photo) const {
    return get + '\n' + parseTypeToStr(QueryType::UPDATE_MY_INFO) + '\n' + login + '\n'
        + name + '\n' + password + '\n' + (isHasPhoto == true ? "true" : "false") + '\n' + photo.serialize() + '\n' + endPacket;
}

std::string SendStringsGenerator::get_loadAllFriendsStatuses_QueryStr(std::vector<std::string>& friendsLoginsVec) {
    std::string queryStr = get + '\n' + parseTypeToStr(QueryType::LOAD_ALL_FRIENDS_STATUSES) + '\n' + vecBegin + '\n';
    for (auto login : friendsLoginsVec) {
        queryStr += login;
        queryStr += '\n';
    }

    queryStr += vecEnd + '\n';
    queryStr += endPacket;
    return queryStr;
}
