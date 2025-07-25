#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <codecvt>
#include <locale>
#include <QFile>
#include <QDir>

#include "operationResult.h"
#include "responseHandler.h"
#include "packetsBuilder.h"
#include "configManager.h"
#include "fileWrapper.h"
#include "base64_my.h"
#include "queryType.h"
#include "database.h"
#include "workerUI.h"
#include "utility.h"
#include "client.h"
#include "photo.h"
#include "chat.h"

Client::Client() :
    m_is_error(false),
    m_is_logged_in(false),
    m_is_hidden(false),
    m_is_able_to_close(true),
    m_config_manager(nullptr)
{
    m_config_manager = std::make_shared<ConfigManager>();
    m_config_manager->setClient(this);

    m_db = new Database;
    m_response_handler = new ResponseHandler(this, m_config_manager);
    m_packets_builder = new PacketsBuilder();
}

Client::~Client() 
{
    for (auto& [loginHash, chat] : m_map_friend_loginHash_to_chat) {
        if (chat != nullptr) {
            delete chat;
            chat = nullptr;
        }
    }
    m_map_friend_loginHash_to_chat.clear();

    delete m_response_handler;
    delete m_packets_builder;
}

bool Client::waitForConnectionWithTimeout(int timeoutMs) {
    auto startTime = std::chrono::steady_clock::now();
    while (!isConnected()) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - startTime).count() > timeoutMs) {
            return false; 
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return true; 
}

void Client::initDatabase(const std::string& loginHash) {
    m_db->init(loginHash);
}

void Client::startProcessingIncomingPackets() { 
    m_worker_thread = std::thread([this]() { update(); });
}

void Client::connectTo(const std::string& ipAddress, int port) {
    m_server_ipAddress = ipAddress;
    m_server_port = port;
    connectMessagesSocket(ipAddress, port);
    runContextThread();
}

void Client::stop() {
    disconnect();
}

void Client::typingNotify(const std::string& friendLogin, bool isTyping) {
    auto it = m_map_friend_loginHash_to_chat.find(utility::calculateHash(friendLogin));
    auto& [hash, chat] = *it;
    sendPacket(m_packets_builder->getTypingPacket(chat->getPublicKey(), m_config_manager->getMyLoginHash(), utility::calculateHash(friendLogin), isTyping), QueryType::TYPING);
}

void Client::authorizeClient(const std::string& loginHash, const std::string& passwordHash) {
    while (!is_messages_socket_validated) {
        if (m_is_error) {
            break;
        }
    }
    if (!m_is_error) {
        m_config_manager->setMyLoginHash(loginHash);
        m_config_manager->setMyPasswordHash(passwordHash);
        sendPacket(m_packets_builder->getAuthorizationPacket(loginHash, passwordHash), QueryType::AUTHORIZATION);
    }
}

void Client::registerClient(const std::string& login, const std::string& password, const std::string& name) {
    // if registration will fail fields will be set empty in "onRegistrationFail" function
    m_config_manager->setMyLogin(login);
    m_config_manager->setMyLoginHash(utility::calculateHash(login));
    m_config_manager->setMyName(name);
    m_config_manager->setMyPasswordHash(utility::calculateHash(password));

    sendPacket(m_packets_builder->getRegistrationPacket(
        m_config_manager->getMyLoginHash(),
        m_config_manager->getMyPasswordHash()), QueryType::REGISTRATION);
}

void Client::generateMyKeyPair() {
    if (m_my_public_key.GetModulus().IsZero() || m_my_private_key.GetModulus().IsZero()) {
        CryptoPP::RSA::PublicKey publicKey;
        CryptoPP::RSA::PrivateKey privateKey;
        utility::generateRSAKeyPair(privateKey, publicKey);

        setPublicKey(publicKey);
        setPrivateKey(privateKey);
    }
}

void Client::afterRegistrationSendMyInfo() {
    sendPacket(m_packets_builder->getAfterRegistrationSendMyInfoPacket(m_server_public_key, m_config_manager->getMyLogin(), m_config_manager->getMyName()), QueryType::AFTER_RREGISTRATION_SEND_MY_INFO);
}

