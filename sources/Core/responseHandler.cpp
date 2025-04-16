#include "responseHandler.h"
#include "workerUI.h"
#include "chatsWidget.h"
#include "client.h"
#include "utility.h"
#include "asio.hpp"
#include "photo.h"
#include "chat.h"


ResponseHandler::ResponseHandler(Client* client)
	: m_client(client), m_worker_UI(nullptr) {
 
}

void ResponseHandler::setWorkerUI(WorkerUI* workerImpl) {
    m_worker_UI = workerImpl;
}

void ResponseHandler::handleResponse(const std::string& packet) {
    std::istringstream iss(packet);

    std::string type;
    std::getline(iss, type);

    if (type != "REGISTRATION_SUCCESS" && type != "AUTHORIZATION_SUCCESS" && type != "REGISTRATION_FAIL" && type != "AUTHORIZATION_FAIL") {
        m_client->waitUntilUIReadyToUpdate();
    }


    if (type == "REGISTRATION_SUCCESS") {
        onRegistrationSuccess();
    }
    else if (type == "REGISTRATION_FAIL") {
        onRegistrationFail();
    }
    else if (type == "AUTHORIZATION_SUCCESS") {
        onAuthorizationSuccess();
    }
    else if (type == "AUTHORIZATION_FAIL") {
        onAuthorizationFail();
    }
    else if (type == "CHAT_CREATE_SUCCESS") {
        onChatCreateSuccess(packet);
    }
    else if (type == "CHAT_CREATE_FAIL") {
        onChatCreateFail();
    }
    else if (type == "FRIENDS_STATUSES") {
        processFriendsStatusesSuccess(iss.str());
    }
    else if (type == "MESSAGE") {
        onMessageReceive(iss.str());
    }
    else if (type == "USER_INFO") {
        onUserInfo(iss.str());
    }
    else if (type == "MESSAGES_READ_CONFIRMATION") {
        onMessageReadConfirmationReceive(iss.str());
    }
    else if (type == "STATUS") {
        onStatusReceive(iss.str());
    }
}

void ResponseHandler::onRegistrationSuccess() {
    const std::string& myLogin = m_client->getMyLogin();
    m_client->setIsNeedToSaveConfig(true);

    m_worker_UI->onRegistrationSuccess();
}

void ResponseHandler::onRegistrationFail() {
    m_worker_UI->onRegistrationFail();
}



void ResponseHandler::onAuthorizationSuccess() {
    const std::string& myLogin = m_client->getMyLogin();

    bool res = m_client->load(myLogin + ".json");
    if (!res) {
        m_client->requestFriendInfoFromServer(myLogin);
        m_worker_UI->showConfigLoadErrorDialog();
    }

    m_client->getAllFriendsStatuses();
    m_client->broadcastMyStatus("online");
    m_client->setIsNeedToSaveConfig(true);

    m_worker_UI->onAuthorizationSuccess();
}

void ResponseHandler::onAuthorizationFail() {
    m_worker_UI->onAuthorizationFail();
}



void ResponseHandler::onChatCreateSuccess(const std::string& packet) {
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

    utility::incrementAllChatLayoutIndexes(m_client->getMyChatsMap());

    Photo* photo = Photo::deserialize(base64_decode(photoStr), login);
    chat->setFriendPhoto(photo);
    chat->setFriendLastSeen(lastSeen);
    chat->setLastReceivedOrSentMessage("no messages yet");

    m_client->getMyChatsMap().emplace(login, chat);

    m_worker_UI->onChatCreateSuccess(chat);
}

void ResponseHandler::onChatCreateFail() {
    m_worker_UI->onChatCreateFail();
}



void ResponseHandler::processFriendsStatusesSuccess(const std::string& packet) {
    std::istringstream iss(packet);
    std::string type;
    std::getline(iss, type);

    std::string vecBegin;
    std::getline(iss, vecBegin);

    auto& chatsMap = m_client->getMyChatsMap();

    std::vector<std::pair<std::string, std::string>> loginToStatusPairsVec;
    std::string line;
    while (std::getline(iss, line)) {
        if (line != "VEC_END") {
            int index = line.find(',');
            std::string login = line.substr(0, index);
            std::string status = line.substr(++index);
            loginToStatusPairsVec.emplace_back(std::make_pair(login, status));

            auto it = chatsMap.find(login);
            if (it != chatsMap.end()) {
                Chat* chat = it->second;
                chat->setFriendLastSeen(status);
            }
        }
        else if (line == "VEC_END") {
            break;
        }
    }

    m_worker_UI->updateFriendsStatuses(loginToStatusPairsVec);
}



