#include "responseHandler.h"
#include "workerUI.h"
#include "chatsWidget.h"
#include "friendInfo.h"
#include "client.h"
#include "utility.h"
#include "queryType.h"
#include "message.h"
#include "photo.h"
#include "chat.h"
#include "net.h"

ResponseHandler::ResponseHandler(Client* client)
	: m_client(client), m_worker_UI(nullptr) {}

void ResponseHandler::setWorkerUI(WorkerUI* workerImpl) {
    m_worker_UI = workerImpl;
}

void ResponseHandler::handleResponse(net::message<QueryType>& msg) {
    if (msg.header.type != QueryType::REGISTRATION_SUCCESS && 
        msg.header.type != QueryType::AUTHORIZATION_SUCCESS &&
        msg.header.type != QueryType::REGISTRATION_FAIL &&
        msg.header.type != QueryType::AUTHORIZATION_FAIL) {
        m_client->waitUntilUIReadyToUpdate();
    }

    std::string packet = "";
    if (msg.body.size() > 0) {
        msg >> packet;
    }


    if (msg.header.type == QueryType::REGISTRATION_SUCCESS) {
        onRegistrationSuccess();
        m_client->bindFileConnectionToMeOnServer();
    }
    else if (msg.header.type == QueryType::REGISTRATION_FAIL) {
        onRegistrationFail();
    }
    else if (msg.header.type == QueryType::AUTHORIZATION_SUCCESS) {
        onAuthorizationSuccess();
        m_client->bindFileConnectionToMeOnServer();
    }
    else if (msg.header.type == QueryType::AUTHORIZATION_FAIL) {
        onAuthorizationFail();
    }
    else if (msg.header.type == QueryType::CHAT_CREATE_SUCCESS) {
        onChatCreateSuccess(packet);
    }
    else if (msg.header.type == QueryType::CHAT_CREATE_FAIL) {
        onChatCreateFail();
    }
    else if (msg.header.type == QueryType::FRIENDS_STATUSES) {
        processFriendsStatusesSuccess(packet);
    }
    else if (msg.header.type == QueryType::MESSAGE) {
        onMessageReceive(packet);
    }
    else if (msg.header.type == QueryType::USER_INFO) {
        onUserInfo(packet);
    }
    else if (msg.header.type == QueryType::MESSAGES_READ_CONFIRMATION) {
        onMessageReadConfirmationReceive(packet);
    }
    else if (msg.header.type == QueryType::STATUS) {
        onStatusReceive(packet);
    }
    else if (msg.header.type == QueryType::VERIFY_PASSWORD_FAIL) {
        onPasswordVerifyFail();
    }
    else if (msg.header.type == QueryType::VERIFY_PASSWORD_SUCCESS) {
        onPasswordVerifySuccess();
    }
    else if (msg.header.type == QueryType::NEW_LOGIN_SUCCESS) {
        onCheckNewLoginSuccess(packet);
    }
    else if (msg.header.type == QueryType::NEW_LOGIN_FAIL) {
        onPasswordVerifySuccess();
    }
    else if (msg.header.type == QueryType::ALL_PENDING_MESSAGES_WERE_SENT) {
        m_client->getAllFriendsStatuses();
    }
    else if (msg.header.type == QueryType::FIND_USER_RESULTS) {
        processFoundUsers(packet);
    }
    else if (msg.header.type == QueryType::TYPING) {
        onTyping(packet);
    }

    // new
    else if (msg.header.type == QueryType::PREPARE_TO_RECEIVE_FILE) {
        prepareToReceiveFile(packet);
        
    }
    else if (msg.header.type == QueryType::PREPARE_TO_RECEIVE_REQUESTED_FILE) {
        prepareToReceiveRequestedFile(packet);
    }

    else if (msg.header.type == QueryType::FILE_PREVIEW) {
        onFilePreview(packet);
    }
}

