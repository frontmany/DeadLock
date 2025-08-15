#include "responseHandler.h"
#include "workerUI.h"
#include "chatsWidget.h"
#include "configManager.h"
#include "friendInfo.h"
#include "client.h"
#include "utility.h"
#include "packetType.h"
#include "database.h"
#include "message.h"
#include "avatar.h"
#include "chat.h"
#include "net.h"

#include "net_packet.h"

ResponseHandler::ResponseHandler(Client* client, std::shared_ptr<ConfigManager> configManager)
	: m_client(client), m_worker_UI(nullptr), m_configManager(configManager) {}

void ResponseHandler::setWorkerUI(WorkerUI* workerImpl) {
    m_worker_UI = workerImpl;
}

void ResponseHandler::handleResponse(net::Packet& packet) {
    if (msg.header.type != static_cast<uint32_t>(QueryType::REGISTRATION_SUCCESS) &&
        msg.header.type != static_cast<uint32_t>(QueryType::AUTHORIZATION_SUCCESS) &&
        msg.header.type != static_cast<uint32_t>(QueryType::REGISTRATION_FAIL) &&
        msg.header.type != static_cast<uint32_t>(QueryType::AVATARS_KEY) &&
        msg.header.type != static_cast<uint32_t>(QueryType::AUTHORIZATION_FAIL)) 
    {
        m_client->waitUntilUIReadyToUpdate();
    }

    std::string packet = "";
    if (msg.body.size() > 0) {
        msg >> packet;
    }


    if (msg.header.type == static_cast<uint32_t>(QueryType::REGISTRATION_SUCCESS)) {
        onRegistrationSuccess(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::REGISTRATION_FAIL)) {
        onRegistrationFail();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::AUTHORIZATION_SUCCESS)) {
        onAuthorizationSuccess(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::AUTHORIZATION_FAIL)) {
        onAuthorizationFail();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::CHAT_CREATE_SUCCESS)) {
        onChatCreateSuccess(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::CHAT_CREATE_FAIL)) {
        onChatCreateFail();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::FRIENDS_STATUSES)) {
        processFriendsStatusesSuccess(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::MESSAGE)) {
        onMessageReceive(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::USER_INFO_SUCCESS)) {
        onUserInfoSuccess(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::USER_INFO_FAIL)) {
        onUserInfoFail(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::MY_INFO)) {
        onMyInfo(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::MESSAGES_READ_CONFIRMATION)) {
        onMessageReadConfirmationReceive(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::STATUS)) {
        onStatusReceive(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::VERIFY_PASSWORD_FAIL)) {
        onPasswordVerifyFail();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::VERIFY_PASSWORD_SUCCESS)) {
        onPasswordVerifySuccess();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::NEW_LOGIN_SUCCESS)) {
        onCheckNewLoginSuccess(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::NEW_LOGIN_FAIL)) {
        onPasswordVerifySuccess();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::ALL_PENDING_MESSAGES_WERE_SENT)) {
        m_client->getAllFriendsStatuses();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::FIND_USER_RESULTS)) {
        onFoundUsers(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::TYPING)) {
        onTyping(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::FILE_PREVIEW)) {
        onFilePreview(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::UPDATE_OFFER)) {
        onUpdateOffer(packet);
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::RECONNECT_SUCCESS)) {
        onReconnectSuccess();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::RECONNECT_FAIL)) {
        onReconnectFail();
    }
    else if (msg.header.type == static_cast<uint32_t>(QueryType::AVATARS_KEY)) {
        onAvatarsKey(packet);
    }
}

