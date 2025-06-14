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
#include "queryType.h"
#include "database.h"
#include "workerUI.h"
#include "utility.h"
#include "base64.h"
#include "fileWrapper.h"
#include "client.h"
#include "photo.h"
#include "chat.h"

Client::Client() :
    m_is_auto_login(false),
    m_is_ui_ready_to_update(false),
    m_is_has_photo(false),
    m_my_login(""),
    m_my_name(""),
    m_my_photo(nullptr),
    m_is_hidden(false),
    m_is_undo_auto_login(false),
    m_is_error(false)
{
    m_db = new Database;
    m_response_handler = new ResponseHandler(this);
    m_packets_builder = new PacketsBuilder();
}

Client::~Client() 
{
    for (auto& [login, chat] : m_map_friend_login_to_chat) {
        if (chat != nullptr) {
            delete chat;
            chat = nullptr;
        }
    }
    m_map_friend_login_to_chat.clear();

    delete m_response_handler;
    delete m_packets_builder;
    delete m_my_photo;
}

void Client::initDatabase(const std::string& login) {
    m_db->init(login);
}

void Client::run() { 
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
    sendPacket(m_packets_builder->getTypingPacket(m_my_login, friendLogin, isTyping), QueryType::TYPING);
}

void Client::authorizeClient(const std::string& login, const std::string& passwordHash) {
    m_my_password_hash = passwordHash;
    while (!is_messages_socket_validated) {
        if (m_is_error) {
            break;
        }
    }
    if (!m_is_error) {
        sendPacket(m_packets_builder->getAuthorizationPacket(login, passwordHash), QueryType::AUTHORIZATION);
    }
}

void Client::registerClient(const std::string& login, const std::string& passwordHash, const std::string& name) {
    m_my_login = login;
    m_my_password_hash = passwordHash;
    m_my_name = name;
    sendPacket(m_packets_builder->getRegistrationPacket(login, name, passwordHash), QueryType::REGISTRATION);
}

void Client::createChatWith(const std::string& friendLogin) {
    sendPacket(m_packets_builder->getCreateChatPacket(m_my_login, friendLogin), QueryType::CREATE_CHAT);
}

void Client::broadcastMyStatus(const std::string& status) {
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getStatusPacket(status, m_my_login, tmpFriendsLoginsVec), QueryType::STATUS);
}

void Client::sendMessage(const std::string& friendLogin, const Message* message) {
    sendPacket(m_packets_builder->getMessagePacket(m_my_login, friendLogin, message), QueryType::MESSAGE);
}

void Client::sendMessageReadConfirmation(const std::string& friendLogin, const Message* message) {
    sendPacket(m_packets_builder->getMessageReadConfirmationPacket(m_my_login, friendLogin, message), QueryType::MESSAGES_READ_CONFIRMATION);
}

void Client::getAllFriendsStatuses() {
    sendPacket(m_packets_builder->getLoadAllFriendsStatusesPacket(getFriendsLoginsVecFromMap()), QueryType::LOAD_ALL_FRIENDS_STATUSES);
}

void Client::findUser(const std::string& text) {
    sendPacket(m_packets_builder->getFindUserPacket(m_my_login, text), QueryType::FIND_USER);
}

void Client::requestFriendInfoFromServer(const std::string& myLogin) {
    sendPacket(m_packets_builder->getLoadUserInfoPacket(myLogin), QueryType::LOAD_FRIEND_INFO);
}

void Client::verifyPassword(const std::string& passwordHash) {
    sendPacket(m_packets_builder->getVerifyPasswordPacket(m_my_login, passwordHash), QueryType::VERIFY_PASSWORD);
}


void Client::checkIsNewLoginAvailable(const std::string& newLogin) {
    if (m_my_login == newLogin) {
        WorkerUI* workerUI = m_response_handler->getWorkerUI();
        workerUI->onCheckNewLoginFail();
    }
    sendPacket(m_packets_builder->getCheckIsNewLoginAvailablePacket(newLogin), QueryType::CHECK_NEW_LOGIN);
}

void Client::updateMyLogin(const std::string& newLogin) {
    if (m_my_login == newLogin) {
        return;
    }

    updateConfigName(newLogin);

    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyLoginPacket(m_my_login, newLogin, tmpFriendsLoginsVec), QueryType::UPDATE_MY_LOGIN);

    m_my_login = newLogin;
}

void Client::updateMyName(const std::string& newName) {
    m_my_name = newName;
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyNamePacket(m_my_login,newName,tmpFriendsLoginsVec), QueryType::UPDATE_MY_NAME);
}

