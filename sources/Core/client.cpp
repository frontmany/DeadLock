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
#include "database.h"
#include "workerUI.h"
#include "utility.h"
#include "base64.h"
#include "client.h"
#include "photo.h"
#include "chat.h"

void Client::processQueue() {
    while (true) {
        if (!m_packets_queue.empty()) {

            m_queue_mutex.lock();
            std::string packet = m_packets_queue.front();
            m_packets_queue.pop();
            m_queue_mutex.unlock();

            m_response_handler->handleResponse(packet);
        }
    }
}

void Client::processToSendQueue() {
    while (m_packets_to_send_queue.size() != 0) {
        std::string packet = m_packets_to_send_queue.front();
        m_packets_to_send_queue.pop();

        sendPacket(packet);
    }
}

Client::Client() :
    m_is_need_to_save_config(false),
    m_is_ui_ready_to_update(false),
    m_is_has_photo(false),
    m_my_login(""),
    m_my_name(""),
    m_socket(m_io_context),                 
    m_my_photo(nullptr)
{
    m_db = new Database;
    m_response_handler = new ResponseHandler(this);
    m_packets_builder = new PacketsBuilder();
    m_buffer = std::make_shared<asio::streambuf>();
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
    startAsyncReceive();
    m_workerThread = std::thread([this]() { processQueue(); });
    m_io_contextThread = std::thread([this]() { m_io_context.run(); });
}

void Client::connectTo(const std::string& ipAddress, int port) {
    try {
        m_endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(ipAddress), port);
        m_socket.connect(m_endpoint);
        std::cout << "Connection established successfully!" << std::endl;
    }
    catch (const asio::system_error& e) {
        std::cerr << "Connection failed: " << e.what()
            << " (code: " << e.code() << ")" << std::endl;
        throw;
    }
}

void Client::startAsyncReceive() {
    m_buffer->prepare(1024 * 1024); 
    processToSendQueue();
    asio::async_read_until(m_socket, *m_buffer, m_packets_builder->getEndPacketString(),
        [this](const std::error_code& ec, std::size_t bytes_transferred) {
            handleAsyncReceive(ec, bytes_transferred);
        });
}

void Client::handleAsyncReceive(const std::error_code& ec, std::size_t bytes_transferred) {
    if (ec) {
        std::cerr << "Error during receive: " << ec.message() << std::endl;

        return;
    }

    std::istream is(m_buffer.get());
    std::string message;
    std::string buffer_content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    if (!buffer_content.empty()) {
        std::cout << "Received complete message: " << buffer_content << std::endl;
        m_queue_mutex.lock();
        m_packets_queue.push(buffer_content);
        m_queue_mutex.unlock();
    }

    m_buffer->consume(bytes_transferred);

    startAsyncReceive();
}

void Client::stop() {
    m_socket.close();
    m_io_context.stop();
    if (m_io_contextThread.joinable()) {
        m_io_contextThread.join();
    }
}

void Client::authorizeClient(const std::string& login, const std::string& passwordHash) {
    sendPacket(m_packets_builder->getAuthorizationPacket(login, passwordHash));
}

void Client::registerClient(const std::string& login, const std::string& passwordHash, const std::string& name) {
    m_my_login = login;
    m_my_name = name;
    sendPacket(m_packets_builder->getRegistrationPacket(login, name, passwordHash));
}

void Client::createChatWith(const std::string& friendLogin) {
    sendPacket(m_packets_builder->getCreateChatPacket(m_my_login, friendLogin));
}

void Client::broadcastMyStatus(const std::string& status) {
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    m_packets_to_send_queue.push(m_packets_builder->getStatusPacket(status, m_my_login, tmpFriendsLoginsVec));
}

void Client::sendMessage(const std::string& friendLogin, const Message* message) {
    sendPacket(m_packets_builder->getMessagePacket(m_my_login, friendLogin, message));
}

void Client::sendMessageReadConfirmation(const std::string& friendLogin, const Message* message) {
    sendPacket(m_packets_builder->getMessageReadConfirmationPacket(m_my_login, friendLogin, message));
}

void Client::getAllFriendsStatuses() {
    m_packets_to_send_queue.push(m_packets_builder->getLoadAllFriendsStatusesPacket(getFriendsLoginsVecFromMap()));
}

void Client::requestUserInfoFromServer(const std::string& myLogin) {
    m_packets_to_send_queue.push(m_packets_builder->getLoadUserInfoPacket(myLogin));
}


void Client::updateMyName(const std::string& newName) {
    m_my_name = newName;
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyNamePacket(m_my_login,newName,tmpFriendsLoginsVec));
}

void Client::updateMyPassword(const std::string& newPasswordHash) {
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyPasswordPacket(m_my_login, newPasswordHash, tmpFriendsLoginsVec));
}

void Client::updateMyPhoto(const Photo& newPhoto) {
    const std::vector<std::string>& tmpFriendsLoginsVec = getFriendsLoginsVecFromMap();
    sendPacket(m_packets_builder->getUpdateMyPhotoPacket(m_my_login, newPhoto, tmpFriendsLoginsVec));
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

OperationResult Client::sendPacket(const std::string& packet) {
    try {
        size_t bytes_sent = asio::write(m_socket,
            asio::buffer(packet),
            asio::transfer_all());

        if (bytes_sent != packet.size()) {
            std::cerr << "Warning: Not all bytes were sent ("
                << bytes_sent << "/" << packet.size() << ")\n";
            return OperationResult::FAIL; 
        }

        return OperationResult::SUCCESS;
    }

    catch (const std::exception& e) {
        std::cerr << "Error sending packet: " << e.what() << std::endl;
        return OperationResult::FAIL;
    }
    catch (...) {
        std::cerr << "Unknown error while sending packet" << std::endl;
        return OperationResult::FAIL;
    }
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