void ResponseHandler::onAvatarsKey(const std::string& packet) {
    std::istringstream iss(packet);

    std::string avatarsKey;
    std::getline(iss, avatarsKey);
    m_client->setAvatarsKey(utility::deserializeAESKey(avatarsKey));

    m_client->initDatabase(m_configManager->getMyLoginHash());

    if (m_client->getIsFirstAuthentication()) {
        m_worker_UI->onRegistrationSuccess();
        m_client->createFilesConnection(m_configManager->getMyLoginHash(), m_client->getServerIpAddress(), m_client->geServerPort());
    }
    else {
        if (!m_configManager->getIsAutoLogin()) {
            bool res = m_configManager->load((m_configManager->getMyLoginHash() + ".json"), m_client->getSpecialServerKey(), m_client->getDatabase());
            if (!res) {
                m_client->requestMyInfoFromServerAndResetKeys(m_configManager->getMyLoginHash());
                m_client->setIsFirstAuthentication(false);
                m_worker_UI->initializeUIWithConfigErrorDialog();
                return;
            }
            else {
                m_worker_UI->supplyTheme(m_configManager->getIsDarkTheme());
                m_configManager->setIsNeedToAutoLogin(true);
            }
        }

        m_client->createFilesConnection(m_configManager->getMyLoginHash(), m_client->getServerIpAddress(), m_client->geServerPort());
        m_worker_UI->onAuthorizationSuccess();
    }
}

void ResponseHandler::onUpdateOffer(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string versionNumber;
    std::getline(iss, versionNumber);
    versionNumber = utility::AESDecrypt(key, versionNumber);
    
    m_configManager->setIsNeedToUpdate(true);
    m_configManager->setNewVersionNumber(versionNumber);
    m_worker_UI->showUpdateButton();
}

void ResponseHandler::onRegistrationSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptionPart;
    std::getline(iss, encryptionPart);

    std::string serverPublicKey;
    std::getline(iss, serverPublicKey);

    m_client->setServerEncryptionPart(encryptionPart);
    m_client->setServerPublicKey(utility::deserializePublicKey(serverPublicKey));
    
    m_client->generateMyKeyPair();
    m_client->sendPublicKeyToServer();

    m_configManager->setIsNeedToAutoLogin(true);
    m_client->setIsFirstAuthentication(true);
    m_client->setIsPassedAuthentication(true);

    m_client->afterRegistrationSendMyInfo();
}

void ResponseHandler::onRegistrationFail() {
    m_worker_UI->onRegistrationFail();
}

void ResponseHandler::onAuthorizationSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptionPart;
    std::getline(iss, encryptionPart);

    std::string serverPublicKey;
    std::getline(iss, serverPublicKey);

    m_client->setIsFirstAuthentication(false);
    m_client->setIsPassedAuthentication(true);
    m_client->setServerEncryptionPart(encryptionPart);
    m_client->setServerPublicKey(utility::deserializePublicKey(serverPublicKey));
}


