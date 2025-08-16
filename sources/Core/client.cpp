#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <codecvt>
#include <locale>
#include <QFile>
#include <QDir>


#include "packetsBuilder.h"
#include "configManager.h"
#include "avatarInfo.h"
#include "keysManager.h"
#include "responseHandler.h"
#include "net_networkManager.h"
#include "file.h"
#include "message.h"
#include "packetType.h"
#include "workerUI.h"
#include "utility.h"
#include "client.h"
#include "avatar.h"
#include "chat.h"

Client::Client() :
    m_isConnectionDown(false),
    m_isAbleToClose(true),
    m_isFirstAuthentication(true),
    m_isPassedAuthentication(false),
    m_configManager(nullptr),
    m_serverIpAddress(""),
    m_serverPort(0),
    m_responseHandler(this, m_configManager)
{
    m_keysManager = std::make_shared<KeysManager>();
    m_configManager = std::make_shared<ConfigManager>();
    m_configManager->setKeysManagerPtr(m_keysManager);
    m_networkManager = std::make_shared<net::NetworkManager>(*this);
}

bool Client::waitForConnectionWithTimeout(int timeoutMs) {
    auto startTime = std::chrono::steady_clock::now();
    while (!m_networkManager->isPacketsConnectionConnected()) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime).count() > timeoutMs) {
            return false; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true; 
}

void Client::initDatabase(const std::string& loginHash) {
    m_db.init(loginHash);
}

void Client::startProcessingIncomingPackets() { 
    m_packetsProcessingThread = std::thread([this]() { m_networkManager->startProcessingIncomingPackets(); });
}

void Client::startProcessingIncomingBlobs() {
    m_blobsProcessingThread = std::thread([this]() { m_networkManager->startProcessingIncomingBlobs(); });
}

void Client::connectTo(const std::string& ipAddress, int port) {
    m_serverIpAddress = ipAddress;
    m_serverPort = port;
    m_networkManager->createPacketsConnection(ipAddress, port);
    m_networkManager->runContextThread();
}

void Client::generateMyKeyPair() {
    if (m_keysManager->getMyPublicKey().GetModulus().IsZero() || m_keysManager->getMyPrivateKey().GetModulus().IsZero()) {
        CryptoPP::RSA::PublicKey publicKey;
        CryptoPP::RSA::PrivateKey privateKey;
        utility::generateRSAKeyPair(privateKey, publicKey);

        m_keysManager->setMyPublicKey(publicKey);
        m_keysManager->setMyPrivateKey(privateKey);
    }
}










// send packet operations
bool Client::tryReconnect() {
    m_networkManager->reconnectPacketsConnection();

    bool isConnected = waitForConnectionWithTimeout(2500);
    if (isConnected) {
        m_networkManager->sendPacket(PacketsBuilder::getReconnectPacket(m_configManager->getMyLoginHash(), m_configManager->getMyPasswordHash()), PacketType::RECONNECT);
        return true;
    }
    else {
        return false;
    }
}

void Client::typingNotify(const std::string& friendUID, bool isTyping) {
    auto it = m_mapChats.find(friendUID);
    auto& [hash, chat] = *it;
    m_networkManager->sendPacket(PacketsBuilder::getTypingPacket(chat->getFriendPublicKey(), m_configManager->getMyUID(), friendUID, isTyping), PacketType::TYPING);
}

void Client::authorizeClient(const std::string& loginHash, const std::string& passwordHash) {
    m_configManager->setMyLoginHash(loginHash);
    m_configManager->setMyPasswordHash(passwordHash);
    m_networkManager->sendPacket(PacketsBuilder::getAuthorizationPacket(loginHash, passwordHash), PacketType::AUTHORIZATION);
}

void Client::registerClient(const std::string& login, const std::string& password, const std::string& name) {
    // if registration will fail fields will be set empty in "onRegistrationFail" function
    m_configManager->setMyLogin(login);
    m_configManager->setMyLoginHash(utility::calculateHash(login));
    m_configManager->setMyName(name);
    m_configManager->setMyPasswordHash(utility::calculateHash(password));

    m_networkManager->sendPacket(PacketsBuilder::getRegistrationPacket(m_configManager->getMyLoginHash(), m_configManager->getMyPasswordHash()), PacketType::REGISTRATION);
}

