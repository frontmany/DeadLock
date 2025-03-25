#include "Client.h"
#include "utility.h"
#include "base64.h"
#include "workerUI.h"
#include <iostream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <QFile>

Client::Client() :
    m_isReceiving(true),
    sh_is_authorized(OperationResult::NOT_STATED),
    sh_is_message_send(OperationResult::NOT_STATED),
    sh_is_first_message_send(OperationResult::NOT_STATED),
    sh_is_info_updated(OperationResult::NOT_STATED),
    sh_is_message_read_confirmation_send(OperationResult::NOT_STATED),
    sh_chat_create(OperationResult::NOT_STATED),
    sh_is_status_send(OperationResult::NOT_STATED),
    sh_is_user_info(OperationResult::NOT_STATED),
    sh_is_statuses(OperationResult::NOT_STATED),
    m_my_photo(Photo()),
    m_is_has_photo(false),
    m_io_context(asio::io_context()),
    m_worker(nullptr),
    m_db(Database()),
    m_buffer(std::make_shared<asio::streambuf>()), 
    m_delimiter("_+14?bb5HmR;%@`7[S^?!#sL8"),
    m_socket(m_io_context) {
}

void Client::run() {
    m_db.init();
    m_isReceiving = true;
    startAsyncReceive();
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
    if (m_isReceiving) {
        // Подготавливаем буфер для новых данных
        m_buffer->prepare(1024 * 1024 * 20); // Подготовка 1KB пространства

        asio::async_read_until(m_socket, *m_buffer, m_delimiter,
            [this](const std::error_code& ec, std::size_t bytes_transferred) {
                handleAsyncReceive(ec, bytes_transferred);
            });
    }
}

void Client::handleAsyncReceive(const std::error_code& ec, std::size_t bytes_transferred) {
    if (ec) {
        std::cerr << "Error during receive: " << ec.message() << std::endl;
        return; // Завершаем обработку в случае ошибки
    }

    // Извлекаем данные из буфера
    std::istream is(m_buffer.get());
    std::string message;

    // Читаем весь буфер
    std::string buffer_content((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());

    // Если буфер не пустой, обрабатываем сообщение
    if (!buffer_content.empty()) {
        std::cout << "Received complete message: " << buffer_content << std::endl;
        handleResponse(buffer_content);
    }

    // Удаляем обработанные данные из буфера
    m_buffer->consume(bytes_transferred);

    // Продолжаем чтение, если еще получаем данные
    if (m_isReceiving) {
        startAsyncReceive();
    }
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


    else if (type == "FRIEND_INFO") {
        m_worker->onFriendInfoReceive(iss.str());
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
        else if (line == "VEC_END"){
            break;
        }
    }
    isStatuses = true;
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


    std::string sizeStr;
    std::getline(iss, sizeStr);
    size_t size = std::stoi(sizeStr);

    std::string photoStr; 
    std::getline(iss, photoStr);


    m_my_photo = *Photo::deserialize(photoStr, size, login);
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

    std::string sizeStr;
    std::getline(iss, sizeStr);
    size_t size = std::stoi(sizeStr);

    std::string lastSeen;
    std::getline(iss, lastSeen);

    std::string photoStr;
    std::getline(iss, photoStr);



    Chat* chat = new Chat;
    chat->setFriendLogin(login);
    chat->setFriendName(name);
    chat->setIsFriendHasPhoto(isHasPhoto == "true");
    chat->setLayoutIndex(0);

    for (auto& pair : m_map_friend_login_to_chat) {
        Chat* chatTmp = pair.second;
        chatTmp->setLayoutIndex(chatTmp->getLayoutIndex() + 1);
    }

    Photo* photo = Photo::deserialize(base64_decode(photoStr), size, login);
    chat->setFriendPhoto(photo);
    chat->setFriendLastSeen(lastSeen);
    chat->setLastIncomeMsg("no messages yet");


    m_map_friend_login_to_chat[login] = chat;
    std::cout << "statuses received\n";
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

    std::vector<std::string> logins;
    logins.reserve(m_map_friend_login_to_chat.size());
    for (auto [login, chat] : m_map_friend_login_to_chat) {
        logins.emplace_back(login);
    }

    for (auto login : m_vec_friends_logins_tmp) {
        if (std::find(logins.begin(), logins.end(), login) != logins.end()) {
            logins.push_back(login);
        }
    }

    std::string packet = m_sender.get_updateMyInfo_QueryStr(login, name, password, isHasPhoto, photo, logins);
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
    
    auto startTime = std::chrono::steady_clock::now();
    auto timeout = std::chrono::seconds(5);

    while (sh_is_statuses == OperationResult::NOT_STATED) {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);

        if (elapsedTime >= timeout) {
            return OperationResult::REQUEST_TIMEOUT;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if (sh_is_statuses == OperationResult::SUCCESS) {
        sh_is_statuses = OperationResult::NOT_STATED;
        return OperationResult::SUCCESS;
    }

    else {
        sh_is_statuses = OperationResult::NOT_STATED;
        return OperationResult::FAIL;
    } 
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

void Client::loadAvatarFromPC(const std::string& login) {
    QString dir = Utility::getSaveDir();
    QString fileNameFinal = QString::fromStdString(login) + "myMainPhoto.png";
    QDir saveDir(dir);
    QString fullPath = saveDir.filePath(fileNameFinal);

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the file:" << fileNameFinal.toStdString();
        return;
    }
    m_my_photo.setPhotoPath(fullPath.toStdString());
    m_is_has_photo = true;
}

bool Client::isAuthorized() {
    if (sh_is_authorized == OperationResult::SUCCESS) {
        return true;
    }
    else {
        return false;
    }
}