void ResponseHandler::onAuthorizationFail() {
    m_configManager->setMyLoginHash("");
    m_configManager->setMyPasswordHash("");
    if (!m_configManager->getIsAutoLogin()) {
        m_worker_UI->onAuthorizationFail();
    }
    else {
        m_worker_UI->setupRegistrationWidget();
    }
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

namespace fs = std::filesystem;

void ResponseHandler::processNewVersionLoadedFile(net::File file) {
    const char* folderName = "updaterTemporary";
    fs::path folderPath = folderName;

    try {
        if (!fs::exists(folderPath)) {
            std::cout << "error unexisting folder " << folderName << std::endl;
            return;
        }

        fs::path versionsPath = folderPath / "versions.txt";

        std::ofstream versionsFile(versionsPath, std::ios::app);
        if (!versionsFile.is_open()) {
            std::cerr << "Couldn't open the file " << versionsPath << " for writing. \n";
            return;
        }

        versionsFile << file.fileName << '\n';
        versionsFile.close();
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "File system error: " << e.what() << std::endl;
    }

    m_worker_UI->updateAndRestart();
}

void ResponseHandler::onAvatar(net::File& file) {
    if (file.senderLoginHash == m_configManager->getMyLoginHash()) {
        Avatar* myAvatar = new Avatar(m_client->getAvatarsKey(), file.filePath);
        m_worker_UI->setRecoveredAvatar(myAvatar);
    }
    else {
        auto& chatsMap = m_client->getMyHashChatsMap();

        if (chatsMap.contains(file.senderLoginHash)) {
            auto chat = chatsMap.at(file.senderLoginHash);
            chat->setIsFriendHasAvatar(true);

            Avatar* friendAvatar = new Avatar(m_client->getAvatarsKey(), file.filePath);
            chat->setFriendAvatar(friendAvatar);

            m_worker_UI->updateFriendAvatar(friendAvatar, chat->getFriendLogin());
        }
    }
}

void ResponseHandler::onAvatarPreview(net::File& file) {
    Avatar* friendAvatar = new Avatar(m_client->getAvatarsKey(), file.filePath);
    m_worker_UI->updateFriendAvatarPreview(friendAvatar, file.senderLoginHash);
}

void ResponseHandler::onFile(net::File& file) {
    if (file.senderLoginHash == "server") {
        processNewVersionLoadedFile(file);
        return;
    }

    Database* database = m_client->getDatabase();

    if (auto vec = database->getRequestedFiles(file.receiverLoginHash); std::find(vec.begin(), vec.end(), file.id) != vec.end()) {
        processRequestedFile(file);
        return;
    }

    std::string myLoginHash = file.receiverLoginHash;
    std::string friendLoginHash = file.senderLoginHash;
    std::string blobUID = file.blobUID;
    int filesInBlobCount = std::stoi(file.filesInBlobCount);
    

    Message* message = nullptr;
    if (database->isBlobExists(myLoginHash, blobUID)) {
        std::string serializedMessage = database->getSerializedMessage(m_client->getPrivateKey(), myLoginHash, blobUID);
        message = Message::deserialize(serializedMessage);

        addDataToMessage(message, file, true);

        std::string newSerializedMessage = message->serialize();
        database->updateSerializedMessage(m_client->getPrivateKey(), myLoginHash, blobUID, newSerializedMessage);
        database->incrementFilesReceivedCounter(myLoginHash, blobUID);
    }
    else {
        message = new Message();

        addDataToMessage(message, file, true);

        std::string newSerializedMessage = message->serialize();
        database->addBlob(m_client->getPublicKey(), myLoginHash, blobUID, filesInBlobCount, 0, newSerializedMessage);
        database->incrementFilesReceivedCounter(myLoginHash, blobUID);
    }

    int receivedFilesCount = database->getReceivedFilesCount(myLoginHash, blobUID);
    if (filesInBlobCount != 1 && receivedFilesCount == 1) {
        m_worker_UI->showNowReceiving(friendLoginHash);
    }

    if (receivedFilesCount == filesInBlobCount) {
        database->removeBlob(myLoginHash, blobUID);
        showFilesMessage(message, friendLoginHash, myLoginHash);
    }

    m_client->setIsAbleToClose(true);
}

void ResponseHandler::onFilePreview(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string fileId;
    std::getline(iss, fileId);

    std::string blobUID;
    std::getline(iss, blobUID);

    std::string myLoginHash;
    std::getline(iss, myLoginHash);

    std::string friendLoginHash;
    std::getline(iss, friendLoginHash);

    std::string fileName;
    std::getline(iss, fileName);
    fileName = utility::AESDecrypt(key, fileName);

    std::string fileSize;
    std::getline(iss, fileSize);

    std::string fileTimestamp;
    std::getline(iss, fileTimestamp);
    fileTimestamp = utility::AESDecrypt(key, fileTimestamp);

    std::string caption;
    std::getline(iss, caption);
    if (caption != "") {
        caption = utility::AESDecrypt(key, caption);
    }

    std::string filesInBlobCount;
    std::getline(iss, filesInBlobCount);
    
    net::File file;
    file.blobUID = blobUID;
    file.caption = caption;
    file.filePath = "";
    file.fileName = fileName;
    file.filesInBlobCount = filesInBlobCount;
    file.fileSize = fileSize;
    file.id = fileId;
    file.receiverLoginHash = myLoginHash;
    file.senderLoginHash = friendLoginHash;
    file.timestamp = fileTimestamp;
    file.encryptedKey = encryptedKey;

    Database* database = m_client->getDatabase();

    Message* message = nullptr;
    if (database->isBlobExists(myLoginHash, blobUID)) {
        std::string serializedMessage = database->getSerializedMessage(m_client->getPrivateKey(), myLoginHash, blobUID);
        message = Message::deserialize(serializedMessage);

        
        auto& vec = message->getRelatedFiles();
        auto it = std::find_if(vec.begin(), vec.end(), [&fileId](fileWrapper wrap) { return wrap.file.id == fileId; });
        if (it == vec.end()) {
            addDataToMessage(message, file, false);
        }
        else {
            return;
        }

        database->updateSerializedMessage(m_client->getPrivateKey(), myLoginHash, blobUID, message->serialize());
        database->incrementFilesReceivedCounter(myLoginHash, blobUID);
    }
    else {
        message = new Message();
        message->setId(blobUID);

        addDataToMessage(message, file, false);

        database->addBlob(m_client->getPublicKey(), myLoginHash, blobUID, std::stoi(filesInBlobCount), 0, message->serialize());
        database->incrementFilesReceivedCounter(myLoginHash, blobUID);
    }

    int receivedFilesCount = database->getReceivedFilesCount(myLoginHash, blobUID);
    if (std::stoi(filesInBlobCount) != 1 && receivedFilesCount == 1) {
        m_worker_UI->showNowReceiving(friendLoginHash);
    }

    if (receivedFilesCount == std::stoi(filesInBlobCount)) {
        database->removeBlob(myLoginHash, blobUID);
        showFilesMessage(message, friendLoginHash, myLoginHash);
    }
}

void ResponseHandler::onReconnectSuccess() {
    m_client->reconnectFilesConnection(m_configManager->getMyLoginHash());
    m_worker_UI->removeConnectionErrorLabel();
}

void ResponseHandler::onReconnectFail() {
    m_worker_UI->setupRegistrationWidget();
}

void ResponseHandler::onFoundUsers(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string countStr;
    std::getline(iss, countStr);
    countStr = utility::AESDecrypt(key, countStr);
    size_t count = std::stoi(countStr);

    std::vector<FriendInfo*> vec;
    vec.reserve(count);

    for (int i = 0; i < count; i++) {
        FriendInfo* friendInfo = new FriendInfo();

        std::string login;
        std::getline(iss, login);
        login = utility::AESDecrypt(key, login);
        friendInfo->setFriendLogin(login);
        friendInfo->setFriendLoginHash(utility::calculateHash(login));

        std::string name;
        std::getline(iss, name);
        name = utility::AESDecrypt(key, name);
        friendInfo->setFriendName(name);

        std::string lastSeen;
        std::getline(iss, lastSeen);
        lastSeen = utility::AESDecrypt(key, lastSeen);
        friendInfo->setFriendLastSeen(lastSeen);

        std::string isHasAvatarStr;
        std::getline(iss, isHasAvatarStr);
        isHasAvatarStr = utility::AESDecrypt(key, isHasAvatarStr);
        bool isHasAvatar = isHasAvatarStr == "true";
        friendInfo->setIsFriendHasAvatar(isHasAvatar);

        std::string friendPublicKeyStr;
        std::getline(iss, friendPublicKeyStr);
        auto friendPublicKey = utility::deserializePublicKey(friendPublicKeyStr);
        friendInfo->setPublicKey(friendPublicKey);
        
        vec.emplace_back(friendInfo);
    }
    
    m_worker_UI->processFoundUsers(std::move(vec));
}

void ResponseHandler::onChatCreateSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string login;
    std::getline(iss, login);
    login = utility::AESDecrypt(key, login);

    std::string name;
    std::getline(iss, name);
    name = utility::AESDecrypt(key, name);

    std::string isHasPhotoStr;
    std::getline(iss, isHasPhotoStr);
    isHasPhotoStr = utility::AESDecrypt(key, isHasPhotoStr);
    bool isHasPhoto = (isHasPhotoStr == "true");

    std::string lastSeen;
    std::getline(iss, lastSeen);
    lastSeen = utility::AESDecrypt(key, lastSeen);

    std::string friendPublicKeyStr;
    std::getline(iss, friendPublicKeyStr);
    auto friendPublicKey = utility::deserializePublicKey(friendPublicKeyStr);

    Chat* chat = new Chat;
    chat->setFriendLogin(login);
    chat->setFriendName(name);
    chat->setIsFriendHasAvatar(isHasPhoto);
    chat->setLayoutIndex(0);
    chat->setPublicKey(friendPublicKey);

    utility::incrementAllChatLayoutIndexes(m_client->getMyHashChatsMap());

    chat->setFriendLastSeen(lastSeen);
    chat->setLastReceivedOrSentMessage("no messages yet");

    if (!m_client->getMyHashChatsMap().contains(utility::calculateHash(login))) {
        m_client->getMyHashChatsMap().emplace(utility::calculateHash(login), chat);
    }
    else {
        return;
    }

    m_configManager->save(
        m_client->getPublicKey(),
        m_client->getPrivateKey(),
        m_client->getSpecialServerKey(),
        m_client->getMyHashChatsMap(),
        m_client->getIsHidden(),
        m_client->getDatabase()
    );

    m_worker_UI->onChatCreateSuccess(chat);
    
}