void Client::sendMyInfoAfterRegistration() {
    m_tasksManager.addSendCommonPacketTask(PacketType::AFTER_RREGISTRATION_SEND_MY_INFO);
    m_networkManager->sendPacket(PacketsBuilder::getAfterRegistrationInfoPacket(m_keysManager->getServerPublicKey(), m_keysManager->getMyPublicKey(), m_configManager->getMyLogin(), m_configManager->getMyName()), PacketType::AFTER_RREGISTRATION_SEND_MY_INFO);
}

void Client::createChatWith(const std::string& supposedFriendLogin) {
    m_networkManager->sendPacket(PacketsBuilder::getCreateChatPacket(m_configManager->getMyUID(), utility::calculateHash(supposedFriendLogin)), PacketType::CREATE_CHAT);
}

void Client::broadcastMyStatus(const std::string& status) {
    const std::vector<std::string>& tmpFriendsUIDsVec = getFriendsUIDsVecFromMap();
    m_networkManager->sendPacket(PacketsBuilder::getStatusPacket(m_keysManager->getServerPublicKey(), status, m_configManager->getMyUID(), tmpFriendsUIDsVec), PacketType::STATUS);
}

void Client::sendMessageReadConfirmation(const std::string& friendUID, const std::string& messageId) {
    auto it = m_mapChats.find(friendUID);
    if (it != m_mapChats.end()) {
        m_tasksManager.addSendReadConfirmationTask(messageId, friendUID);
        m_networkManager->sendPacket(PacketsBuilder::getMessageReadConfirmationPacket(it->second->getFriendPublicKey(), m_configManager->getMyUID(), utility::calculateHash(friendUID), messageId), PacketType::MESSAGE_READ_CONFIRMATION);
    }
}

void Client::sendBlobReadConfirmation(const std::string& friendUID, const std::string& blobId) {
    auto it = m_mapChats.find(friendUID);
    if (it != m_mapChats.end()) {
        m_tasksManager.addSendReadConfirmationTask(blobId, friendUID);
        m_networkManager->sendPacket(PacketsBuilder::getBlobReadConfirmationPacket(it->second->getFriendPublicKey(), m_configManager->getMyUID(), utility::calculateHash(friendUID), blobId), PacketType::BLOB_READ_CONFIRMATION);
    }
}

void Client::getAllFriendsStatuses() {
    m_networkManager->sendPacket(PacketsBuilder::getLoadAllFriendsStatusesPacket(m_configManager->getMyUID(), getFriendsUIDsVecFromMap()), PacketType::LOAD_ALL_FRIENDS_STATUSES);
}

void Client::findUser(const std::string& searchText) {
    m_networkManager->sendPacket(PacketsBuilder::getFindUserPacket(m_keysManager->getServerPublicKey(), m_configManager->getMyUID(), searchText), PacketType::FIND_USER);
}

void Client::requestUserInfoFromServer(const std::string& friendUID, const std::string& myUID) {
    m_networkManager->sendPacket(PacketsBuilder::getLoadUserInfoPacket(friendUID, myUID), PacketType::LOAD_USER_INFO);
}

void Client::requestUpdate() {
    m_networkManager->sendPacket(PacketsBuilder::getUpdateRequestPacket(m_keysManager->getServerPublicKey(), m_configManager->getMyUID(), m_configManager->getVersionNumberToUpdate()), PacketType::UPDATE_REQUEST);
}

void Client::verifyPassword(const std::string& passwordHash) {
    m_networkManager->sendPacket(PacketsBuilder::getVerifyPasswordPacket(m_configManager->getMyUID(), passwordHash), PacketType::VERIFY_PASSWORD);
}

void Client::checkIsNewLoginAvailable(const std::string& newLogin) {
    m_networkManager->sendPacket(PacketsBuilder::getCheckIsNewLoginAvailablePacket(m_keysManager->getServerPublicKey(), m_configManager->getMyUID(), newLogin), PacketType::CHECK_NEW_LOGIN);
}

