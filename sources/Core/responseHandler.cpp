#include "responseHandler.h"
#include "workerUI.h"
#include "chatsWidget.h"
#include "configManager.h"
#include "friendInfo.h"
#include "client.h"
#include "utility.h"
#include "queryType.h"
#include "database.h"
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
    else if (msg.header.type == QueryType::USER_INFO_SUCCESS) {
        onUserInfoSuccess(packet);
    }
    else if (msg.header.type == QueryType::USER_INFO_FAIL) {
        onUserInfoFail(packet);
    }
    else if (msg.header.type == QueryType::MY_INFO) {
        onMyInfo(packet);
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
    else if (msg.header.type == QueryType::FILE_PREVIEW) {
        onFilePreview(packet);
    }
    else if (msg.header.type == QueryType::UPDATE_OFFER) {
        onUpdateOffer(packet);
    }
}

void  ResponseHandler::onUpdateOffer(const std::string& packet) {
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
    m_client->initDatabase(m_configManager->getMyLoginHash());

    m_client->afterRegistrationSendMyInfo();
    m_client->generateMyKeyPair();
    m_client->sendPublicKeyToServer();

    m_client->connectFilesSocket(m_configManager->getMyLoginHash(), m_client->getServerIpAddress(), m_client->geServerPort());

    m_configManager->setIsNeedToAutoLogin(true);
    m_client->setIsLoggedIn(true);
    m_worker_UI->onRegistrationSuccess();
}

void ResponseHandler::onRegistrationFail() {
    m_worker_UI->onRegistrationFail();
}