void ResponseHandler::processFriendsStatusesSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string vecBegin;
    std::getline(iss, vecBegin);

    auto& chatsMap = m_client->getMyHashChatsMap();

    std::vector<std::pair<std::string, std::string>> loginHashToStatusPairsVec;
    std::string line;
    while (std::getline(iss, line)) {
        if (line != "VEC_END") {
            line = utility::AESDecrypt(key, line);
            int index = line.find(',');
            std::string loginHash = line.substr(0, index);
            std::string status = line.substr(++index);
            loginHashToStatusPairsVec.emplace_back(std::make_pair(loginHash, status));

            auto it = chatsMap.find(loginHash);
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
    m_worker_UI->updateFriendsStatuses(loginHashToStatusPairsVec);
}

void ResponseHandler::onMessageReceive(const std::string& packet) {
    std::istringstream iss(packet);

    std::string myLoginHash;
    std::getline(iss, myLoginHash);

    std::string friendLoginHash;
    std::getline(iss, friendLoginHash);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string id;
    std::getline(iss, id);

    std::string message;
    std::getline(iss, message);
    message = utility::AESDecrypt(key, message);

    std::string timestamp;
    std::getline(iss, timestamp);
    timestamp = utility::AESDecrypt(key, timestamp);

    Message* msg = new Message(message, timestamp, id, false);
    msg->setIsRead(false);

    auto& chatsHashMap = m_client->getMyHashChatsMap();

    auto chatPairIt = chatsHashMap.find(friendLoginHash);
    std::pair<std::string, Chat*> chatPair;
    if (chatPairIt != chatsHashMap.end()) {
        chatPair = *chatPairIt;

        Chat* chat = chatPair.second;
        chat->setLastReceivedOrSentMessage(msg->getMessage());
        chat->getMessagesVec().push_back(msg);

        m_worker_UI->onMessageReceive(friendLoginHash, msg);
    }
    else {
        Chat* chat = new Chat;
        auto& msgsVec = chat->getMessagesVec();
        msgsVec.push_back(msg);

        utility::incrementAllChatLayoutIndexes(chatsHashMap);
        chat->setLayoutIndex(0);

        chatsHashMap[friendLoginHash] = chat;

        m_client->requestUserInfoFromServer(friendLoginHash, myLoginHash);
    }
}

void ResponseHandler::onUserInfoSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

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

    std::string friendPublicKeyStr;
    std::getline(iss, friendPublicKeyStr);
    auto friendPublicKey = utility::deserializePublicKey(friendPublicKeyStr);

    std::string newLogin;
    std::getline(iss, newLogin);

    auto& chatsMap = m_client->getMyHashChatsMap();
    const auto it = chatsMap.find(utility::calculateHash(login));

    Chat* chat = nullptr;
    if (it != chatsMap.end()) {
        chat = it->second;
    }
    else {
        std::cout << "unExisting chat (onUserInfoSuccess)\n";
        return;
    }

    chat->setFriendLogin(login);
    chat->setFriendName(name);
    chat->setFriendLastSeen(lastSeen);
    chat->setIsFriendHasAvatar(isHasPhoto);
    chat->setPublicKey(friendPublicKey);

    if (chat->getMessagesVec().size() == 0) {
        chat->setLastReceivedOrSentMessage("no messages yet");
    }
    else {
        chat->setLastReceivedOrSentMessage(chat->getMessagesVec().back()->getMessage());
    }

    if (newLogin != "") {
        newLogin = utility::AESDecrypt(key, newLogin);

        auto& chatsMap = m_client->getMyHashChatsMap();

        auto it = chatsMap.find(utility::calculateHash(login));
        if (it != chatsMap.end()) {
            Chat* chat = it->second;
            chat->setFriendLogin(newLogin);

            auto node = chatsMap.extract(it);
            node.key() = utility::calculateHash(newLogin);
            
            chatsMap.insert(std::move(node));

            Avatar::rename(utility::calculateHash(login) + ".dph", utility::calculateHash(newLogin) + ".dph");
        }

        m_configManager->updateInConfigFriendLogin(login, newLogin);
    }

    m_worker_UI->showNewChatOrUpdateExisting(chat);
}