void Client::updateMyPassword(const std::string& newPasswordHash) {
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyPasswordPacket(m_my_login, newPasswordHash, tmpFriendsLoginsVec), QueryType::UPDATE_MY_PASSWORD);
}

void Client::updateMyPhoto(const Photo& newPhoto) {
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyPhotoPacket(m_my_login, newPhoto, tmpFriendsLoginsVec), QueryType::UPDATE_MY_PHOTO);
}


//essantial functions
void Client::setWorkerUI(WorkerUI* workerImpl) {
    m_response_handler->setWorkerUI(workerImpl);
}

const std::vector<std::string> Client::getFriendsLoginsVecFromMap() {
    std::vector<std::string> result;
    result.reserve(m_map_friend_login_to_chat.size());
    std::transform(m_map_friend_login_to_chat.begin(),
        m_map_friend_login_to_chat.end(),
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


// save & load
void Client::save() const {
    QString dir = QString::fromStdString(utility::getSaveDir());
    QDir saveDir(dir);

    if (!saveDir.exists()) {
        if (!saveDir.mkpath(".")) {
            qWarning() << "Failed to create directory:" << dir;
            return;
        }
    }

    bool hashExists = false;
    QStringList jsonFiles = saveDir.entryList(QStringList() << "*.json", QDir::Files);

    for (const QString& file : jsonFiles) {
        if (file == QString::fromStdString(m_my_login) + ".json") {
            continue;
        }

        QFileInfo fileInfo(saveDir.filePath(file));
        QFile f(fileInfo.filePath());

        if (f.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            f.close();

            if (doc.isObject() && doc.object().contains("my_password_hash")) {
                hashExists = true;
                break;
            }
        }
    }

    QJsonObject jsonObject;
    QJsonArray chatsArray;

    for (const auto& chatPair : m_map_friend_login_to_chat) {
        chatsArray.append(chatPair.second->serialize(m_my_login, *m_db));
    }
    jsonObject["chatsArray"] = chatsArray;

    jsonObject["my_login"] = QString::fromStdString(m_my_login);
    jsonObject["is_hidden"] = QString::fromStdString(m_is_hidden ? "1" : "0");
    jsonObject["my_name"] = QString::fromStdString(m_my_name);
    jsonObject["is_has_photo"] = m_is_has_photo;

    if (!hashExists && !m_is_undo_auto_login) {
        jsonObject["my_password_hash"] = QString::fromStdString(m_my_password_hash);
        qDebug() << "Saving password hash for user:" << QString::fromStdString(m_my_login);
    }
    else {
        qDebug() << "Password hash already exists in another file, skipping save";
    }

    if (m_is_has_photo && m_my_photo != nullptr) {
        jsonObject["my_photo"] = QString::fromStdString(m_my_photo->getPhotoPath());
    }

    QString fileName = QString::fromStdString(m_my_login) + ".json";
    QString fullPath = saveDir.filePath(fileName);

    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(jsonObject);
        file.write(saveDoc.toJson());
        file.close();
        qDebug() << "Successfully saved user data to:" << fullPath;
    }
    else {
        qWarning() << "Failed to open file for writing:" << fullPath;
    }
}

bool Client::load(const std::string& fileName) {
    QString dir = QString::fromStdString(utility::getSaveDir());
    QString fileNameFinal = QString::fromStdString(fileName);
    QDir saveDir(dir);
    QString fullPath = saveDir.filePath(fileNameFinal);

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the .json config file:" << QString::fromStdString(fileName);
        return false;
    }

    QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!loadDoc.isObject()) {
        qWarning() << "Invalid JSON in the file:" << QString::fromStdString(fileName);
        return false;
    }

    QJsonObject jsonObject = loadDoc.object();
    m_my_login = jsonObject["my_login"].toString().toStdString();
    m_is_hidden = jsonObject["is_hidden"].toString().toStdString() == "1";
    m_my_name = jsonObject["my_name"].toString().toStdString();
    m_is_has_photo = jsonObject["is_has_photo"].toBool();

    m_my_photo = nullptr; 
    if (m_is_has_photo && jsonObject.contains("my_photo")) {
        QString photoPath = jsonObject["my_photo"].toString();
        if (!photoPath.isEmpty()) {
            m_my_photo = new Photo(photoPath.toStdString());
        }
    }

    m_map_friend_login_to_chat.clear();
    if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].isArray()) {
        QJsonArray chatsArray = jsonObject["chatsArray"].toArray();
        for (const QJsonValue& value : chatsArray) {
            if (value.isObject()) {
                Chat* chat = Chat::deserialize(m_my_login, value.toObject(), *m_db);
                if (chat) {
                    m_map_friend_login_to_chat[chat->getFriendLogin()] = chat;
                }
            }
        }
    }

    return true;
}