void Client::updateMyLogin(const std::string& newLogin) {
    if (m_configManager->getMyLogin() == newLogin) {
        return;
    }

    std::string oldLoginHash = m_configManager->getMyLoginHash();
    std::string newLoginHash = utility::calculateHash(newLogin);

    m_configManager->setMyLogin(newLogin);
    m_configManager->setMyLoginHash(newLoginHash);

    m_tasksManager.addSendCommonPacketTask(PacketType::UPDATE_MY_LOGIN);

    m_networkManager->sendPacket(PacketsBuilder::getUpdateMyLoginPacket(m_keysManager->getServerPublicKey(), oldLoginHash, newLoginHash, newLogin), PacketType::UPDATE_MY_LOGIN);
}

void Client::updateMyAvatar(AvatarPtr newAvatar) {
    m_tasksManager.addSendCommonPacketTask(PacketType::UPDATE_MY_AVATAR);
    m_networkManager->sendAvatar(m_configManager->getMyUID(), newAvatar);
}

void Client::updateMyName(const std::string& newName) {
    m_configManager->setMyName(newName);

    m_tasksManager.addSendCommonPacketTask(PacketType::UPDATE_MY_NAME);

    m_networkManager->sendPacket(PacketsBuilder::getUpdateMyNamePacket(m_keysManager->getServerPublicKey(), m_configManager->getMyUID(), newName, getFriendsUIDsVecFromMap()), PacketType::UPDATE_MY_NAME);
}

void Client::updateMyPassword(const std::string& newPasswordHash) {
    m_configManager->setMyPasswordHash(newPasswordHash);

    m_tasksManager.addSendCommonPacketTask(PacketType::UPDATE_MY_PASSWORD);

    m_networkManager->sendPacket(PacketsBuilder::getUpdateMyPasswordPacket(m_configManager->getMyUID(), newPasswordHash), PacketType::UPDATE_MY_PASSWORD);
}

void Client::requestFile(const std::string& fileId) {
    std::string packetStr = PacketsBuilder::getSendMeFilePacket(m_configManager->getMyUID(), fileId);
    m_db.addRequestedFileId(fileId);
    m_networkManager->sendPacket(packetStr, PacketType::SEND_ME_FILE);
}

void Client::sendMessage(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& friendUID, MessagePtr message) {
    message->setIsSending(true);
    m_networkManager->sendPacket(PacketsBuilder::getMessagePacket(friendPublicKey, m_configManager->getMyLoginHash(), friendUID, message->getId(), message->getMessage(), message->getTimestamp()), PacketType::MESSAGE);
}



/*
void Client::requestMyInfoFromServerAndResetKeys(const std::string& loginHash) {
    CryptoPP::RSA::PrivateKey newPrivateKey;
    CryptoPP::RSA::PublicKey newPublicKey;
    utility::generateRSAKeyPair(newPrivateKey, newPublicKey);

    setPublicKey(newPublicKey);
    setPrivateKey(newPrivateKey);

    sendPacket(m_packets_builder->getLoadMyInfoPacket(loginHash, newPublicKey), PacketType::LOAD_MY_INFO);
}
*/


//essantial functions
ChatPtr Client::findChat(const std::string& UID) const {
    auto it = m_mapChats.find(UID);

    if (it != m_mapChats.end()) {
        auto& [loginHash, chat] = *it;
        return chat;
    }
    else {
        return nullptr;
        std::cout << "\"updateInConfigFriendLogin\" cannot find friend UID: " << UID << std::endl;
    }
}

void Client::setWorkerUI(WorkerUI* workerImpl) {
    m_responseHandler.setWorkerUI(workerImpl);
}

std::vector<std::string> Client::getFriendsUIDsVecFromMap() {
    std::vector<std::string> result;
    result.reserve(m_mapChats.size());
    std::transform(m_mapChats.begin(),
        m_mapChats.end(),
        std::back_inserter(result),
        [](const auto& pair) { return pair.first; });

    return result;
}

void Client::waitUntilUIReadyToUpdate() {
    while (!m_isUIReadyToUpdate.load()) {
        std::this_thread::yield();
    }
}

void Client::deleteMessagesInDatabaseWith(const std::string& friendUID) {
    m_db.deleteAllMessages(friendUID);
}

void Client::deleteFriendFromChatsMap(const std::string& friendUID) {
    if (m_mapChats.contains(friendUID)) {
        m_mapChats.erase(friendUID);
    }
}

