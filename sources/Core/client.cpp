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
#include "client.h"
#include "photo.h"
#include "chat.h"

void Client::processIncomingMessagesQueue() {
    net::safe_deque<net::owned_message<QueryType>>& queue = getMessagesQueue();
    while (true) {
        if (!queue.empty()) {
            net::owned_message<QueryType> msg = queue.pop_front();
            std::string messageStr = "";
            msg.msg >> messageStr;
            m_response_handler->handleResponse(messageStr);
        }
    }
}

Client::Client() :
    m_is_need_to_save_config(false),
    m_is_ui_ready_to_update(false),
    m_is_has_photo(false),
    m_my_login(""),
    m_my_name(""),            
    m_my_photo(nullptr)
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


void Client::run() { 
    m_db->init();
    m_workerThread = std::thread([this]() { processIncomingMessagesQueue(); });
}

void Client::connectTo(const std::string& ipAddress, int port) {
    connect(ipAddress, port);
}

void Client::stop() {
    disconnect();
}

void Client::authorizeClient(const std::string& login, const std::string& passwordHash) {
    sendPacket(m_packets_builder->getAuthorizationPacket(login, passwordHash), QueryType::AUTHORIZATION);
}

void Client::registerClient(const std::string& login, const std::string& passwordHash, const std::string& name) {
    m_my_login = login;
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

void Client::requestFriendInfoFromServer(const std::string& myLogin) {
    sendPacket(m_packets_builder->getLoadUserInfoPacket(myLogin), QueryType::LOAD_FRIEND_INFO);
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
    QJsonObject jsonObject;
    QJsonArray chatsArray;

    for (const auto& chatPair : m_map_friend_login_to_chat) {
        chatsArray.append(chatPair.second->serialize(*m_db));
    }
    jsonObject["chatsArray"] = chatsArray;

    jsonObject["my_login"] = QString::fromStdString(m_my_login);
    jsonObject["my_name"] = QString::fromStdString(m_my_name);
    jsonObject["is_has_photo"] = m_is_has_photo;

    if (m_is_has_photo && m_my_photo != nullptr) {
        jsonObject["my_photo"] = QString::fromStdString(m_my_photo->getPhotoPath());
    }

    QString dir = QString::fromStdString(utility::getSaveDir());
    QString fileName = QString::fromStdString(m_my_login) + ".json";

    QDir saveDir(dir);
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(".")) {
            qWarning() << "Failed to create directory:" << dir;
            return;
        }
    }

    QString fullPath = saveDir.filePath(fileName);
    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(jsonObject);
        file.write(saveDoc.toJson());
        file.close();
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
                Chat* chat = Chat::deserialize(value.toObject(), *m_db);
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