void ResponseHandler::handleFile(const net::file<QueryType>& file) {
    auto& messageBlobsMap = m_client->getMapMessageBlobs();

    if (m_is_received_file_requested) {
        auto& chatsMap = m_client->getMyChatsMap();
        auto it = chatsMap.find(file.receiverLogin);
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            auto& chatsVec = chat->getMessagesVec();
            auto it = std::find_if(chatsVec.begin(), chatsVec.end(), [&file](Message* msg) {
                return msg->getId() == file.blobUID;
            });

            if (it != chatsVec.end()) {
                Message* message = *it;
                auto& relatedFilesVec = message->getRelatedFiles();
                auto itFileWrapper = std::find_if(relatedFilesVec.begin(), relatedFilesVec.end(), [&file](fileWrapper fileWrapper) {
                    return fileWrapper.file.id == file.id;
                });

                fileWrapper& wrap = *itFileWrapper;
                wrap.isPresent = true;
                wrap.file = file;

                m_worker_UI->updateFileLoadingState(file.receiverLogin, wrap, false);
                return;
            }
        }
        else {
            // impossible
        }
    }

    Message* message = messageBlobsMap[file.blobUID];
    fileWrapper fileWrapper;
    fileWrapper.isPresent = true;
    fileWrapper.file = std::move(file);

    message->addRelatedFile(fileWrapper);

    if (message->getRelatedFilesCount() == file.filesInBlobCount) {
        auto& chatsMap = m_client->getMyChatsMap();
        auto it = chatsMap.find(file.receiverLogin);
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            chat->getMessagesVec().push_back(message);

            m_worker_UI->onMessageReceive(file.receiverLogin, message);
        }
        else {
            Chat* chat = new Chat;
            chat->setFriendLastSeen("online");
            chat->setFriendLogin(file.receiverLogin);
            auto& msgsVec = chat->getMessagesVec();
            msgsVec.push_back(message);

            utility::incrementAllChatLayoutIndexes(chatsMap);
            chat->setLayoutIndex(0);

            chatsMap[file.receiverLogin] = chat;

            m_client->requestFriendInfoFromServer(file.receiverLogin);
        }
    }
}

void ResponseHandler::onFilePreview(const std::string& packet) {
    std::istringstream iss(packet);

    std::string friendLogin;
    std::getline(iss, friendLogin);

    std::string myLogin;
    std::getline(iss, myLogin);

    std::string fileName;
    std::getline(iss, fileName);

    std::string fileId;
    std::getline(iss, fileId);

    std::string fileSize;
    std::getline(iss, fileSize);

    std::string fileTimestamp;
    std::getline(iss, fileTimestamp);

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
    if (!caption.empty()) {
        caption.pop_back();
    }

    std::string filesInBlobCountStr;
    std::getline(iss, filesInBlobCountStr);
    size_t filesInBlobCount = std::stoi(filesInBlobCountStr);

    std::string blobUID;
    std::getline(iss, blobUID);

    auto& messageBlobsMap = m_client->getMapMessageBlobs();

    Message* msgFile = new Message();
    msgFile->setIsRead(false);
    msgFile->setIsSend(false);
    msgFile->setMessage(caption);
    msgFile->setId(blobUID);
    msgFile->setTimestamp(fileTimestamp);

    net::file<QueryType> file;
    file.blobUID = blobUID;
    file.caption = caption;
    file.filePath = fileName;
    file.filesInBlobCount = filesInBlobCount;
    file.fileSize = std::stoi(fileSize);
    file.id = fileId;
    file.receiverLogin = friendLogin;
    file.senderLogin = myLogin;
    file.timestamp = fileTimestamp;


    fileWrapper fileWrapper;
    fileWrapper.isPresent = false;
    fileWrapper.file = std::move(file);

    msgFile->addRelatedFile(fileWrapper);


    if (messageBlobsMap.contains(blobUID)) {}
    else if (filesInBlobCount == 1) {
        auto& chatsMap = m_client->getMyChatsMap();
        auto it = chatsMap.find(friendLogin);
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            chat->getMessagesVec().push_back(msgFile);
            m_worker_UI->onMessageReceive(friendLogin, msgFile);
        }
        else {
            Chat* chat = new Chat;
            chat->setFriendLastSeen("online");
            chat->setFriendLogin(friendLogin);
            auto& msgsVec = chat->getMessagesVec();
            msgsVec.push_back(msgFile);

            utility::incrementAllChatLayoutIndexes(chatsMap);
            chat->setLayoutIndex(0);

            chatsMap[friendLogin] = chat;

            m_client->requestFriendInfoFromServer(friendLogin);
        }

    }
    else {
        messageBlobsMap.emplace(blobUID, msgFile);
    }

    std::string filePath = utility::getFileSavePath(fileName);
}

