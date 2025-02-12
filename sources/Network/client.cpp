#include "Client.h"
#include "utility.h"
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>

Client::Client() :  m_isReceiving(true),
sh_is_authorized(OperationResult::NOT_STATED),
sh_is_message_send(OperationResult::NOT_STATED),
sh_is_info_updated(OperationResult::NOT_STATED),
sh_is_message_read_confirmation_send(OperationResult::NOT_STATED),
sh_chat_create(OperationResult::NOT_STATED),
sh_is_status_send(OperationResult::NOT_STATED),
m_my_photo(Photo()), m_is_has_photo(false), m_io_context(asio::io_context()),
m_worker(nullptr), m_db(Database()), m_buffer(std::array<char, 1024>()), m_socket(m_io_context) {}

void Client::run() {
    m_db.init();
    m_isReceiving = true; // Убедитесь, что флаг инициализирован
    startAsyncReceive();
    m_io_contextThread = std::thread([this]() { m_io_context.run(); });
}

void Client::startAsyncReceive() {
    if (m_isReceiving) {
        // Начинаем асинхронное чтение
        m_socket.async_read_some(asio::buffer(m_buffer),
            [this](std::error_code error, std::size_t bytes_transferred) {
                handleAsyncReceive(error, bytes_transferred);
            });
    }
}

void Client::connectTo(const std::string& ipAddress, int port) {
    try {
        m_endpoint = asio::ip::tcp::endpoint(asio::ip::make_address(ipAddress), port);
        m_socket.connect(m_endpoint); 
        std::cout << "Подключение успешно!" << std::endl;
    }
    catch (const asio::system_error& e) {
        std::cerr << "Ошибка подключения: " << e.what() << " (код ошибки: " << e.code() << ")" << std::endl;
    }
}

void Client::handleAsyncReceive(const std::error_code& error, std::size_t bytes_transferred) {
    if (error) {
        if (error == asio::error::eof) {
            std::cout << "Connection closed by server." << std::endl;
        }
        else if (error == asio::error::connection_reset) {
            std::cerr << "Connection reset by server." << std::endl;
        }
        else {
            std::cerr << "Receive error: " << error.message() << std::endl;
        }
        return; // Завершаем обработку в случае ошибки
    }

    // Обрабатываем полученные данные
    m_accumulated_data.append(m_buffer.data(), bytes_transferred);

    // Проверяем, есть ли завершение пакета
    size_t pos = m_accumulated_data.find(c_endPacket);
    while (pos != std::string::npos) {
        std::string packet = m_accumulated_data.substr(0, pos);
        std::cout << "Received: " << packet << std::endl;
        handleResponse(packet);

        m_accumulated_data.erase(0, pos + c_endPacket.length());
        pos = m_accumulated_data.find(c_endPacket);
    }

    // Запускаем следующее асинхронное чтение
    startAsyncReceive();
}

void Client::close() {
    m_isReceiving = false;
    m_socket.close();
    m_io_context.stop(); // Important: Stop the io_context
    if (m_io_contextThread.joinable()) {
        m_io_contextThread.join();
    }
}

void Client::handleResponse(const std::string& packet) {
    std::istringstream iss(packet);

    std::string type;
    std::getline(iss, type);
    
    if (type == "REGISTRATION_SUCCESS") {
        sh_is_authorized = OperationResult::SUCCESS;
    }
    else if (type == "REGISTRATION_FAIL") {
        sh_is_authorized = OperationResult::FAIL;
    }
    else if (type == "AUTHORIZATION_SUCCESS") {
        sh_is_authorized = OperationResult::SUCCESS;
    }
    else if (type == "AUTHORIZATION_FAIL") {
        sh_is_authorized = OperationResult::FAIL;
    }
    else if (type == "CHAT_CREATE_SUCCESS") {
        sh_chat_create = OperationResult::SUCCESS;
    }
    else if (type == "CHAT_CREATE_FAIL") {
        sh_chat_create = OperationResult::FAIL;
    }
    else if (type == "MESSAGE_SUCCESS") {
        sh_is_message_send = OperationResult::SUCCESS;
    }
    else if (type == "MESSAGE_FAIL") {
        sh_is_message_send = OperationResult::FAIL;
    }
    else if (type == "MESSAGE_READ_CONFIRAMTION_SUCESS") {
        sh_is_message_read_confirmation_send = OperationResult::SUCCESS;
    }
    else if (type == "MESSAGE_READ_CONFIRAMTION_FAIL") {
        sh_is_message_read_confirmation_send = OperationResult::FAIL;
    }


    else if (type == "STATUS") {
        m_worker->onStatusReceive();
    }
    else if (type == "MESSAGE") {
        m_worker->onMessageReceive();
    }
    else if (type == "MESSAGE_READ_CONFIRMATION") {
        m_worker->onMessageReadConfirmationReceive();
    }
}

OperationResult Client::authorizeClient(const std::string& login, const std::string& password) {
    sendPacket(m_sender.get_authorization_QueryStr(login, password));

    auto startTime = std::chrono::steady_clock::now(); 
    auto timeout = std::chrono::seconds(5); 

    while (sh_is_authorized == OperationResult::NOT_STATED) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= timeout) {
            return OperationResult::REQUEST_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (sh_is_authorized == OperationResult::SUCCESS) {
        return OperationResult::SUCCESS;
    }
    else {
        return OperationResult::FAIL;
    }
}

