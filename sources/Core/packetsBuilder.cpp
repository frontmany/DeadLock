#include"packetsBuilder.h"
#include"photo.h"
#include"message.h"
#include"queryType.h"


const std::string PacketsBuilder::parseTypeToStr(QueryType type) {
    switch (type) {
    case QueryType::AUTHORIZATION:              return "AUTHORIZATION";
    case QueryType::REGISTRATION:               return "REGISTRATION";
    case QueryType::CREATE_CHAT:                return "CREATE_CHAT";
    case QueryType::UPDATE_MY_NAME:             return "UPDATE_MY_NAME";
    case QueryType::UPDATE_MY_PASSWORD:         return "UPDATE_MY_PASSWORD";
    case QueryType::UPDATE_MY_PHOTO:            return "UPDATE_MY_PHOTO";
    case QueryType::MESSAGE:                    return "MESSAGE";
    case QueryType::STATUS:                     return "STATUS";
    case QueryType::MESSAGES_READ_CONFIRMATION: return "MESSAGES_READ_CONFIRMATION";
    case QueryType::LOAD_FRIEND_INFO:           return "LOAD_FRIEND_INFO";
    case QueryType::LOAD_ALL_FRIENDS_STATUSES:  return "LOAD_ALL_FRIENDS_STATUSES";
    }
}

const std::string& PacketsBuilder::getEndPacketString() {
    return endPacket;
}

//GET
const std::string PacketsBuilder::getAuthorizationPacket(const std::string& login,
    const std::string& passwordHash)
{
    std::ostringstream oss;
    oss << get << '\n'
        << parseTypeToStr(QueryType::AUTHORIZATION) << '\n'
        << login << '\n'
        << passwordHash << '\n'
        << endPacket;
    return oss.str();
}

const std::string PacketsBuilder::getRegistrationPacket(const std::string& login,
    const std::string& name,
    const std::string& passwordHash) 
{
    std::ostringstream oss;
    oss << get << '\n'
        << parseTypeToStr(QueryType::REGISTRATION) << '\n'
        << login << '\n'
        << name << '\n'
        << passwordHash << '\n'
        << endPacket;
    return oss.str();
}

const std::string PacketsBuilder::getCreateChatPacket(const std::string& myLogin,
    const std::string& friendLogin) 
{
    std::ostringstream oss;
    oss << get << '\n'
        << parseTypeToStr(QueryType::CREATE_CHAT) << '\n'
        << myLogin << '\n'
        << friendLogin << '\n'
        << endPacket;
    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyNamePacket(
    const std::string& login,
    const std::string& newName,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;
    oss << get << '\n'
        << parseTypeToStr(QueryType::UPDATE_MY_NAME) << '\n'
        << login << '\n'
        << newName << '\n';

    oss << vecBegin << '\n';
    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }
    oss << vecEnd << '\n';

    oss << endPacket;
    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyPasswordPacket(
    const std::string& login,
    const std::string& newPassword,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;
    oss << get << '\n'
        << parseTypeToStr(QueryType::UPDATE_MY_PASSWORD) << '\n'
        << login << '\n'
        << newPassword << '\n';

    oss << vecBegin << '\n';
    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }
    oss << vecEnd << '\n';

    oss << endPacket;
    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyPhotoPacket(
    const std::string& login,
    const Photo& photo,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;
    oss << get << '\n'
        << parseTypeToStr(QueryType::UPDATE_MY_PHOTO) << '\n'
        << login << '\n';

    oss << vecBegin << '\n';
    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }

    oss << vecEnd << '\n';
    oss << "true" << '\n';
    oss << std::to_string(photo.getSize()) << '\n';

    std::string photoString = photo.serialize();
    oss << photoString << '\n';

    oss << endPacket;
    return oss.str();
}

const std::string PacketsBuilder::getLoadUserInfoPacket(const std::string& login) {
    std::ostringstream oss;
    oss << get << '\n'
        << parseTypeToStr(QueryType::LOAD_FRIEND_INFO) << '\n'
        << login << '\n'
        << endPacket;
    return oss.str();
}

const std::string PacketsBuilder::getLoadAllFriendsStatusesPacket(const std::vector<std::string>& friendsLoginsVec) {
    std::ostringstream oss;

    oss << get << '\n'
        << parseTypeToStr(QueryType::LOAD_ALL_FRIENDS_STATUSES) << '\n'
        << vecBegin << '\n';

    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }

    oss << vecEnd << '\n'
        << endPacket;

    return oss.str();
}


//RPL
const std::string PacketsBuilder::getMessagePacket(const std::string& myLogin,
    const std::string& friendLogin,
    const Message* message)
{
    std::ostringstream oss;
    oss << rpl << '\n'
        << friendLogin << '\n'
        << parseTypeToStr(QueryType::MESSAGE) << '\n'
        << myLogin << '\n'
        << messageBegin << '\n'
        << message->getMessage() << '\n'
        << messageEnd << '\n'
        << message->getId() << '\n'
        << message->getTimestamp() << '\n'
        << endPacket;

    return oss.str();
}

const std::string PacketsBuilder::getMessageReadConfirmationPacket(const std::string& myLogin,
    const std::string& friendLogin,
    const Message* message)
{
    std::ostringstream oss;
    oss << rpl << '\n'
        << friendLogin << '\n'
        << parseTypeToStr(QueryType::MESSAGES_READ_CONFIRMATION) << '\n'
        << myLogin << '\n'
        << message->getId() << '\n'
        << endPacket;

    return oss.str();
}


//BROADCAST
const std::string PacketsBuilder::getStatusPacket(const std::string& status,
    const std::string& myLogin,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;

    oss << broadcast << '\n'
        << parseTypeToStr(QueryType::STATUS) << '\n'
        << status << '\n'
        << myLogin << '\n'
        << vecBegin << '\n';

    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }

    oss << vecEnd << '\n'
        << endPacket;

    return oss.str();
}