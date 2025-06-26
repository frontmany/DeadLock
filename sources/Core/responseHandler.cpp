#include "responseHandler.h"
#include "workerUI.h"
#include "chatsWidget.h"
#include "configManager.h"
#include "friendInfo.h"
#include "client.h"
#include "utility.h"
#include "queryType.h"
#include "message.h"
#include "photo.h"
#include "chat.h"
#include "net.h"

ResponseHandler::ResponseHandler(Client* client, std::shared_ptr<ConfigManager> configManager)
	: m_client(client), m_worker_UI(nullptr), m_configManager(configManager) {}

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
        onRegistrationSuccess(packet);
    }
    else if (msg.header.type == QueryType::REGISTRATION_FAIL) {
        onRegistrationFail();
    }
    else if (msg.header.type == QueryType::AUTHORIZATION_SUCCESS) {
        onAuthorizationSuccess(packet);
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
        onFoundUsers(packet);
    }
    else if (msg.header.type == QueryType::TYPING) {
        onTyping(packet);
    }

    // new
    else if (msg.header.type == QueryType::FILE_PREVIEW) {
        onFilePreview(packet);
    }
}

void ResponseHandler::onRegistrationSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptionPart;
    std::getline(iss, encryptionPart);

    std::string serverPublicKey;
    std::getline(iss, serverPublicKey);

    m_client->buildSpecialServerKey(encryptionPart);
    m_client->setServerPublicKey(utility::deserializePublicKey(serverPublicKey));
    m_client->initDatabase(m_configManager->getMyLoginHash());
    m_client->connectFilesSocket(m_configManager->getMyLoginHash(), m_client->getServerIpAddress(), m_client->geServerPort());

    m_client->afterRegistrationSendMyInfo();
    m_client->generateMyKeyPair();
    m_client->sendPublicKeyToServer();

    m_configManager->setIsNeedToAutoLogin(true);
    m_worker_UI->onRegistrationSuccess();
}

void ResponseHandler::onRegistrationFail() {
    m_worker_UI->onRegistrationFail();
}

void ResponseHandler::onAuthorizationFail() {
    m_worker_UI->onAuthorizationFail();
}

void ResponseHandler::onPasswordVerifySuccess() {
    m_worker_UI->onPasswordVerifySuccess();
}

void ResponseHandler::onPasswordVerifyFail() {
    m_worker_UI->onPasswordVerifyFail();
}

void ResponseHandler::onCheckNewLoginFail() {
    m_worker_UI->onCheckNewLoginFail();
}

void ResponseHandler::onChatCreateFail() {
    m_worker_UI->onChatCreateFail();
}

void ResponseHandler::handleFile(net::file<QueryType>& file) {
    auto& messageBlobsMap = m_client->getMapMessageBlobs();

    if (auto& vec = m_client->getRequestedFileIdsVec(); std::find(vec.begin(), vec.end(), file.id) != vec.end()) {
        auto& chatsMap = m_client->getMyHashChatsMap();
        auto it = chatsMap.find(file.senderLoginHash);
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

                m_worker_UI->updateFileLoadingState(file.senderLoginHash, wrap, false);
                return;
            }

            vec.erase(std::find(vec.begin(), vec.end(), file.id));
        }
        else {
            std::filesystem::remove(file.filePath);
        }
    }

    Message* message = nullptr;
    if (messageBlobsMap.contains(file.blobUID)) {
        message = messageBlobsMap[file.blobUID];
    }
    else {
        message = new Message();
        message->setId(file.blobUID);
        messageBlobsMap.emplace(file.blobUID, message);
    }

    if (file.caption != "") {
        message->setMessage(file.caption);
    }

    std::string filesCountInBlob = file.filesInBlobCount;
    std::string friendLoginHash = file.senderLoginHash;

    fileWrapper fileWrapper;
    fileWrapper.isPresent = true;
    fileWrapper.file = std::move(file);

    message->addRelatedFile(fileWrapper);

    if (message->getRelatedFilesCount() == std::stoi(filesCountInBlob)) {
        auto& chatsMap = m_client->getMyHashChatsMap();
        auto it = chatsMap.find(friendLoginHash);
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            chat->getMessagesVec().push_back(message);

            m_worker_UI->onMessageReceive(friendLoginHash, message);
        }
        else {
            Chat* chat = new Chat;
            chat->setFriendLastSeen("online");
            auto& msgsVec = chat->getMessagesVec();
            msgsVec.push_back(message);

            utility::incrementAllChatLayoutIndexes(chatsMap);
            chat->setLayoutIndex(0);

            chatsMap[friendLoginHash] = chat;

            m_client->requestUserInfoFromServer(friendLoginHash);
        }
    }
}

