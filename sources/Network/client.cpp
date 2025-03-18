#include "Client.h"
#include "utility.h"
#include "workerUI.h"
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>

Client::Client() : m_isReceiving(true),
sh_is_authorized(OperationResult::NOT_STATED),
sh_is_message_send(OperationResult::NOT_STATED),
sh_is_first_message_send(OperationResult::NOT_STATED),
sh_is_info_updated(OperationResult::NOT_STATED),
sh_is_message_read_confirmation_send(OperationResult::NOT_STATED),
sh_chat_create(OperationResult::NOT_STATED),
sh_is_status_send(OperationResult::NOT_STATED),
sh_is_user_info(OperationResult::NOT_STATED),
sh_is_statuses(OperationResult::NOT_STATED),
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
    else if (type == "USER_INFO_SUCCESS") {
        processUserInfoSuccess(iss.str());
        sh_is_user_info = OperationResult::SUCCESS;
    }
    else if (type == "USER_INFO_FAIL") {
        sh_is_user_info = OperationResult::FAIL;
    }
    else if (type == "REGISTRATION_FAIL") {
        sh_is_authorized = OperationResult::FAIL;
    }
    else if (type == "AUTHORIZATION_SUCCESS") {
        sh_packet_auth = iss.str();
        sh_is_authorized = OperationResult::SUCCESS;
    }
    else if (type == "AUTHORIZATION_FAIL") {
        sh_is_authorized = OperationResult::FAIL;
    }
    else if (type == "CHAT_CREATE_SUCCESS") {
        processChatCreateSuccess(iss.str());
        sh_chat_create = OperationResult::SUCCESS;
    }
    else if (type == "FRIENDS_STATUSES") {
        processFriendsStatusesSuccess(iss.str());
        sh_is_statuses = OperationResult::SUCCESS;
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
    else if (type == "LOGIN_TO_SEND_STATUS") {
        std::string login;
        std::getline(iss, login);
        m_vec_friends_logins_tmp.push_back(login);
    }

    else if (type == "STATUS") {
        m_worker->onStatusReceive(iss.str());
    }
    else if (type == "MESSAGE") {
        m_worker->onMessageReceive(iss.str());
    }
    else if (type == "FIRST_MESSAGE") {
        m_worker->onFirstMessageReceive(iss.str());
    }
    else if (type == "MESSAGES_READ_CONFIRMATION") {
        m_worker->onMessageReadConfirmationReceive(iss.str());
    }
}

void Client::processFriendsStatusesSuccess(const std::string& packet) {
    std::istringstream iss(packet);
    std::string type;
    std::getline(iss, type);

    std::string vecBegin;
    std::getline(iss, vecBegin);

    std::string line;
    while (std::getline(iss, line)) {
        if (line != "VEC_END") {
            int index = line.find(',');
            std::string login = line.substr(0, index);
            std::string status = line.substr(++index);
            Chat* chat = m_map_friend_login_to_chat[login];
            chat->setFriendLastSeen(status);
        }
    }
}

void Client::processUserInfoSuccess(const std::string& packet) {
    std::istringstream iss(packet);
    std::string type;
    std::getline(iss, type);

    std::string login;
    std::getline(iss, login);

    std::getline(iss, m_my_name);

    std::string isHasPhoto;
    std::getline(iss, isHasPhoto);
    m_is_has_photo = isHasPhoto == "true";

    std::string photoStr;
    std::getline(iss, photoStr);
    m_my_photo = *Photo::deserialize(photoStr);
}

void Client::processChatCreateSuccess(const std::string& packet) {
    std::istringstream iss(packet);
    std::string type;
    std::getline(iss, type);

    std::string login;
    std::getline(iss, login);
    std::string name;
    std::getline(iss, name);
    std::string isHasPhoto;
    std::getline(iss, isHasPhoto);
    std::string photoStr;
    std::getline(iss, photoStr);
    std::string lastSeen;
    std::getline(iss, lastSeen);


    Chat* chat = new Chat;
    chat->setFriendLogin(login);
    chat->setFriendName(name);
    chat->setIsFriendHasPhoto(isHasPhoto == "true");
    chat->setLayoutIndex(0);

    for (auto& pair : m_map_friend_login_to_chat) {
        Chat* chatTmp = pair.second;
        chatTmp->setLayoutIndex(chatTmp->getLayoutIndex() + 1);
    }

    Photo* photo = Photo::deserialize(photoStr);
    chat->setFriendPhoto(photo);
    chat->setFriendLastSeen(lastSeen);
    chat->setLastIncomeMsg("no messages yet");


    m_map_friend_login_to_chat[login] = chat;
}

OperationResult Client::authorizeClient(const std::string& login, const std::string& password) {
    sendPacket(m_sender.get_authorization_QueryStr(login, password));

    auto startTime = std::chrono::steady_clock::now(); 
    auto timeout = std::chrono::seconds(2); 

    while (sh_is_authorized == OperationResult::NOT_STATED) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= timeout) {
            return OperationResult::REQUEST_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (sh_is_authorized == OperationResult::SUCCESS) {
        m_worker->onAuthorization(sh_packet_auth);
        return OperationResult::SUCCESS;
    }
    else {
        sh_is_authorized = OperationResult::NOT_STATED;
        return OperationResult::FAIL;
    }
}

OperationResult Client::registerClient(const std::string& login, const std::string& password, const std::string& name) {
    m_my_login = login;
    m_my_name = name;
    sendPacket(m_sender.get_registration_QueryStr(login, name, password));

    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(2);

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
        sh_is_authorized = OperationResult::NOT_STATED;
        return OperationResult::FAIL;
    }
}