OperationResult Client::registerClient(const std::string& login, const std::string& password, const std::string& name) {
    m_my_login = login;
    m_my_name = name;
    sendPacket(m_sender.get_registration_QueryStr(login, name, password));

    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(5);

    while (sh_is_authorized == OperationResult::NOT_STATED) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= timeout) {
            return OperationResult::REQUEST_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (sh_is_authorized == OperationResult::SUCCESS) {
        return OperationResult::SUCCESS;
    }
    else {
        return OperationResult::FAIL;
    }
}

OperationResult Client::createChatWith(const std::string& friendLogin) {
    std::lock_guard<std::mutex> guard(m_mtx);

    m_map_friend_login_to_chat[friendLogin] = new Chat;
    sendPacket(m_sender.get_createChat_QueryStr(m_my_login, friendLogin));

    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(5);

    while (sh_chat_create == OperationResult::NOT_STATED) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= timeout) {
            return OperationResult::REQUEST_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (sh_chat_create == OperationResult::SUCCESS) {
        return OperationResult::SUCCESS;
    }

    else {
        return OperationResult::FAIL;
    }

}

OperationResult Client::sendMyStatus(const std::string& status) {
    std::vector<std::string> friendsLoginsVec;
    for (const auto& pair : m_map_friend_login_to_chat) {
        friendsLoginsVec.push_back(pair.first);
    }
    std::string packet = m_sender.get_status_ReplyStr(status, m_my_login, friendsLoginsVec);
    sendPacket(packet);

    return waitForResponse(5, [this] {
        return sh_is_status_send;
        });
}

OperationResult Client::updateMyInfo(const std::string& login, const std::string& name, const std::string& password, bool isHasPhoto, Photo photo) {
    m_my_login = login;
    m_my_name = name;
    m_is_has_photo = isHasPhoto;
    m_my_photo = photo;
    std::string packet = m_sender.get_updateMyInfo_QueryStr(login, name, password, isHasPhoto, photo);
    sendPacket(packet);

    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(5);

    while (sh_is_info_updated == OperationResult::NOT_STATED) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= timeout) {
            return OperationResult::REQUEST_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (sh_is_info_updated == OperationResult::SUCCESS) {
        return OperationResult::SUCCESS;
    }
    else {
        return OperationResult::FAIL;
    }
}

OperationResult Client::waitForResponse(int seconds, std::function<OperationResult()> checkFunction) {
    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(seconds); 
    while (true) { 
        if (checkFunction() != OperationResult::FAIL) {
            return checkFunction();
        }
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
        if (elapsedTime >= timeout) {
            return OperationResult::FAIL;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    sh_is_message_send = OperationResult::NOT_STATED;
}

OperationResult Client::sendMessage(const std::string& friendLogin, const std::string& message, std::string timestamp) {
    std::lock_guard<std::mutex> guard(m_mtx);
    sendPacket(m_sender.get_message_ReplyStr(m_my_login, friendLogin, message, timestamp));
    return waitForResponse(5, [this] {
        return sh_is_message_send;
        });
}

OperationResult Client::sendMessageReadConfirmation(const std::string& friendLogin, const std::vector<Message*>& messagesReadIdsVec) {
    std::lock_guard<std::mutex> guard(m_mtx);
    sendPacket(m_sender.get_messageReadConfirmation_ReplyStr(m_my_login, friendLogin, messagesReadIdsVec));
    return waitForResponse(5, [this] {
        return sh_is_message_read_confirmation_send;
        });
}

void Client::setWorkerUI(WorkerUI* workerImpl) {
    m_worker = workerImpl;
}

void Client::sendPacket(const std::string& packet) {
    asio::write(m_socket, asio::buffer(packet), asio::transfer_all());
}


void Client::save() const {
    QJsonObject jsonObject;
    QJsonArray chatsArray;

    for (const auto& chatPair : m_map_friend_login_to_chat) {
        chatsArray.append(chatPair.second->serialize(m_db));
    }
    jsonObject["chatsArray"] = chatsArray;

    jsonObject["my_login"] = QString::fromStdString(m_my_login);
    jsonObject["my_name"] = QString::fromStdString(m_my_name);
    jsonObject["is_has_photo"] = m_is_has_photo;
    jsonObject["my_photo"] = QString::fromStdString(m_my_photo.getPhotoPath());

    QString dir = Utility::getSaveDir();
    QString fileName = QString::fromStdString(m_my_login) + ".json";

    QDir saveDir(dir);
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(".")) {
            qWarning() << "era:" << dir;
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
        qWarning() << "era" << fullPath;
    }
}

void Client::load(const std::string& fileName) {
    QFile file(QString::fromStdString(fileName));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the file:" << QString::fromStdString(fileName);
    }

    QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!loadDoc.isObject()) {
        qWarning() << "Invalid JSON in the file:" << QString::fromStdString(fileName);
    }

    QJsonObject jsonObject = loadDoc.object();
    m_my_login = jsonObject["my_login"].toString().toStdString();
    m_my_name = jsonObject["my_name"].toString().toStdString();
    m_is_has_photo = jsonObject["is_has_photo"].toBool();

    if (jsonObject.contains("my_photo")) {
        m_my_photo = Photo(jsonObject["my_photo"].toString().toStdString()); 
    }

    if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].isArray()) {
        QJsonArray chatsArray = jsonObject["chatsArray"].toArray();
        for (const QJsonValue& value : chatsArray) {
            Chat* chat = Chat::deserialize(value.toObject(), m_db);
            if (chat) {
                m_map_friend_login_to_chat[chat->getFriendLogin()] = chat; 
            }
        }
    } 
}