void ResponseHandler::prepareToReceiveRequestedFile(const std::string& packet) {
    prepareToReceiveFile(packet);
    m_is_received_file_requested = true;
}

void ResponseHandler::prepareToReceiveFile(const std::string& packet) {
    std::istringstream iss(packet);

    std::string myLogin;
    std::getline(iss, myLogin);

    std::string friendLogin;
    std::getline(iss, friendLogin);

    std::string fileName;
    std::getline(iss, fileName);

    std::string fileId;
    std::getline(iss, fileId);

    std::string fileSize;
    std::getline(iss, fileSize);

    std::string fileTimestamp;
    std::getline(iss, fileTimestamp);

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
    if (!caption.empty()) {
        caption.pop_back(); 
    }

    std::string filesInBlobCountStr;
    std::getline(iss, filesInBlobCountStr);
    size_t filesInBlobCount = std::stoi(filesInBlobCountStr);

    std::string blobUID;
    std::getline(iss, blobUID);

    auto& messageBlobsMap = m_client->getMapMessageBlobs();

    Message* msgFile = new Message();
    msgFile->setIsRead(false);
    msgFile->setIsSend(false);
    msgFile->setMessage(caption);
    msgFile->setId(blobUID);
    msgFile->setTimestamp(fileTimestamp);

    if (messageBlobsMap.contains(blobUID)) {}
    else {
        messageBlobsMap.emplace(blobUID, msgFile);
    }

    std::string filePath = utility::getFileSavePath(fileName);

    m_client->supplyFileData(myLogin, friendLogin, filePath, fileName, fileId, std::stoi(fileSize), fileTimestamp, caption, blobUID, filesInBlobCount);
}


void ResponseHandler::onRegistrationSuccess() {
    const std::string& myLogin = m_client->getMyLogin();
    m_client->initDatabase(myLogin);
    m_client->setIsNeedToAutoLogin(true);
    m_client->setNeedToUndoAutoLogin(false);

    m_worker_UI->onRegistrationSuccess();
}

void ResponseHandler::onRegistrationFail() {
    m_worker_UI->onRegistrationFail();
}



void ResponseHandler::onAuthorizationSuccess() {
    if (m_client->isAutoLogin() != true) {
        const std::string& myLogin = m_client->getMyLogin();
        m_client->initDatabase(myLogin);

        bool res = m_client->load(myLogin + ".json");
        if (!res) {
            m_client->requestFriendInfoFromServer(myLogin);
            m_worker_UI->showConfigLoadErrorDialog();
        }

        m_client->setIsNeedToAutoLogin(true);
    }
    
    m_client->setNeedToUndoAutoLogin(false);
    m_worker_UI->onAuthorizationSuccess();
}

void ResponseHandler::onAuthorizationFail() {
    m_worker_UI->onAuthorizationFail();
}