void Client::waitUntilUIReadyToUpdate() {
    while (!m_is_ui_ready_to_update.load()) {
        std::this_thread::yield();
    }
}

void Client::deleteFriendMessagesInDatabase(const std::string& friendLogin) {
    m_db->deleteAllMessages(m_my_login, friendLogin);
}

void Client::deleteFriendFromChatsMap(const std::string& friendLogin) {
    auto it = m_map_friend_login_to_chat.find(friendLogin);
    if (it != m_map_friend_login_to_chat.end()) {
        delete it->second;
        m_map_friend_login_to_chat.erase(it);
    }
}


void Client::updateConfigName(const std::string& newLogin) {
    QString oldFileName = QString::fromStdString(utility::getSaveDir()) +
        QString::fromStdString("/" + m_my_login) + ".json";
    QFile oldFile(oldFileName);

    if (oldFile.exists()) {
        QString newFileName = QString::fromStdString(utility::getSaveDir()) +
            QString::fromStdString("/" + newLogin) + ".json";

        if (!oldFile.rename(newFileName)) {
            qWarning() << "Failed to rename config file from" << oldFileName << "to" << newFileName;
            return;
        }
    }
}

void Client::updateInConfigFriendLogin(const std::string& oldLogin, const std::string& newLogin) {
    QString dir = QString::fromStdString(utility::getSaveDir());
    QString fileName = QString::fromStdString("/" + m_my_login) + ".json";
    QString fullPath = dir + fileName;
    std::string STRDEBUG = fullPath.toStdString();

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file for reading:" << fullPath;
        return;
    }

    QJsonDocument configDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (configDoc.isNull()) {
        qWarning() << "Invalid JSON in config file:" << fullPath;
        return;
    }

    QJsonObject configObj = configDoc.object();

    if (!configObj.contains("chatsArray") || !configObj["chatsArray"].isArray()) {
        qWarning() << "No chats array found in config";
        return;
    }

    QJsonArray chatsArray = configObj["chatsArray"].toArray();
    bool found = false;

    for (auto&& chatValue : chatsArray) {
        if (!chatValue.isObject()) continue;

        QJsonObject chatObj = chatValue.toObject();
        QString currentLogin = chatObj["friend_login"].toString();

        if (currentLogin == QString::fromStdString(oldLogin)) {
            chatObj["friend_login"] = QString::fromStdString(newLogin);
            chatValue = chatObj;
            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "Friend login" << oldLogin.c_str() << "not found in config";
        return;
    }

    configObj["chatsArray"] = chatsArray;

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << fullPath;
        return;
    }

    file.write(QJsonDocument(configObj).toJson());
    file.close();
}

bool Client::autoLoginAndLoad() {
    QString dir = QString::fromStdString(utility::getSaveDir());
    QDir saveDir(dir);

    QStringList jsonFiles = saveDir.entryList(QStringList() << "*.json", QDir::Files);

    if (jsonFiles.isEmpty()) {
        qWarning() << "No JSON files found in directory:" << dir;
        return false;
    }

    for (const QString& jsonFile : jsonFiles) {
        QString fullPath = saveDir.filePath(jsonFile);
        QFile file(fullPath);

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Couldn't open JSON file:" << fullPath;
            continue;
        }

        QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (!loadDoc.isObject()) {
            qWarning() << "Invalid JSON in file:" << fullPath;
            continue;
        }

        QJsonObject jsonObject = loadDoc.object();

        if (jsonObject.contains("my_password_hash")) {
            m_my_login = jsonObject["my_login"].toString().toStdString();
            initDatabase(m_my_login);
            m_is_hidden = jsonObject["is_hidden"].toString().toStdString() == "1";

            m_my_name = jsonObject["my_name"].toString().toStdString();
            m_my_password_hash = jsonObject["my_password_hash"].toString().toStdString();
            m_is_has_photo = jsonObject["is_has_photo"].toBool();

            m_my_photo = nullptr;
            if (m_is_has_photo && jsonObject.contains("my_photo")) {
                QString photoPath = jsonObject["my_photo"].toString();
                if (!photoPath.isEmpty()) {
                    m_my_photo = new Photo(photoPath.toStdString());
                }
            }

            m_map_friend_login_to_chat.clear();
            if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].isArray()) {
                QJsonArray chatsArray = jsonObject["chatsArray"].toArray();
                for (const QJsonValue& value : chatsArray) {
                    if (value.isObject()) {
                        Chat* chat = Chat::deserialize(m_my_login, value.toObject(), *m_db);
                        if (chat) {
                            m_map_friend_login_to_chat[chat->getFriendLogin()] = chat;
                        }
                    }
                }
            }

            m_is_auto_login = true;

            qDebug() << "Auto-login configuration found in file:" << fullPath;

            return true;
        }
    }

    qWarning() << "No JSON file with passwordHash field found in directory:" << dir;
    return false;
}