void ResponseHandler::onUserInfoFail(const std::string& packet) {
    std::cout << "user info was not found";
}

void ResponseHandler::onMyInfo(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string login;
    std::getline(iss, login);
    login = utility::AESDecrypt(key, login);

    std::string name;
    std::getline(iss, name);
    name = utility::AESDecrypt(key, name);

    std::string lastSeen;
    std::getline(iss, lastSeen);
    lastSeen = utility::AESDecrypt(key, lastSeen);

    std::string isHasAvatarStr;
    std::getline(iss, isHasAvatarStr);
    isHasAvatarStr = utility::AESDecrypt(key, isHasAvatarStr);
    bool isHasAvatar = isHasAvatarStr == "true";

    m_configManager->setIsHasAvatar(isHasAvatar);
    m_configManager->setIsNeedToAutoLogin(true);
    m_client->setIsPassedAuthentication(true);
    m_configManager->setMyLogin(login);
    m_configManager->setMyName(name);

    m_worker_UI->setNameFieldInProfileEditorWidget(name);
}

void ResponseHandler::onTyping(const std::string& packet) {
    std::istringstream iss(packet);

    std::string myLoginHash;
    std::getline(iss, myLoginHash);

    std::string friendLoginHash;
    std::getline(iss, friendLoginHash);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string isTypingStr;
    std::getline(iss, isTypingStr);
    isTypingStr = utility::AESDecrypt(key, isTypingStr);
    bool isTyping = isTypingStr == "1";

    auto& chatsHashMap = m_client->getMyHashChatsMap();
    auto chatPairIt = chatsHashMap.find(friendLoginHash);
    if (chatPairIt != chatsHashMap.end()) {
        auto& [friendLoginHash, chat] = *chatPairIt;

        std::string friendLogin = chat->getFriendLogin();
        if (isTyping) {
            m_worker_UI->showTypingLabel(friendLogin);
        }
        else {
            m_worker_UI->hideTypingLabel(friendLogin);
        }
    }
}