/*
void Client::sendBlob(BlobPtr blob) {
    auto workerUI = m_response_handler->getWorkerUI();
    workerUI->blockProfileEditing();

    auto& relatedFiles = filesMessage.getRelatedFiles();
    for (auto& wrapper : relatedFiles) {
        auto chat = m_mapChats[wrapper.file.receiverLoginHash];
        wrapper.file.friendPublicKey = chat->getPublicKey();
        sendFile(wrapper.file);
    }
}
*/

/*
void Client::retrySendFilesMessage(Message& filesMessage) {
    auto workerUI = m_response_handler->getWorkerUI();
    workerUI->blockProfileEditing();

    auto& relatedFiles = filesMessage.getRelatedFiles();
    for (auto& wrapper : relatedFiles) {
        if (wrapper.isNeedToRetry) {
            auto chat = m_mapChats[wrapper.file.receiverLoginHash];
            wrapper.file.friendPublicKey = chat->getPublicKey();
            sendFile(wrapper.file);
        }
    }
}
*/

/*
void Client::onSendMessageError(std::error_code ec, Message unsentMessage) {
    std::string messageStr;
    unsentMessage >> messageStr;
    std::istringstream iss(messageStr);

    PacketType type = static_cast<PacketType>(unsentMessage.header.type);

    if (type == PacketType::MESSAGE) {
        skipLines(iss, 1);

        std::string friendLoginHash;
        std::getline(iss, friendLoginHash);

        std::string id;
        std::getline(iss, id);

        auto chatPair = m_mapChats.find(friendLoginHash);
        if (chatPair != m_mapChats.end()) {
            Chat* chat = chatPair->second;

            auto& messagesVec = chat->getMessagesVec();
            auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&id](Message* msg) {
                return msg->getId() == id;
            });

            Message* msg = *msgChatIt;
            msg->setIsNeedToRetry(true);
            m_response_handler->getWorkerUI()->onMessageSendingError(friendLoginHash, msg);
        }
    }
    else if (type == PacketType::MESSAGES_READ_CONFIRMATION) {
        skipLines(iss, 1);

        std::string friendLoginHash;
        std::getline(iss, friendLoginHash);

        skipLines(iss, 2);

        std::string id;
        std::getline(iss, id);

        auto chatPair = m_mapChats.find(friendLoginHash);

        if (chatPair != m_mapChats.end()) {
            Chat* chat = chatPair->second;

            auto& messagesVec = chat->getMessagesVec();
            auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&id](Message* msg) {
                return msg->getId() == id;
                });
            Message* msg = *msgChatIt;
            msg->setIsRead(false);
        }
    }
    else if (type == PacketType::SEND_ME_FILE) {
        skipLines(iss, 2);
        
        std::string myLoginHash;
        std::getline(iss, myLoginHash);

        std::string fileId;
        std::getline(iss, fileId);

        std::string blobUID;
        std::getline(iss, blobUID);

        std::string friendLoginHash;
        std::getline(iss, friendLoginHash);

        net::File file;
        file.blobUID = blobUID;
        file.filesInBlobCount = -1;
        file.id = fileId;

        m_response_handler->getWorkerUI()->onRequestedFileError(friendLoginHash, { false, file });
    }
}

void Client::onSendFileError(std::error_code ec, net::File unsentFille) {
        auto itChat = m_mapChats.find(unsentFille.receiverLoginHash);
        auto& [friendLoginHash, chat] = *itChat;
        auto& messagesVec = chat->getMessagesVec();
        auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&unsentFille](Message* msg) {
            return msg->getId() == unsentFille.blobUID;
        });

        Message* msg = *msgChatIt;
        msg->setIsNeedToRetry(true);
        m_response_handler->getWorkerUI()->onMessageSendingError(chat->getFriendLogin(), msg);
}

void Client::onReceiveFileError(std::error_code ec, std::optional<net::File> unreadFile) {
    onConnectionDown();

    if (auto vec = m_db->getRequestedFiles(m_configManager->getMyLoginHash()); std::find(vec.begin(), vec.end(), unreadFile.value().id) != vec.end()) {
        m_response_handler->getWorkerUI()->onRequestedFileError(unreadFile.value().receiverLoginHash, { false, unreadFile.value() });
    }
}
*/