void ResponseHandler::onMessageReceive(const std::string& packet) {

    std::istringstream iss(packet);
    std::string type;
    std::getline(iss, type);

    std::string myLogin;
    std::getline(iss, myLogin);

    std::string type2;
    std::getline(iss, type2);

    std::string friendLogin;
    std::getline(iss, friendLogin);

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
        }
    }

    std::string id;
    std::getline(iss, id);

    std::string timestamp;
    std::getline(iss, timestamp);
    
    Message* msg = new Message(message, timestamp, id, false);
    msg->setIsRead(false);

    auto& chatsMap = m_client->getMyChatsMap();

    auto chatPairIt = chatsMap.find(friendLogin);
    std::pair<std::string, Chat*> chatPair;
    if (chatPairIt != chatsMap.end()) {
        chatPair = *chatPairIt;

        Chat* chat = chatPair.second;
        chat->setLastReceivedOrSentMessage(msg->getMessage());
        chat->getMessagesVec().push_back(msg);

        if (chat->getLayoutIndex() != 0) {
            utility::increasePreviousChatIndexes(chatsMap, chat);
        }
        chat->setLayoutIndex(0);

        m_worker_UI->onMessageReceive(friendLogin, msg);
    }
    else {
        Chat* chat = new Chat;
        chat->setFriendLastSeen("online");
        chat->setFriendLogin(friendLogin);
        auto& msgsVec = chat->getMessagesVec();
        msgsVec.push_back(msg);

        utility::incrementAllChatLayoutIndexes(chatsMap);
        chat->setLayoutIndex(0);

        chatsMap[friendLogin] = chat;

        m_client->requestFriendInfoFromServer(friendLogin);
    }
}


void ResponseHandler::onUserInfo(const std::string& packet) {

    std::istringstream iss(packet);
    std::string type;
    std::getline(iss, type);

    std::string login;
    std::getline(iss, login);

    std::string name;
    std::getline(iss, name);

    std::string lastSeen;
    std::getline(iss, lastSeen);

    std::string isHasPhotoStr;
    std::getline(iss, isHasPhotoStr);
    bool isHasPhoto = isHasPhotoStr == "true";

    std::string sizeStr;
    std::getline(iss, sizeStr);

    std::string photoStr;
    std::getline(iss, photoStr);

    Photo* photo = Photo::deserialize(base64_decode(photoStr), login);

    auto& chatsMap = m_client->getMyChatsMap();
    const auto it = chatsMap.find(login);

    if (it != chatsMap.end()) {
        Chat* chat = it->second;
        chat->setFriendName(name);
        chat->setFriendLastSeen(lastSeen);
        chat->setIsFriendHasPhoto(isHasPhoto);
        chat->setFriendPhoto(photo);

        std::vector<Message*>& messagesVec = chat->getMessagesVec();
        if (messagesVec.size() == 0) {
            chat->setLastReceivedOrSentMessage("no messages yet");
        }
        else {
            chat->setLastReceivedOrSentMessage(messagesVec.back()->getMessage());
        }

        m_worker_UI->showNewChatOrUpdateExisting(chat);
    }
}



void ResponseHandler::onMessageReadConfirmationReceive(const std::string& packet) {

    std::istringstream iss(packet);

    std::string type;
    std::getline(iss, type);

    std::string myLogin;
    std::getline(iss, myLogin);

    std::getline(iss, type);

    std::string friendLogin;
    std::getline(iss, friendLogin);

    std::string id;
    std::getline(iss, id);

    auto& chatsMap = m_client->getMyChatsMap();
    auto chatPair = chatsMap.find(friendLogin);

    if (chatPair != chatsMap.end()) {
        Chat* chat = chatPair->second;

        auto& messagesVec = chat->getMessagesVec();
        auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&id](Message* msg) {
            return msg->getId() == id;
            });
        Message* msg = *msgChatIt;
        msg->setIsRead(true);

        m_worker_UI->onMessageReadConfirmationReceive(friendLogin, id);

        if (chat->getFriendLastSeen() == "Their last visit ? A mystery for the ages.") {
            chat->setFriendLastSeen("online");
            m_worker_UI->onStatusReceive(friendLogin, "online");
        }
    }
}



void ResponseHandler::onStatusReceive(const std::string& packet) {

    std::istringstream iss(packet);

    std::string type;
    std::getline(iss, type);

    std::string friendLogin;
    std::getline(iss, friendLogin);

    std::string status;
    std::getline(iss, status);

    auto& chatsMap = m_client->getMyChatsMap();
    auto chatPair = chatsMap.find(friendLogin);
    if (chatPair != chatsMap.end()) {
        Chat* chat = chatPair->second; 
        chat->setFriendLastSeen(status);

        m_worker_UI->onStatusReceive(friendLogin, status);
    }
}