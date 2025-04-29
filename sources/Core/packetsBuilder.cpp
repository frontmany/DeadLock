#include"packetsBuilder.h"
#include"photo.h"
#include"message.h"
#include"queryType.h"

//GET
const std::string PacketsBuilder::getAuthorizationPacket(const std::string& login,
    const std::string& passwordHash)
{
    std::ostringstream oss;
    oss << get << '\n'
        << login << '\n'
        << passwordHash << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getRegistrationPacket(const std::string& login,
    const std::string& name,
    const std::string& passwordHash) 
{
    std::ostringstream oss;
    oss << get << '\n'
        << login << '\n'
        << name << '\n'
        << passwordHash << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getCreateChatPacket(const std::string& myLogin,
    const std::string& friendLogin) 
{
    std::ostringstream oss;
    oss << get << '\n'
        << myLogin << '\n'
        << friendLogin << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyNamePacket(
    const std::string& login,
    const std::string& newName,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;
    oss << get << '\n'
        << login << '\n'
        << newName << '\n';

    oss << vecBegin << '\n';
    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }
    oss << vecEnd << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyPasswordPacket(
    const std::string& login,
    const std::string& newPassword,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;
    oss << get << '\n'
        << login << '\n'
        << newPassword << '\n';

    oss << vecBegin << '\n';
    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }
    oss << vecEnd << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getUpdateMyPhotoPacket(
    const std::string& login,
    const Photo& photo,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;
    oss << get << '\n'
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

    return oss.str();
}

const std::string  PacketsBuilder::getUpdateMyLoginPacket(const std::string& login, const std::string& newLogin, const std::vector<std::string>& friendsLoginsVec) {
    std::ostringstream oss;
    oss << get << '\n'
        << login << '\n'
        << newLogin << '\n';

    oss << vecBegin << '\n';
    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }
    oss << vecEnd << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getLoadUserInfoPacket(const std::string& login) {
    std::ostringstream oss;
    oss << get << '\n'
        << login << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getVerifyPasswordPacket(const std::string& login, const std::string& passwordHash) {
    std::ostringstream oss;

    oss << get << '\n'
        << login << '\n'
        << passwordHash;

    return oss.str();
}

const std::string PacketsBuilder::getLoadAllFriendsStatusesPacket(const std::vector<std::string>& friendsLoginsVec) {
    std::ostringstream oss;

    oss << get << '\n'
        << vecBegin << '\n';

    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }

    oss << vecEnd << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getCheckIsNewLoginAvailablePacket(const std::string& newLogin) {
    std::ostringstream oss;
    oss << get << '\n'
        << newLogin;

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
        << myLogin << '\n'
        << messageBegin << '\n'
        << message->getMessage() << '\n'
        << messageEnd << '\n'
        << message->getId() << '\n'
        << message->getTimestamp() << '\n';

    return oss.str();
}

const std::string PacketsBuilder::getMessageReadConfirmationPacket(const std::string& myLogin,
    const std::string& friendLogin,
    const Message* message)
{
    std::ostringstream oss;
    oss << rpl << '\n'
        << friendLogin << '\n'
        << myLogin << '\n'
        << message->getId() << '\n';

    return oss.str();
}


//BROADCAST
const std::string PacketsBuilder::getStatusPacket(const std::string& status,
    const std::string& myLogin,
    const std::vector<std::string>& friendsLoginsVec)
{
    std::ostringstream oss;

    oss << broadcast << '\n'
        << status << '\n'
        << myLogin << '\n'
        << vecBegin << '\n';

    for (const auto& login : friendsLoginsVec) {
        oss << login << '\n';
    }

    oss << vecEnd << '\n';

    return oss.str();
}