void ResponseHandler::processFoundUsers(const std::string& packet) {
    std::istringstream iss(packet);

    std::string countStr;
    std::getline(iss, countStr);
    size_t count = std::stoi(countStr);

    std::vector<FriendInfo*> vec;
    vec.reserve(count);

    for (int i = 0; i < count; i++) {
        FriendInfo* user = new FriendInfo();

        std::string login;
        std::getline(iss, login);
        user->setFriendLogin(login);

        std::string name;
        std::getline(iss, name);
        user->setFriendName(name);

        std::string lastSeen;
        std::getline(iss, lastSeen);
        user->setFriendLastSeen(lastSeen);

        std::string isHasPhotoStr;
        std::getline(iss, isHasPhotoStr);
        bool isHasPhoto = isHasPhotoStr == "true";
        user->setIsFriendHasPhoto(isHasPhoto);

        std::string sizeStr;
        std::getline(iss, sizeStr);

        std::string photoStr;
        std::getline(iss, photoStr);
        Photo* photo = Photo::deserializeWithoutSaveOnDisc(base64_decode(photoStr));
        user->setFriendPhoto(photo);

        vec.emplace_back(user);
    }
    
    m_worker_UI->processFoundUsers(std::move(vec));
}


void ResponseHandler::onChatCreateSuccess(const std::string& packet) {
    std::istringstream iss(packet);

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

    Photo* photo = Photo::deserializeAndSaveOnDisc(base64_decode(photoStr), login);
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

    if (!m_client->getIsHidden()) {
        m_client->broadcastMyStatus("online");
    }
    m_worker_UI->updateFriendsStatuses(loginToStatusPairsVec);
}



void ResponseHandler::onMessageReceive(const std::string& packet) {
    std::istringstream iss(packet);

    std::string myLogin;
    std::getline(iss, myLogin);

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
            message += '\n';
        }
    }
    message.pop_back();

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

    std::string newLogin;
    std::getline(iss, newLogin);

    if (newLogin != "") {
        auto& chatsMap = m_client->getMyChatsMap();

        auto it = chatsMap.find(login);
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            chat->setFriendLogin(newLogin);

            auto node = chatsMap.extract(it);
            node.key() = newLogin;
            
            chatsMap.insert(std::move(node));
        }
        m_client->updateInConfigFriendLogin(login, newLogin);
    }

    Photo* photo = Photo::deserializeAndSaveOnDisc(base64_decode(photoStr), login);

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

void ResponseHandler::onTyping(const std::string& packet) {
    std::istringstream iss(packet);

    std::string myLogin;
    std::getline(iss, myLogin);

    std::string friendLogin;
    std::getline(iss, friendLogin);

    std::string isTypingStr;
    std::getline(iss, isTypingStr);
    bool isTyping = isTypingStr == "1";

    if (isTyping) {
        m_worker_UI->showTypingLabel(friendLogin);
    }
    else {
        m_worker_UI->hideTypingLabel(friendLogin);
    }


}

void ResponseHandler::onMessageReadConfirmationReceive(const std::string& packet) {
    std::istringstream iss(packet);

    std::string myLogin;
    std::getline(iss, myLogin);

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

        if (chat->getFriendLastSeen() == "last seen: N/A") {
            chat->setFriendLastSeen("online");
            m_worker_UI->onStatusReceive(friendLogin, "online");
        }
    }
}



void ResponseHandler::onStatusReceive(const std::string& packet) {
    std::istringstream iss(packet);

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


void ResponseHandler::onPasswordVerifySuccess() {
    m_worker_UI->onPasswordVerifySuccess();

}

void ResponseHandler::onPasswordVerifyFail() {
    m_worker_UI->onPasswordVerifyFail();
}


void ResponseHandler::onCheckNewLoginSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string allowedLogin;
    std::getline(iss, allowedLogin);

    m_client->updateMyLogin(allowedLogin);

    m_worker_UI->onCheckNewLoginSuccess();
}

void ResponseHandler::onCheckNewLoginFail() {
    m_worker_UI->onCheckNewLoginFail();
}