void ResponseHandler::onAuthorizationFail() {
    m_configManager->setMyLoginHash("");
    m_configManager->setMyPasswordHash("");
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

namespace fs = std::filesystem;

void ResponseHandler::processNewVersionLoadedFile(net::file<QueryType>& file) {
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

void ResponseHandler::onFile(net::file<QueryType>& file) {
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
    
    net::file<QueryType> file;
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

void ResponseHandler::onAuthorizationSuccess(const std::string& packet) {
    std::istringstream iss(packet);

    std::string encryptionPart;
    std::getline(iss, encryptionPart);

    std::string serverPublicKey;
    std::getline(iss, serverPublicKey);

    m_client->setServerEncryptionPart(encryptionPart);
    m_client->setServerPublicKey(utility::deserializePublicKey(serverPublicKey));

    const std::string& myLoginHash = m_configManager->getMyLoginHash();
    m_client->initDatabase(myLoginHash);

    if (!m_configManager->getIsAutoLogin()) {
        bool res = m_configManager->load((myLoginHash + ".json"), m_client->getSpecialServerKey(), m_client->getDatabase());
        if (!res) {
            m_client->requestMyInfoFromServerAndResetKeys(m_configManager->getMyLoginHash());
            m_worker_UI->showConfigLoadErrorDialog();
        }
        else {
            m_worker_UI->supplyTheme(m_configManager->getIsDarkTheme());
            m_configManager->setIsNeedToAutoLogin(true);
        }
    }

    m_client->connectFilesSocket(myLoginHash, m_client->getServerIpAddress(), m_client->geServerPort());
    m_client->setIsLoggedIn(true);
    m_worker_UI->onAuthorizationSuccess();
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

        std::string name;
        std::getline(iss, name);
        name = utility::AESDecrypt(key, name);
        friendInfo->setFriendName(name);

        std::string lastSeen;
        std::getline(iss, lastSeen);
        lastSeen = utility::AESDecrypt(key, lastSeen);
        friendInfo->setFriendLastSeen(lastSeen);

        std::string isHasPhotoStr;
        std::getline(iss, isHasPhotoStr);
        isHasPhotoStr = utility::AESDecrypt(key, isHasPhotoStr);
        bool isHasPhoto = isHasPhotoStr == "true";
        friendInfo->setIsFriendHasPhoto(isHasPhoto);

        std::string sizeStr;
        std::getline(iss, sizeStr);
        sizeStr = utility::AESDecrypt(key, sizeStr);
        
        if (isHasPhoto) {
            std::string photoEncrypted;
            std::getline(iss, photoEncrypted);
            std::string photoDecrypted = utility::AESDecrypt(key, photoEncrypted);

            size_t pos = photoDecrypted.find('\n');
            std::string dataFirstPartStr;
            std::string dataSecondPartStr;

            if (pos != std::string::npos) {
                dataFirstPartStr = photoDecrypted.substr(0, pos);
                dataSecondPartStr = photoDecrypted.substr(pos + 1);
            }
            else {
                dataFirstPartStr = photoDecrypted;
                dataSecondPartStr.clear();
            }


            Photo* photo = Photo::deserializeWithoutSaveOnDisc(m_client->getPrivateKey(), m_client->getServerPublicKey(), dataFirstPartStr + "\n" + dataSecondPartStr);
            friendInfo->setFriendPhoto(photo);
        }
        else {
            std::string spaceString;
            std::getline(iss, spaceString);
            std::getline(iss, spaceString);
        }

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

    std::string isHasPhoto;
    std::getline(iss, isHasPhoto);
    isHasPhoto = utility::AESDecrypt(key, isHasPhoto);

    std::string sizeStr;
    std::getline(iss, sizeStr);
    sizeStr = utility::AESDecrypt(key, sizeStr);
    size_t size = std::stoi(sizeStr);

    std::string lastSeen;
    std::getline(iss, lastSeen);
    lastSeen = utility::AESDecrypt(key, lastSeen);

    Photo* photo = nullptr;
    if (isHasPhoto == "true") {
        std::string dataFirstPartStr;
        std::getline(iss, dataFirstPartStr);
        std::string dataSecondPartStr;
        std::getline(iss, dataSecondPartStr);

         photo = Photo::deserializeAndSaveOnDisc(m_client->getPrivateKey(), dataFirstPartStr + "\n" + dataSecondPartStr, login);
    }

    std::string friendPublicKeyStr;
    std::getline(iss, friendPublicKeyStr);
    auto friendPublicKey = utility::deserializePublicKey(friendPublicKeyStr);

    Chat* chat = new Chat;
    chat->setFriendLogin(login);
    chat->setFriendName(name);
    chat->setIsFriendHasPhoto(isHasPhoto == "true");
    chat->setLayoutIndex(0);
    chat->setPublicKey(friendPublicKey);

    utility::incrementAllChatLayoutIndexes(m_client->getMyHashChatsMap());

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

    Photo* photo = nullptr;
    if (isHasPhoto) {
        std::string dataFirstPartStr;
        std::getline(iss, dataFirstPartStr);


        std::string dataSecondPartStr;
        std::getline(iss, dataSecondPartStr);

        photo = Photo::deserializeAndSaveOnDisc(m_client->getPrivateKey(), dataFirstPartStr + "\n" + dataSecondPartStr, login);
        photo->loadBinaryDataFromPc();
    }

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
    chat->setIsFriendHasPhoto(isHasPhoto);
    chat->setPublicKey(friendPublicKey);
    chat->setFriendPhoto(isHasPhoto ? photo : nullptr);

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

    std::string isHasPhotoStr;
    std::getline(iss, isHasPhotoStr);
    isHasPhotoStr = utility::AESDecrypt(key, isHasPhotoStr);
    bool isHasPhoto = isHasPhotoStr == "true";

    std::string sizeStr;
    std::getline(iss, sizeStr);
    sizeStr = utility::AESDecrypt(key, sizeStr);

    std::string dataFirstPartStr;
    std::getline(iss, dataFirstPartStr);
    std::string dataSecondPartStr;
    std::getline(iss, dataSecondPartStr);

    if (std::stoi(sizeStr) != 0) {
        Photo* photo = Photo::deserializeAndSaveOnDisc(m_client->getPrivateKey(), dataFirstPartStr + "\n" + dataSecondPartStr, login);
        m_configManager->setIsHasPhoto(true);
        m_configManager->setPhoto(photo);
        m_worker_UI->setRecoveredAvatar(photo);
    }
    else {
        m_configManager->setIsHasPhoto(false);
        m_configManager->setPhoto(nullptr);
    }

    m_configManager->setIsNeedToAutoLogin(true);
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

void ResponseHandler::processRequestedFile(net::file<QueryType>& file) {
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

void ResponseHandler::addDataToMessage(Message* message, net::file<QueryType>& file, bool isPresent) {
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