void Client::deleteFriendChatInConfig(const std::string& friendLogin) {
    QString configPath = QString::fromStdString(utility::getSaveDir() + "/" + m_my_login + ".json");
    QFile file(configPath);

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning() << "Couldn't open config file for update:" << configPath;
        return;
    }

    QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
    if (!loadDoc.isObject()) {
        qWarning() << "Invalid JSON in config file:" << configPath;
        file.close();
        return;
    }

    QJsonObject jsonObject = loadDoc.object();

    if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].isArray()) {
        QJsonArray chatsArray = jsonObject["chatsArray"].toArray();
        QJsonArray newChatsArray;

        for (const QJsonValue& value : chatsArray) {
            if (value.isObject()) {
                QJsonObject chatObj = value.toObject();
                if (chatObj.contains("friend_login") &&
                    chatObj["friend_login"].toString().toStdString() != friendLogin) {
                    newChatsArray.append(chatObj);
                }
            }
        }

        if (newChatsArray.size() != chatsArray.size()) {
            jsonObject["chatsArray"] = newChatsArray;

            file.resize(0);
            file.write(QJsonDocument(jsonObject).toJson());
            qDebug() << "Chat with friend" << QString::fromStdString(friendLogin)
                << "removed from config";
        }
    }

    file.close();
}

bool Client::undoAutoLogin() {
    QString oldFileName = QString::fromStdString(utility::getSaveDir()) +
        QString::fromStdString("/" + m_my_login) + ".json";

    QFile file(oldFileName);

    if (!file.exists()) {
        qWarning() << "Auto-login file not found:" << oldFileName;
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file for reading:" << oldFileName;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON in file:" << oldFileName;
        return false;
    }

    QJsonObject jsonObj = doc.object();

    if (!jsonObj.contains("my_password_hash")) {
        qDebug() << "Password hash field not found in file:" << oldFileName;
        return true;
    }

    jsonObj.remove("my_password_hash");

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open file for writing:" << oldFileName;
        return false;
    }

    QJsonDocument newDoc(jsonObj);
    if (file.write(newDoc.toJson()) == -1) {
        qWarning() << "Failed to write to file:" << oldFileName;
        file.close();
        return false;
    }

    file.close();
    qDebug() << "Successfully removed password hash from:" << oldFileName;
    return true;
}


// new 
void Client::onMessage(net::message<QueryType> message) {
    m_response_handler->handleResponse(message);
}

void Client::onFile(net::file<QueryType> file) {
    m_response_handler->handleFile(file);
}

void Client::onFileSent(net::file<QueryType> sentFile) {
    //TODO
}

void Client::sendFilesMessage(Message& filesMessage) {
    const auto& relatedFiles = filesMessage.getRelatedFiles();
    for (auto& wrapper : relatedFiles) {
        sendFile(wrapper.file);
    }
}

void Client::requestFile(const fileWrapper& fileWrapper) {
    std::string packetStr = m_packets_builder->getSendMeFilePacket(m_my_login, fileWrapper.file.receiverLogin, fileWrapper.file.fileName, fileWrapper.file.id, std::to_string(fileWrapper.file.fileSize), fileWrapper.file.timestamp, fileWrapper.file.caption, fileWrapper.file.blobUID, fileWrapper.file.filesInBlobCount);
    sendPacket(packetStr, QueryType::SEND_ME_FILE);
}