void Client::createChatWith(const std::string& friendLogin) {
    sendPacket(m_packets_builder->getCreateChatPacket(m_config_manager->getMyLoginHash(), utility::calculateHash(friendLogin)), QueryType::CREATE_CHAT);
}

void Client::broadcastMyStatus(const std::string& status) {
    const std::vector<std::string>& tmpFriendsLoginHashesVec = getFriendsLoginHashesVecFromMap();
    if (utility::validatePublicKey(m_server_public_key)) {
        sendPacket(m_packets_builder->getStatusPacket(m_server_public_key, status, m_config_manager->getMyLoginHash(), tmpFriendsLoginHashesVec), QueryType::STATUS);
    }
}

void Client::sendMessage(const CryptoPP::RSA::PublicKey& friendPublicKey, const std::string& friendLogin, const Message* message) {
    sendPacket(m_packets_builder->getMessagePacket(friendPublicKey, m_config_manager->getMyLoginHash(), utility::calculateHash(friendLogin), message), QueryType::MESSAGE);
}

void Client::sendMessageReadConfirmation(const std::string& friendLogin, const Message* message) {
    auto it = m_map_friend_loginHash_to_chat.find(utility::calculateHash(friendLogin));
    if (it != m_map_friend_loginHash_to_chat.end()) {
        sendPacket(m_packets_builder->getMessageReadConfirmationPacket(it->second->getPublicKey(), m_config_manager->getMyLoginHash(), utility::calculateHash(friendLogin), message), QueryType::MESSAGES_READ_CONFIRMATION);
    }
}

void Client::getAllFriendsStatuses() {
    sendPacket(m_packets_builder->getLoadAllFriendsStatusesPacket(m_config_manager->getMyLoginHash(), getFriendsLoginHashesVecFromMap()), QueryType::LOAD_ALL_FRIENDS_STATUSES);
}

void Client::findUser(const std::string& searchText) {
    sendPacket(m_packets_builder->getFindUserPacket(m_server_public_key, m_config_manager->getMyLoginHash(), searchText), QueryType::FIND_USER);
}

void Client::requestUserInfoFromServer(const std::string& loginHashToSearch, const std::string& loginHash) {
    sendPacket(m_packets_builder->getLoadUserInfoPacket(loginHashToSearch, loginHash), QueryType::LOAD_USER_INFO);
}

void Client::requestMyInfoFromServerAndResetKeys(const std::string& loginHash) {
    CryptoPP::RSA::PrivateKey newPrivateKey;
    CryptoPP::RSA::PublicKey newPublicKey;
    utility::generateRSAKeyPair(newPrivateKey, newPublicKey);

    setPublicKey(newPublicKey);
    setPrivateKey(newPrivateKey);

    sendPacket(m_packets_builder->getLoadMyInfoPacket(loginHash, newPublicKey), QueryType::LOAD_MY_INFO);
}

void Client::requestUpdate() {
    sendPacket(m_packets_builder->getUpdateRequestPacket(m_server_public_key, m_config_manager->getMyLoginHash(), m_config_manager->getNewVersionNumber()), QueryType::UPDATE_REQUEST);
}

void Client::verifyPassword(const std::string& passwordHash) {
    sendPacket(m_packets_builder->getVerifyPasswordPacket(m_config_manager->getMyLoginHash(), passwordHash), QueryType::VERIFY_PASSWORD);
}


void Client::checkIsNewLoginAvailable(const std::string& newLogin) {
    if (m_config_manager->getMyLogin() == newLogin) {
        WorkerUI* workerUI = m_response_handler->getWorkerUI();
        workerUI->onCheckNewLoginFail();
    }
    sendPacket(m_packets_builder->getCheckIsNewLoginAvailablePacket(m_server_public_key, m_config_manager->getMyLoginHash(), newLogin), QueryType::CHECK_NEW_LOGIN);
}