void ResponseHandler::onMessageReadConfirmationReceive(const std::string& packet) {
    std::istringstream iss(packet);

    std::string myLoginHash;
    std::getline(iss, myLoginHash);

    std::string friendLoginHash;
    std::getline(iss, friendLoginHash);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string id;
    std::getline(iss, id);
    id = utility::AESDecrypt(key, id);

    auto& chatsHashMap = m_client->getMyHashChatsMap();
    auto chatPair = chatsHashMap.find(friendLoginHash);

    if (chatPair != chatsHashMap.end()) {
        Chat* chat = chatPair->second;

        auto& messagesVec = chat->getMessagesVec();
        auto msgChatIt = std::find_if(messagesVec.begin(), messagesVec.end(), [&id](Message* msg) {
            return msg->getId() == id;
        });

        Message* msg = *msgChatIt;
        msg->setIsRead(true);

        std::string friendLogin = chat->getFriendLogin();
        m_worker_UI->onMessageReadConfirmationReceive(friendLogin, id);
    }
}

void ResponseHandler::onStatusReceive(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptedKey;
    std::getline(iss, encryptedKey);
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

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
    CryptoPP::SecByteBlock key = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

    std::string allowedLogin;
    std::getline(iss, allowedLogin);
    allowedLogin = utility::AESDecrypt(key, allowedLogin);

    m_client->updateMyLogin(allowedLogin);

    m_worker_UI->onCheckNewLoginSuccess();
}