OperationResult Client::createChatWith(const std::string& friendLogin) {
    std::lock_guard<std::mutex> guard(m_mtx);

    m_map_friend_login_to_chat[friendLogin] = new Chat;
    sendPacket(m_sender.get_createChat_QueryStr(m_my_login, friendLogin));

    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::milliseconds(300);

    while (sh_chat_create == OperationResult::NOT_STATED) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= timeout) {
            return OperationResult::REQUEST_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (sh_chat_create == OperationResult::SUCCESS) {
        sh_chat_create = OperationResult::NOT_STATED;
        return OperationResult::SUCCESS;
    }

    else {
        sh_chat_create = OperationResult::NOT_STATED;
        return OperationResult::FAIL;
    }
}

OperationResult Client::sendMyStatus(const std::string& status) {
    std::vector<std::string> friendsLoginsVec;
    for (const auto& pair : m_map_friend_login_to_chat) {
        friendsLoginsVec.push_back(pair.first);
    }
    for (const auto& login : m_vec_friends_logins_tmp) {
        friendsLoginsVec.push_back(login);
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
        sh_is_info_updated = OperationResult::NOT_STATED;
        return OperationResult::SUCCESS;
    }
    else {
        sh_is_info_updated = OperationResult::NOT_STATED;
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

OperationResult Client::sendMessage(const std::string& friendLogin, const std::string& message, const std::string& id, std::string timestamp) {
    std::lock_guard<std::mutex> guard(m_mtx);
    sendPacket(m_sender.get_message_ReplyStr(m_my_login, friendLogin, message, id, timestamp));
    return waitForResponse(5, [this] {
        return sh_is_message_send;
        });

}

OperationResult Client::sendFirstMessage(const std::string& friendLogin, const std::string& message, const std::string& id, const std::string timestamp) {
    std::lock_guard<std::mutex> guard(m_mtx);
    sendPacket(m_sender.get_first_message_ReplyStr(m_my_login, m_my_name, m_is_has_photo, m_my_photo, friendLogin, message, id, timestamp));
    return waitForResponse(5, [this] {
        return sh_is_message_send;
        });
}

OperationResult Client::sendMessageReadConfirmation(const std::string& friendLogin, const std::vector<Message*>& messagesReadIdsVec) {
    for (auto msg : messagesReadIdsVec) {
        msg->setIsRead(true);
    }

    std::lock_guard<std::mutex> guard(m_mtx);
    sendPacket(m_sender.get_messageReadConfirmation_ReplyStr(m_my_login, friendLogin, messagesReadIdsVec));
    return waitForResponse(5, [this] {
        return sh_is_message_read_confirmation_send;
        });
}

OperationResult Client::getMyInfoFromServer(const std::string& myLogin) {
    std::lock_guard<std::mutex> guard(m_mtx);
    sendPacket(m_sender.get_loadFriendInfo_QueryStr(myLogin));
    return waitForResponse(5, [this] {
        return sh_is_user_info;
        });
}

OperationResult Client::getFriendsStatuses(std::vector<std::string> vecFriends) {
    std::lock_guard<std::mutex> guard(m_mtx);
    sendPacket(m_sender.get_loadAllFriendsStatuses_QueryStr(vecFriends));
    return waitForResponse(5, [this] {
        return sh_is_statuses;
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

    // Сериализация чатов
    for (const auto& chatPair : m_map_friend_login_to_chat) {
        chatsArray.append(chatPair.second->serialize(m_db));
    }
    jsonObject["chatsArray"] = chatsArray;

    // Сохранение информации о клиенте
    jsonObject["my_login"] = QString::fromStdString(m_my_login);
    jsonObject["my_name"] = QString::fromStdString(m_my_name);
    jsonObject["is_has_photo"] = m_is_has_photo;
    jsonObject["my_photo"] = QString::fromStdString(m_my_photo.getPhotoPath());

    // Сохранение вектора логинов друзей
    QJsonArray friendsLoginsArray;
    for (const auto& login : m_vec_friends_logins_tmp) {
        friendsLoginsArray.append(QString::fromStdString(login));
    }
    jsonObject["friends_logins"] = friendsLoginsArray;

    // Определение директории для сохранения
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


bool Client::load(const std::string& fileName) {
    QString dir = Utility::getSaveDir();
    QString fileNameFinal = QString::fromStdString(fileName);
    QDir saveDir(dir);
    QString fullPath = saveDir.filePath(fileNameFinal);

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the file:" << QString::fromStdString(fileName);
        return false; // Добавляем return, чтобы избежать дальнейших операций, если файл не открыт
    }

    QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!loadDoc.isObject()) {
        qWarning() << "Invalid JSON in the file:" << QString::fromStdString(fileName);
        return false; // Добавляем return для обработки ошибки
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

    // Чтение вектора логинов друзей
    if (jsonObject.contains("friends_logins") && jsonObject["friends_logins"].isArray()) {
        QJsonArray friendsLoginsArray = jsonObject["friends_logins"].toArray();
        m_vec_friends_logins_tmp.clear(); // Очищаем вектор перед загрузкой
        for (const QJsonValue& value : friendsLoginsArray) {
            m_vec_friends_logins_tmp.push_back(value.toString().toStdString());
        }
    }

    return true;
}

bool Client::isAuthorized() {
    if (sh_is_authorized == OperationResult::SUCCESS) {
        return true;
    }
    else {
        return false;
    }
}