void ResponseHandler::onFilePreview(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string fileId;
    std::getline(iss, fileId);

    std::string blobUID;
    std::getline(iss, blobUID);

    std::string myLoginHash;
    std::getline(iss, myLoginHash);
    myLoginHash = utility::AESDecrypt(key, myLoginHash);

    std::string friendLoginHash;
    std::getline(iss, friendLoginHash);
    friendLoginHash = utility::AESDecrypt(key, friendLoginHash);

    std::string fileName;
    std::getline(iss, fileName);
    fileName = utility::AESDecrypt(key, fileName);

    std::string fileSize;
    std::getline(iss, fileSize);
    fileSize = utility::AESDecrypt(key, fileSize);

    std::string fileTimestamp;
    std::getline(iss, fileTimestamp);
    fileTimestamp = utility::AESDecrypt(key, fileTimestamp);

    std::string messageBegin;
    std::getline(iss, messageBegin);
    messageBegin = utility::AESDecrypt(key, messageBegin);

    std::string caption;
    std::getline(iss, caption);
    caption = utility::AESDecrypt(key, caption);

    std::string messageEnd;
    std::getline(iss, messageEnd);
    messageEnd = utility::AESDecrypt(key, messageEnd);

    std::string filesInBlobCountStr;
    std::getline(iss, filesInBlobCountStr);
    filesInBlobCountStr = utility::AESDecrypt(key, filesInBlobCountStr);
    uint32_t filesInBlobCount = static_cast<uint32_t>(std::stoul(filesInBlobCountStr));

    auto& messageBlobsMap = m_client->getMapMessageBlobs();

    Message* msgFile = nullptr;
    if (messageBlobsMap.contains(blobUID)) {
        msgFile = messageBlobsMap[blobUID];
    }
    else {
        msgFile = new Message();
        msgFile->setId(blobUID);
        messageBlobsMap.emplace(blobUID, msgFile);
    }

    msgFile->setIsRead(false);
    msgFile->setIsSend(false);
    msgFile->setMessage(caption);
    msgFile->setId(blobUID);
    msgFile->setTimestamp(fileTimestamp);

    net::file<QueryType> file;
    file.blobUID = blobUID;
    file.caption = caption;
    file.filePath = "";
    file.fileName = fileName;
    file.filesInBlobCount = filesInBlobCount;
    file.fileSize = std::stoi(fileSize);
    file.id = fileId;
    file.receiverLoginHash = myLoginHash;
    file.senderLoginHash = friendLoginHash;
    file.timestamp = fileTimestamp;

    

    fileWrapper fileWrapper;
    fileWrapper.isPresent = false;
    fileWrapper.file = std::move(file);

    msgFile->addRelatedFile(fileWrapper);


    if (msgFile->getRelatedFilesCount() == filesInBlobCount) {
        auto& chatsMap = m_client->getMyHashChatsMap();
        auto it = chatsMap.find(utility::calculateHash(friendLoginHash));
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            chat->getMessagesVec().push_back(msgFile);
            m_worker_UI->onMessageReceive(friendLoginHash, msgFile);
        }
        else {
            Chat* chat = new Chat;
            chat->setFriendLastSeen("online");
            auto& msgsVec = chat->getMessagesVec();
            msgsVec.push_back(msgFile);

            utility::incrementAllChatLayoutIndexes(chatsMap);
            chat->setLayoutIndex(0);

            chatsMap[friendLoginHash] = chat;

            m_client->requestUserInfoFromServer(friendLoginHash);
        }
    }
}

void ResponseHandler::onAuthorizationSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptionPart;
    std::getline(iss, encryptionPart);

    std::string serverPublicKey;
    std::getline(iss, serverPublicKey);

    m_client->buildSpecialServerKey(encryptionPart);
    m_client->setServerPublicKey(utility::deserializePublicKey(serverPublicKey));

    const std::string& myLoginHash = m_configManager->getMyLoginHash();
    m_client->initDatabase(myLoginHash);

    if (!m_configManager->getIsAutoLogin()) {
        bool res = m_configManager->load((myLoginHash + ".json"), m_client->getSpecialServerKey(), m_client->getDatabase());
        if (!res) {
            m_client->requestUserInfoFromServer(myLoginHash);
            m_worker_UI->showConfigLoadErrorDialog();
        }
        m_configManager->setIsNeedToAutoLogin(true);
    }

    m_client->connectFilesSocket(myLoginHash, m_client->getServerIpAddress(), m_client->geServerPort());
    m_worker_UI->onAuthorizationSuccess();
}