void ResponseHandler::showFilesMessage(Message* message, const std::string& friendLoginHash, const std::string& myLoginHash) {
    auto& chatsMap = m_client->getMyHashChatsMap();
    auto it = chatsMap.find(friendLoginHash);
    if (it != chatsMap.end()) {
        Chat* chat = it->second;
        chat->getMessagesVec().push_back(message);
        m_worker_UI->onMessageReceive(friendLoginHash, message);
    }
    else {
        Chat* chat = new Chat;
        auto& msgsVec = chat->getMessagesVec();
        msgsVec.push_back(message);

        utility::incrementAllChatLayoutIndexes(chatsMap);
        chat->setLayoutIndex(0);

        chatsMap[friendLoginHash] = chat;

        m_client->requestUserInfoFromServer(friendLoginHash, myLoginHash);
    }
}

void ResponseHandler::processRequestedFile(net::File& file) {
    auto& chatsMap = m_client->getMyHashChatsMap();

    auto it = chatsMap.find(file.senderLoginHash);

    if (it != chatsMap.end()) {
        Chat* chat = it->second;
        auto& messagesVec = chat->getMessagesVec();
        auto it = std::find_if(messagesVec.begin(), messagesVec.end(), [&file](Message* msg) {
            return msg->getId() == file.blobUID;
            });

        if (it != messagesVec.end()) {
            Message* message = *it;
            auto& relatedFilesVec = message->getRelatedFiles();
            auto itFileWrapper = std::find_if(relatedFilesVec.begin(), relatedFilesVec.end(), [&file](fileWrapper fileWrapper) {
                return fileWrapper.file.id == file.id;
                });

            fileWrapper& wrap = *itFileWrapper;
            wrap.isPresent = true;
            wrap.file = file;

            m_worker_UI->updateFileLoadingState(file.senderLoginHash, wrap, false);
            m_client->getDatabase()->removeRequestedFile(file.receiverLoginHash, file.id);
            return;
        }
    }
    else {
        m_client->getDatabase()->removeRequestedFile(file.receiverLoginHash, file.id);
        std::filesystem::remove(file.filePath);
        return;
    }
}

void ResponseHandler::addDataToMessage(Message* message, net::File& file, bool isPresent) {
    Database* database = m_client->getDatabase();

    message->setIsRead(false);
    message->setIsSend(false);
    message->setMessage(file.caption);
    message->setId(file.blobUID);
    message->setTimestamp(file.timestamp);

    fileWrapper fileWrapper;
    fileWrapper.isPresent = isPresent;
    fileWrapper.file = std::move(file);
    message->addRelatedFile(fileWrapper);
}