void Client::updateMyLogin(const std::string& newLogin) {
    if (m_config_manager->getMyLogin() == newLogin) {
        return;
    }

    std::string oldLoginHash = m_config_manager->getMyLoginHash();
    std::string newLoginHash = utility::calculateHash(newLogin);

    m_config_manager->setMyLogin(newLogin);
    m_config_manager->setMyLoginHash(newLoginHash);
    m_config_manager->updateConfigFileName(oldLoginHash, newLoginHash);

    m_db->updateTableName(oldLoginHash, newLoginHash);

    const std::vector<std::string>& tmpFriendsLoginHashesVec = getFriendsLoginHashesVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyLoginPacket(m_server_public_key, oldLoginHash, newLoginHash, newLogin, tmpFriendsLoginHashesVec), QueryType::UPDATE_MY_LOGIN);
}

void Client::updateMyName(const std::string& newName) {
    m_config_manager->setMyName(newName);
    const std::vector<std::string>& tmpFriendsLoginHashesVec = getFriendsLoginHashesVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyNamePacket(m_server_public_key, m_config_manager->getMyLoginHash(), newName, tmpFriendsLoginHashesVec), QueryType::UPDATE_MY_NAME);
}

void Client::updateMyPassword(const std::string& newPasswordHash) {
    m_config_manager->setMyPasswordHash(newPasswordHash);
    sendPacket(m_packets_builder->getUpdateMyPasswordPacket(m_config_manager->getMyLoginHash(), newPasswordHash), QueryType::UPDATE_MY_PASSWORD);
}

void Client::updateMyPhoto(const Photo& newPhoto) {
    const std::vector<std::string>& tmpFriendsLoginHashesVec = getFriendsLoginHashesVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyPhotoPacket(m_server_public_key, m_config_manager->getMyLoginHash(), newPhoto, tmpFriendsLoginHashesVec), QueryType::UPDATE_MY_PHOTO);
}


//essantial functions
void Client::skipLines(std::istream& iss, int count) {
    std::string dummy;
    for (int i = 0; i < count; ++i) {
        if (!std::getline(iss, dummy)) {
            break;
        }
    }
}

std::optional<Chat*> Client::findChat(const std::string& loginHash) const {
    auto it = m_map_friend_loginHash_to_chat.find(loginHash);

    if (it != m_map_friend_loginHash_to_chat.end()) {
        auto& [loginHash, chat] = *it;
        return chat;
    }
    else {
        return std::nullopt;
        std::cout << "\"updateInConfigFriendLogin\" cannot find friend loginHash: " << loginHash << std::endl;
    }
}

void Client::setWorkerUI(WorkerUI* workerImpl) {
    m_response_handler->setWorkerUI(workerImpl);
}

const std::vector<std::string> Client::getFriendsLoginHashesVecFromMap() {
    std::vector<std::string> result;
    result.reserve(m_map_friend_loginHash_to_chat.size());
    std::transform(m_map_friend_loginHash_to_chat.begin(),
        m_map_friend_loginHash_to_chat.end(),
        std::back_inserter(result),
        [](const auto& pair) { return pair.first; });

    return result;
}

void Client::sendPacket(const std::string& packet, QueryType type) {
    net::message<QueryType> msg;
    msg.header.type = type;
    msg << packet;
    send(msg);
}

void Client::waitUntilUIReadyToUpdate() {
    while (!m_is_ui_ready_to_update.load()) {
        std::this_thread::yield();
    }
}

void Client::deleteFriendMessagesInDatabase(const std::string& friendLogin) {
    m_db->deleteAllMessages(m_config_manager->getMyLogin(), friendLogin);
}

void Client::deleteFriendFromChatsMap(const std::string& friendLoginHash) {
    auto it = m_map_friend_loginHash_to_chat.find(friendLoginHash);
    if (it != m_map_friend_loginHash_to_chat.end()) {
        delete it->second;
        m_map_friend_loginHash_to_chat.erase(it);
    }
}

void Client::setServerEncryptionPart(const std::string& encryptionPart) {
    m_server_encryption_part = encryptionPart;
}