void ResponseHandler::onFoundUsers(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string countStr;
    std::getline(iss, countStr);
    countStr = utility::AESDecrypt(key, countStr);
    size_t count = std::stoi(countStr);

    std::vector<FriendInfo*> vec;
    vec.reserve(count);

    for (int i = 0; i < count; i++) {
        FriendInfo* user = new FriendInfo();

        std::string login;
        std::getline(iss, login);
        login = utility::AESDecrypt(key, login);
        user->setFriendLogin(login);

        std::string name;
        std::getline(iss, name);
        name = utility::AESDecrypt(key, name);
        user->setFriendName(name);

        std::string lastSeen;
        std::getline(iss, lastSeen);
        lastSeen = utility::AESDecrypt(key, lastSeen);
        user->setFriendLastSeen(lastSeen);

        std::string isHasPhotoStr;
        std::getline(iss, isHasPhotoStr);
        isHasPhotoStr = utility::AESDecrypt(key, isHasPhotoStr);
        bool isHasPhoto = isHasPhotoStr == "true";
        user->setIsFriendHasPhoto(isHasPhoto);

        std::string sizeStr;
        std::getline(iss, sizeStr);
        sizeStr = utility::AESDecrypt(key, sizeStr);

        std::string photoStr;
        std::getline(iss, photoStr);
        photoStr = utility::AESDecrypt(key, photoStr);
        Photo* photo = Photo::deserializeWithoutSaveOnDisc(base64_decode(photoStr));
        user->setFriendPhoto(photo);

        vec.emplace_back(user);
    }
    
    m_worker_UI->processFoundUsers(std::move(vec));
}

void ResponseHandler::onChatCreateSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string login;
    std::getline(iss, login);
    login = utility::AESDecrypt(key, login);

    std::string name;
    std::getline(iss, name);
    name = utility::AESDecrypt(key, name);

    std::string isHasPhoto;
    std::getline(iss, isHasPhoto);
    isHasPhoto = utility::AESDecrypt(key, isHasPhoto);

    std::string sizeStr;
    std::getline(iss, sizeStr);
    size_t size = std::stoi(sizeStr);
    sizeStr = utility::AESDecrypt(key, sizeStr);

    std::string lastSeen;
    std::getline(iss, lastSeen);
    lastSeen = utility::AESDecrypt(key, lastSeen);

    std::string photoStr;
    std::getline(iss, photoStr);
    photoStr = utility::AESDecrypt(key, photoStr);


    Chat* chat = new Chat;
    chat->setFriendLogin(login);
    chat->setFriendName(name);
    chat->setIsFriendHasPhoto(isHasPhoto == "true");
    chat->setLayoutIndex(0);

    utility::incrementAllChatLayoutIndexes(m_client->getMyHashChatsMap());

    Photo* photo = Photo::deserializeAndSaveOnDisc(base64_decode(photoStr), login);
    chat->setFriendPhoto(photo);
    chat->setFriendLastSeen(lastSeen);
    chat->setLastReceivedOrSentMessage("no messages yet");

    m_client->getMyHashChatsMap().emplace(utility::calculateHash(login), chat);

    m_worker_UI->onChatCreateSuccess(chat);
}