// errors
void Client::onSendMessageError(std::error_code ec, net::message<QueryType> unsentMessage) {
    std::string messageStr;
    unsentMessage >> messageStr;
    std::istringstream iss(messageStr);

    QueryType type = unsentMessage.header.type;

    if (type == QueryType::MESSAGE) {
        std::string friendLogin;
        std::getline(iss, friendLogin);

        std::string myLogin;
        std::getline(iss, myLogin);

        std::string messageBegin;
        std::getline(iss, messageBegin);

        std::string message;
        std::string line;
        while (std::getline(iss, line)) {
            if (line == "MESSAGE_END") {
                break;
            }
            else {
                message += line;
                message += '\n';
            }
        }
        message.pop_back();

        std::string id;
        std::getline(iss, id);

        std::string timestamp;
        std::getline(iss, timestamp);

        auto chatPair = m_map_friend_login_to_chat.find(friendLogin);
        if (chatPair != m_map_friend_login_to_chat.end()) {
            Chat* chat = chatPair->second;

            auto& messagesVec = chat->getMessagesVec();
            auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&id](Message* msg) {
                return msg->getId() == id;
            });

            Message* msg = *msgChatIt;
            msg->setIsNeedToRetry(true);
            m_response_handler->getWorkerUI()->onMessageSendingError(friendLogin, msg);
        }
    }
    else if (type == QueryType::MESSAGES_READ_CONFIRMATION) {
        std::string friendLogin;
        std::getline(iss, friendLogin);

        std::string myLogin;
        std::getline(iss, myLogin);

        std::string id;
        std::getline(iss, id);

        auto chatPair = m_map_friend_login_to_chat.find(friendLogin);

        if (chatPair != m_map_friend_login_to_chat.end()) {
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
        std::string myLogin;
        std::getline(iss, myLogin);

        std::string friendLogin;
        std::getline(iss, friendLogin);

        std::string fileName;
        std::getline(iss, fileName);

        std::string fileId;
        std::getline(iss, fileId);

        std::string fileTimestamp;
        std::getline(iss, fileTimestamp);

        std::string fileSize;
        std::getline(iss, fileSize);

        std::string messageBegin;
        std::getline(iss, messageBegin);

        std::string caption;
        std::string line;
        while (std::getline(iss, line)) {
            if (line == "MESSAGE_END") {
                break;
            }
            else {
                caption += line;
                caption += '\n';
            }
        }
        caption.pop_back();

        std::string filesCountInBlobStr;
        std::getline(iss, filesCountInBlobStr);
        size_t filesCountInBlob = std::stoi(filesCountInBlobStr);

        std::string blobUID;
        std::getline(iss, blobUID);

        net::file<QueryType> file;
        file.blobUID = blobUID;
        file.filesInBlobCount = filesCountInBlob;
        file.id = fileId;

        m_response_handler->getWorkerUI()->onRequestedFileError(friendLogin, { false, file });
    }

    if (type != QueryType::AUTHORIZATION) {
        if (!utility::isHasInternetConnection()) {
            m_response_handler->getWorkerUI()->onNetworkError();
        }
    }
}

void Client::onSendFileError(std::error_code ec, net::file<QueryType> unsentFille) {
        auto itChat = m_map_friend_login_to_chat.find(unsentFille.receiverLogin);
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
    if (unreadFile.senderLogin == "" && unreadFile.receiverLogin == "") {
        onServerDown();
        return;
    }

    bool isRequested = unreadFile.isRequested;
    if (isRequested) {
        m_response_handler->getWorkerUI()->onRequestedFileError(unreadFile.receiverLogin, { false, unreadFile });
    }
    else {
        auto it = m_map_message_blobs.find(unreadFile.receiverLogin);
        auto [blobUID, message] = *it;
        for (auto& file : message->getRelatedFiles()) {
            std::string path = file.file.filePath;
            if (path.empty()) {
            }

            std::error_code ec;
            bool removed = std::filesystem::remove(path, ec);

            if (ec) {
                std::cerr << "Failed to delete (onReadFileError)" << path << ": " << ec.message() << "\n";
            }
        }

        delete message;
        m_map_message_blobs.erase(unreadFile.receiverLogin);
    }
}

void Client::onConnectError(std::error_code ec) {
    m_response_handler->getWorkerUI()->onConnectError();
    m_is_error = true;
}

void Client::onNetworkError() {
    if (!isStopped()) {
    }
    m_response_handler->getWorkerUI()->onNetworkError();
}

void Client::onServerDown() {
    m_response_handler->getWorkerUI()->onServerDown();
}

void  Client::attemptReconnect() {
    connectMessagesSocket(m_server_ipAddress, m_server_port);
    runContextThread();
    connectFilesSocket(m_my_login, m_server_ipAddress, m_server_port);

}

void Client::onSendFileProgressUpdate(const net::file<QueryType>& file, uint32_t progressPercent) {
    waitUntilUIReadyToUpdate();
    m_response_handler->getWorkerUI()->updateFileSendingProgress(file.receiverLogin, file, progressPercent);
}

void Client::onReceiveFileProgressUpdate(const net::file<QueryType>& file, uint32_t progressPercent) {
    waitUntilUIReadyToUpdate();
    m_response_handler->getWorkerUI()->updateFileLoadingProgress(file.senderLogin, file, progressPercent);
}