std::string Client::getSpecialServerKey() const {
    return m_config_manager->getMyPasswordHash() + m_server_encryption_part;
}

void Client::onMessage(net::message<QueryType> message) {
    m_is_able_to_close = false;
    m_response_handler->handleResponse(message);
    m_is_able_to_close = true;
}

void Client::onFile(net::file<QueryType> file) {
    m_is_able_to_close = false;
    m_response_handler->onFile(file);
    m_is_able_to_close = true;
}

void Client::sendFilesMessage(Message& filesMessage) {
    auto workerUI = m_response_handler->getWorkerUI();
    workerUI->blockProfileEditing();

    auto& relatedFiles = filesMessage.getRelatedFiles();
    for (auto& wrapper : relatedFiles) {
        auto chat = m_map_friend_loginHash_to_chat[wrapper.file.receiverLoginHash];
        wrapper.file.friendPublicKey = chat->getPublicKey();
        sendFile(wrapper.file);
    }
}

void Client::onAllFilesSent() {
    auto workerUI = m_response_handler->getWorkerUI();
    workerUI->activateProfileEditing();
}

void Client::retrySendFilesMessage(Message& filesMessage) {
    auto workerUI = m_response_handler->getWorkerUI();
    workerUI->blockProfileEditing();

    auto& relatedFiles = filesMessage.getRelatedFiles();
    for (auto& wrapper : relatedFiles) {
        if (wrapper.isNeedToRetry) {
            auto chat = m_map_friend_loginHash_to_chat[wrapper.file.receiverLoginHash];
            wrapper.file.friendPublicKey = chat->getPublicKey();
            sendFile(wrapper.file);
        }
    }
}

void Client::requestFile(const fileWrapper& fileWrapper) {
    std::string packetStr = m_packets_builder->getSendMeFilePacket(m_my_private_key, fileWrapper.file.encryptedKey, m_config_manager->getMyLoginHash(), fileWrapper.file.senderLoginHash, fileWrapper.file.fileName, fileWrapper.file.id, fileWrapper.file.fileSize, fileWrapper.file.timestamp, fileWrapper.file.caption, fileWrapper.file.blobUID, fileWrapper.file.filesInBlobCount);
    m_db->addRequestedFile(m_config_manager->getMyLoginHash(), fileWrapper.file.id);
    sendPacket(packetStr, QueryType::SEND_ME_FILE);
}




void Client::onSendMessageError(std::error_code ec, net::message<QueryType> unsentMessage) {
    std::string messageStr;
    unsentMessage >> messageStr;
    std::istringstream iss(messageStr);

    QueryType type = unsentMessage.header.type;

    if (type == QueryType::MESSAGE) {
        skipLines(iss, 1);

        std::string friendLoginHash;
        std::getline(iss, friendLoginHash);

        std::string id;
        std::getline(iss, id);

        auto chatPair = m_map_friend_loginHash_to_chat.find(friendLoginHash);
        if (chatPair != m_map_friend_loginHash_to_chat.end()) {
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
    else if (type == QueryType::MESSAGES_READ_CONFIRMATION) {
        skipLines(iss, 1);

        std::string friendLoginHash;
        std::getline(iss, friendLoginHash);

        skipLines(iss, 2);

        std::string id;
        std::getline(iss, id);

        auto chatPair = m_map_friend_loginHash_to_chat.find(friendLoginHash);

        if (chatPair != m_map_friend_loginHash_to_chat.end()) {
            Chat* chat = chatPair->second;

            auto& messagesVec = chat->getMessagesVec();
            auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&id](Message* msg) {
                return msg->getId() == id;
                });
            Message* msg = *msgChatIt;
            msg->setIsRead(false);
        }
    }
    else if (type == QueryType::SEND_ME_FILE) {
        skipLines(iss, 2);
        
        std::string myLoginHash;
        std::getline(iss, myLoginHash);

        std::string fileId;
        std::getline(iss, fileId);

        std::string blobUID;
        std::getline(iss, blobUID);

        std::string friendLoginHash;
        std::getline(iss, friendLoginHash);

        net::file<QueryType> file;
        file.blobUID = blobUID;
        file.filesInBlobCount = -1;
        file.id = fileId;

        m_response_handler->getWorkerUI()->onRequestedFileError(friendLoginHash, { false, file });
    }

    if (type != QueryType::AUTHORIZATION) {
        if (!utility::isHasInternetConnection()) {
            m_response_handler->getWorkerUI()->onNetworkError();
        }
    }
}