/*
void Client::onSendFileProgressUpdate(const InfoToWhom& info, uint32_t progressPercent) {
    m_responseHandler.getWorkerUI()->updateFileSendingProgress(file.receiverLoginHash, file, progressPercent);
}
*/

/*
void Client::onRequestedFileProgressUpdate(const InfoToWhom& info, uint32_t progressPercent) {
    m_responseHandler.getWorkerUI()->updateFileLoadingProgress(file.senderLoginHash, file, progressPercent);
}
*/







// callback handlers
void Client::onBlob(BlobPtr blob) {
    /*
    if (file.isAvatarPreview && file.isAvatar) {
        m_response_handler->onAvatarPreview(file);
        return;
    }
    else if (!file.isAvatarPreview && file.isAvatar) {
        m_response_handler->onAvatar(file);
    }
    else {
        m_is_able_to_close = false;
        m_response_handler->onFile(file);
        m_is_able_to_close = true;
    }
    */
}

void Client::onPacket(net::Packet&& packet) {
    m_isAbleToClose = false;
    m_responseHandler.handleResponse(packet);
    m_isAbleToClose = true;
}

void Client::onAvatarSent() {
    m_tasksManager.removeSendCommonPacketTask(PacketType::UPDATE_MY_AVATAR);
}

void Client::onPacketSent(net::Packet&& packet) {
    uint32_t type = packet.type();
    if (type == PacketType::UPDATE_MY_LOGIN ||
        type == PacketType::UPDATE_MY_NAME ||
        type == PacketType::UPDATE_MY_PASSWORD ||
        type == PacketType::AFTER_RREGISTRATION_SEND_MY_INFO)
    {
        m_tasksManager.removeSendCommonPacketTask(type);
    }
    else if (type == PacketType::BLOB_READ_CONFIRMATION ||
        type == PacketType::MESSAGE_READ_CONFIRMATION)
    {
        nlohmann::json jsonObj(packet.get());
        std::string id;
        if (jsonObj.contains(MESSAGE_ID)) {
            id = jsonObj[MESSAGE_ID];
        }
        else if (jsonObj.contains(BLOB_ID)) {
            id = jsonObj[BLOB_ID];
        }

        m_tasksManager.removeSendReadConfirmationTask(id);
    }
}

void Client::onConnectionDown() {
    m_networkManager->disconnectPacketsConnection();
    m_networkManager->disconnectFilesConnection();
    m_responseHandler.getWorkerUI()->onConnectionDown();
}





// GET && SET implementations
const Database& Client::getDatabase() const {
    return m_db;
}

bool Client::getIsUIReadyToUpdate() const {
    return m_isUIReadyToUpdate.load();
}

void Client::setIsUIReadyToUpdate(bool isUIReadyToUpdate) {
    m_isUIReadyToUpdate.store(isUIReadyToUpdate);
}

const std::string& Client::getServerIpAddress() const {
    return m_serverIpAddress;
}

void Client::setServerIpAddress(const std::string& ipAddress) {
    m_serverIpAddress = ipAddress;
}

int Client::getServerPort() const {
    return m_serverPort;
}

void Client::setServerPort(int port) {
    m_serverPort = port;
}

std::unordered_map<std::string, ChatPtr>& Client::getMyChatsMap() {
    return m_mapChats;
}

bool Client::getIsAbleToClose() const {
    return m_isAbleToClose;
}

void Client::setIsAbleToClose(bool isAbleToClose) {
    m_isAbleToClose = isAbleToClose;
}

ConfigManagerPtr Client::getConfigManager() const {
    return m_configManager;
}

void Client::setConfigManager(ConfigManagerPtr configManager) {
    m_configManager = configManager;
}

bool Client::getIsFirstAuthentication() const {
    return m_isFirstAuthentication;
}

void Client::setIsFirstAuthentication(bool isFirstAuthentication) {
    m_isFirstAuthentication = isFirstAuthentication;
}

bool Client::getIsPassedAuthentication() const {
    return m_isPassedAuthentication;
}

void Client::setIsPassedAuthentication(bool isPassedAuthentication) {
    m_isPassedAuthentication = isPassedAuthentication;
}