void ResponseHandler::processFriendsStatusesSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string vecBegin;
    std::getline(iss, vecBegin);

    auto& chatsMap = m_client->getMyHashChatsMap();

    std::vector<std::pair<std::string, std::string>> loginToStatusPairsVec;
    std::string line;
    while (std::getline(iss, line)) {
        line = utility::AESDecrypt(key, line);
        if (line != "VEC_END") {
            int index = line.find(',');
            std::string login = line.substr(0, index);
            std::string status = line.substr(++index);
            loginToStatusPairsVec.emplace_back(std::make_pair(utility::calculateHash(login), status));

            auto it = chatsMap.find(utility::calculateHash(login));
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

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string myLogin;
    std::getline(iss, myLogin);
    myLogin = utility::AESDecrypt(key, myLogin);

    std::string friendLogin;
    std::getline(iss, friendLogin);
    friendLogin = utility::AESDecrypt(key, friendLogin);

    std::string messageBegin;
    std::getline(iss, messageBegin);
    messageBegin = utility::AESDecrypt(key, messageBegin);

    std::string message;
    std::getline(iss, message);
    message = utility::AESDecrypt(key, message);

    std::string messageEnd;
    std::getline(iss, messageEnd);
    messageEnd = utility::AESDecrypt(key, messageEnd);

    std::string id;
    std::getline(iss, id);

    std::string timestamp;
    std::getline(iss, timestamp);
    timestamp = utility::AESDecrypt(key, timestamp);
    


    Message* msg = new Message(message, timestamp, id, false);
    msg->setIsRead(false);

    auto& chatsMap = m_client->getMyHashChatsMap();

    auto chatPairIt = chatsMap.find(utility::calculateHash(friendLogin));
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

        m_client->requestUserInfoFromServer(utility::calculateHash(friendLogin));
    }
}

void ResponseHandler::onUserInfo(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string login;
    std::getline(iss, login);
    login = utility::AESDecrypt(key, login);

    std::string name;
    std::getline(iss, name);
    name = utility::AESDecrypt(key, name);

    std::string lastSeen;
    std::getline(iss, lastSeen);
    lastSeen = utility::AESDecrypt(key, lastSeen);

    std::string isHasPhotoStr;
    std::getline(iss, isHasPhotoStr);
    isHasPhotoStr = utility::AESDecrypt(key, isHasPhotoStr);
    bool isHasPhoto = isHasPhotoStr == "true";

    std::string sizeStr;
    std::getline(iss, sizeStr);
    sizeStr = utility::AESDecrypt(key, sizeStr);

    std::string photoStr;
    std::getline(iss, photoStr);
    photoStr = utility::AESDecrypt(key, photoStr);

    std::string newLogin;
    std::getline(iss, newLogin);
    newLogin = utility::AESDecrypt(key, newLogin);

    if (newLogin != "") {
        auto& chatsMap = m_client->getMyHashChatsMap();

        auto it = chatsMap.find(utility::calculateHash(login));
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            chat->setFriendLogin(newLogin);

            auto node = chatsMap.extract(it);
            node.key() = newLogin;
            
            chatsMap.insert(std::move(node));
        }
        m_configManager->updateInConfigFriendLogin(login, newLogin);
    }

    Photo* photo = Photo::deserializeAndSaveOnDisc(base64_decode(photoStr), login);

    auto& chatsMap = m_client->getMyHashChatsMap();
    const auto it = chatsMap.find(utility::calculateHash(login));

    if (it != chatsMap.end()) {
        Chat* chat = it->second;
        chat->setFriendLogin(login);
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

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string myLogin;
    std::getline(iss, myLogin);
    myLogin = utility::AESDecrypt(key, myLogin);

    std::string friendLogin;
    std::getline(iss, friendLogin);
    friendLogin = utility::AESDecrypt(key, friendLogin);

    std::string isTypingStr;
    std::getline(iss, isTypingStr);
    isTypingStr = utility::AESDecrypt(key, isTypingStr);
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

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string myLogin;
    std::getline(iss, myLogin);
    myLogin = utility::AESDecrypt(key, myLogin);

    std::string friendLogin;
    std::getline(iss, friendLogin);
    friendLogin = utility::AESDecrypt(key, friendLogin);

    std::string id;
    std::getline(iss, id);

    auto& chatsMap = m_client->getMyHashChatsMap();
    auto chatPair = chatsMap.find(utility::calculateHash(friendLogin));

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

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string friendLoginHash;
    std::getline(iss, friendLoginHash);

    std::string status;
    std::getline(iss, status);
    status = utility::AESDecrypt(key, status);

    auto& chatsMap = m_client->getMyHashChatsMap();
    auto chatPair = chatsMap.find(friendLoginHash);
    if (chatPair != chatsMap.end()) {
        Chat* chat = chatPair->second; 
        chat->setFriendLastSeen(status);

        m_worker_UI->onStatusReceive(friendLoginHash, status);
    }
}

void ResponseHandler::onCheckNewLoginSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecrypt(m_client->getPrivateKey(), encryptedKey);

    std::string allowedLogin;
    std::getline(iss, allowedLogin);
    allowedLogin = utility::AESDecrypt(key, allowedLogin);

    m_client->updateMyLogin(allowedLogin);

    m_worker_UI->onCheckNewLoginSuccess();
}