void Client::onSendFileError(std::error_code ec, net::file<QueryType> unsentFille) {
        auto itChat = m_map_friend_loginHash_to_chat.find(unsentFille.receiverLoginHash);
        auto& [friendLogin, chat] = *itChat;
        auto& messagesVec = chat->getMessagesVec();
        auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&unsentFille](Message* msg) {
            return msg->getId() == unsentFille.blobUID;
        });

        Message* msg = *msgChatIt;
        msg->setIsNeedToRetry(true);
        m_response_handler->getWorkerUI()->onMessageSendingError(friendLogin, msg);
}

void Client::onReceiveMessageError(std::error_code ec) {
    if (!utility::isHasInternetConnection()) {
        onNetworkError();
    }
    else {
        onServerDown();
    }
}

void Client::onReceiveFileError(std::error_code ec, net::file<QueryType> unreadFile) {
    if (!utility::isHasInternetConnection()) {
        onNetworkError();
        return;
    }
    if (unreadFile.senderLoginHash == "" && unreadFile.receiverLoginHash == "") {
        onServerDown();
        return;
    }

    if (auto vec = m_db->getRequestedFiles(m_config_manager->getMyLoginHash()); std::find(vec.begin(), vec.end(), unreadFile.id) != vec.end()) {
        m_response_handler->getWorkerUI()->onRequestedFileError(unreadFile.receiverLoginHash, { false, unreadFile });
    }
}

void Client::onConnectError(std::error_code ec) {
    m_response_handler->getWorkerUI()->onConnectError();
    m_is_error = true;
}

void Client::onNetworkError() {
    if (!isStopped()) {
        stop();
    }
    m_response_handler->getWorkerUI()->onNetworkError();
}

void Client::onServerDown() {
    stop();
    m_response_handler->getWorkerUI()->onServerDown();
}

void Client::onSendFileProgressUpdate(const net::file<QueryType>& file, uint32_t progressPercent) {
    waitUntilUIReadyToUpdate();
    m_response_handler->getWorkerUI()->updateFileSendingProgress(file.receiverLoginHash, file, progressPercent);
}

void Client::onReceiveFileProgressUpdate(const net::file<QueryType>& file, uint32_t progressPercent) {
    waitUntilUIReadyToUpdate();

    m_response_handler->getWorkerUI()->updateFileLoadingProgress(file.senderLoginHash, file, progressPercent);
}


void Client::sendPublicKeyToServer() {
    std::string keyStr = m_packets_builder->getPublicKeyPacket(m_config_manager->getMyLoginHash(), m_my_public_key);
    sendPacket(keyStr, QueryType::PUBLIC_KEY);
}

const CryptoPP::RSA::PublicKey& Client::getPublicKey() const {
    if (!utility::validatePublicKey(m_my_public_key)) {
        assert("Public key is not initialized or invalid");
    }

    return m_my_public_key;
}

const CryptoPP::RSA::PrivateKey& Client::getPrivateKey() const {
    if (!utility::validatePrivateKey(m_my_private_key)) {
        assert("Private key is not initialized or invalid");
    }

    return m_my_private_key;
}

void Client::setPublicKey(const CryptoPP::RSA::PublicKey& key) {
    if (!utility::validatePublicKey(key)) {
        assert("Invalid public key provided");
    }

    m_my_public_key = key;
}

void Client::setPrivateKey(const CryptoPP::RSA::PrivateKey& key) {
    if (!utility::validatePrivateKey(key)) {
        assert("Invalid private key provided");
    }

    m_my_private_key = key;
}
