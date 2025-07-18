#include "configManager.h"
#include "database.h"
#include "utility.h"
#include "chat.h"
#include "photo.h"
#include "client.h"

#include <rsa.h>                                           
#include <secblockfwd.h> 

ConfigManager::ConfigManager()
    : m_my_password_hash(""),
    m_my_login_hash(""),
    m_auto_login_path(""),
    m_is_undo_auto_login(false),
    m_my_login(""),
    m_my_name(""),
    m_is_auto_login(false),
    m_is_has_photo(false),
    m_my_photo(nullptr),
    m_client(nullptr)
{
}

void ConfigManager::save(const CryptoPP::RSA::PublicKey& myPublicKey, const CryptoPP::RSA::PrivateKey& myPrivateKey, const std::string& serverKey, std::unordered_map<std::string, Chat*> mapFriendLoginHashToChat, bool isHidden, Database* database) {
    CryptoPP::SecByteBlock AESEConfigKey;
    utility::generateAESKey(AESEConfigKey);

    QJsonObject jsonObject;
    jsonObject["my_login"] = QString::fromStdString(utility::AESEncrypt(AESEConfigKey, m_my_login));
    jsonObject["my_login_hash"] = QString::fromStdString(m_my_login_hash);
    jsonObject["is_hidden"] = QString::fromStdString(utility::AESEncrypt(AESEConfigKey, (isHidden ? "1" : "0")));
    jsonObject["my_name"] = QString::fromStdString(utility::AESEncrypt(AESEConfigKey, m_my_name));
    jsonObject["is_has_photo"] = QString::fromStdString(utility::AESEncrypt(AESEConfigKey, (m_is_has_photo ? "1" : "0")));
    if (m_is_has_photo && m_my_photo) {
        jsonObject["my_photo_path"] = QString::fromStdString(utility::AESEncrypt(AESEConfigKey, m_my_photo->getPhotoPath()));
    }
    if (!checkIsPasswordHashPresentInMyConfig() && !m_is_undo_auto_login) {
        jsonObject["my_password_hash"] = QString::fromStdString(m_my_password_hash);
    }

    if (!myPublicKey.GetModulus().IsZero() && !myPrivateKey.GetModulus().IsZero()) {
        jsonObject["public_key"] = QString::fromStdString(utility::encryptWithServerKey(utility::serializePublicKey(myPublicKey), serverKey));
        jsonObject["private_key"] = QString::fromStdString(utility::encryptWithServerKey(utility::serializePrivateKey(myPrivateKey), serverKey));
    }

    std::string encryptedAESEConfigKey = utility::RSAEncryptKey(myPublicKey, AESEConfigKey);
    jsonObject["encrypted_config_key"] = QString::fromStdString(encryptedAESEConfigKey);



    QJsonArray chatsArray;
    for (const auto& chatPair : mapFriendLoginHashToChat) {
        chatsArray.append(chatPair.second->serialize(myPublicKey, m_my_login, *database));
    }
    jsonObject["chatsArray"] = chatsArray;
    
    QString fileName = QString::fromStdString(m_my_login_hash) + ".json";
    QString dir = QString::fromStdString(utility::getConfigsAndPhotosDirectory());
    QDir saveDir(dir);
    QString fullPath = saveDir.filePath(fileName);

    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(jsonObject);
        file.write(saveDoc.toJson());
        file.close();
        qDebug() << "Successfully saved user data to:" << fullPath;
    }
    else {
        qWarning() << "Failed to open file for writing:" << fullPath;
    }
}

bool ConfigManager::load(const std::string& fileName, const std::string& specialServerKey, Database* database) {
    QString dir = QString::fromStdString(utility::getConfigsAndPhotosDirectory());
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

    if (jsonObject.contains("public_key") && jsonObject.contains("private_key")) {
        std::string encryptedPublicKeyStr = jsonObject["public_key"].toString().toStdString();
        m_client->setPublicKey(utility::deserializePublicKey(utility::decryptWithServerKey(encryptedPublicKeyStr, specialServerKey)));

        std::string encryptedPrivateKeyStr = jsonObject["private_key"].toString().toStdString();
        m_client->setPrivateKey(utility::deserializePrivateKey(utility::decryptWithServerKey(encryptedPrivateKeyStr, specialServerKey)));

        if (!utility::validateKeys(m_client->getPublicKey(), m_client->getPrivateKey()))
            std::cerr << "error: your keys do not match!\n";
    }

    CryptoPP::SecByteBlock AESEConfigKey = utility::RSADecryptKey(m_client->getPrivateKey(), jsonObject["encrypted_config_key"].toString().toStdString());

    m_my_login = utility::AESDecrypt(AESEConfigKey, jsonObject["my_login"].toString().toStdString());
    m_my_name = utility::AESDecrypt(AESEConfigKey, jsonObject["my_name"].toString().toStdString());
    m_is_has_photo = utility::AESDecrypt(AESEConfigKey, jsonObject["is_has_photo"].toString().toStdString()) == "1";
    m_client->setIsHidden(utility::AESDecrypt(AESEConfigKey, jsonObject["is_hidden"].toString().toStdString()) == "1");

    if (m_is_has_photo && jsonObject.contains("my_photo_path")) {
        QString photoPath = QString::fromStdString(utility::AESDecrypt(AESEConfigKey, jsonObject["my_photo_path"].toString().toStdString()));
        if (!photoPath.isEmpty()) {
            m_my_photo = new Photo(m_client->getPrivateKey(), photoPath.toStdString());
            m_my_photo->loadBinaryDataFromPc();
        }
    }

    auto& mapFriendLoginToChat = m_client->getMyHashChatsMap();
    if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].isArray()) {
        QJsonArray chatsArray = jsonObject["chatsArray"].toArray();
        for (const QJsonValue& value : chatsArray) {
            if (value.isObject()) {
                Chat* chat = Chat::deserialize(m_client->getPrivateKey(), m_my_login, value.toObject(), *database);
                if (chat) {
                    mapFriendLoginToChat[utility::calculateHash(chat->getFriendLogin())] = chat;
                }
            }
        }
    }

    return true;
}

void ConfigManager::updateConfigFileName(const std::string& oldLoginHash, const std::string& newLoginHash) {
    QString oldFileName = QString::fromStdString(utility::getConfigsAndPhotosDirectory()) +
        QString::fromStdString("/" + oldLoginHash) + ".json";
    QFile oldFile(oldFileName);

    if (oldFile.exists()) {
        QString newFileName = QString::fromStdString(utility::getConfigsAndPhotosDirectory()) +
            QString::fromStdString("/" + newLoginHash) + ".json";

        if (!oldFile.rename(newFileName)) {
            qWarning() << "Failed to rename config file from" << oldFileName << "to" << newFileName;
            return;
        }
    }
}

void ConfigManager::updateInConfigFriendLogin(const std::string& oldLogin, const std::string& newLogin) {
    QString dir = QString::fromStdString(utility::getConfigsAndPhotosDirectory());
    QString fileName = QString::fromStdString("/" + m_my_login_hash) + ".json";
    QString fullPath = dir + fileName;
    std::string STRDEBUG = fullPath.toStdString();

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open config file for reading:" << fullPath;
        return;
    }

    QJsonDocument configDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (configDoc.isNull()) {
        qWarning() << "Invalid JSON in config file:" << fullPath;
        return;
    }

    QJsonObject configObj = configDoc.object();

    if (!configObj.contains("chatsArray") || !configObj["chatsArray"].isArray()) {
        qWarning() << "No chats array found in config";
        return;
    }

    QJsonArray chatsArray = configObj["chatsArray"].toArray();
    bool found = false;

    for (auto&& chatValue : chatsArray) {
        if (!chatValue.isObject()) continue;
        CryptoPP::SecByteBlock chatConfigKey = getChatConfigKey(newLogin);

        QJsonObject chatObj = chatValue.toObject();
        std::string currentLogin = utility::AESDecrypt(chatConfigKey, chatObj["friend_login"].toString().toStdString());
        if (currentLogin == oldLogin) {
            chatObj["friend_login"] = QString::fromStdString(utility::AESEncrypt(chatConfigKey, newLogin));
            chatValue = chatObj;
            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "Friend login" << oldLogin.c_str() << "not found in config";
        return;
    }

    configObj["chatsArray"] = chatsArray;

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open config file for writing:" << fullPath;
        return;
    }

    file.write(QJsonDocument(configObj).toJson());
    file.close();
}

bool ConfigManager::checkIsAutoLogin() {
    QString dir = QString::fromStdString(utility::getConfigsAndPhotosDirectory());
    QDir saveDir(dir);

    QStringList jsonFiles = saveDir.entryList(QStringList() << "*.json", QDir::Files);
    if (jsonFiles.isEmpty()) {
        qDebug() << "No JSON files found for auto-login";
        return false;
    }

    for (const QString& jsonFile : jsonFiles) {
        QString fullPath = saveDir.filePath(jsonFile);
        QFile file(fullPath);

        if (!file.open(QIODevice::ReadOnly)) {
            qWarning() << "Couldn't open JSON file:" << fullPath;
            continue;
        }

        QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (!loadDoc.isObject()) {
            qWarning() << "Invalid JSON format in file:" << fullPath;
            continue;
        }

        QJsonObject jsonObject = loadDoc.object();
        if (jsonObject.contains("my_password_hash")) {
            qDebug() << "Auto-login configuration available in file:" << fullPath;

            m_auto_login_path = fullPath.toStdString();
            return true;
        }
    }

    qDebug() << "No valid auto-login configuration found";
        return false;
}

void ConfigManager::deleteFriendChatInConfig(const std::string& friendLogin) {
    QString configPath = QString::fromStdString(utility::getConfigsAndPhotosDirectory() + "/" + m_my_login_hash + ".json");
    QFile file(configPath);

    if (!file.open(QIODevice::ReadWrite)) {
        qWarning() << "Couldn't open config file for update:" << configPath;
        return;
    }

    QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
    if (!loadDoc.isObject()) {
        qWarning() << "Invalid JSON in config file:" << configPath;
        file.close();
        return;
    }

    QJsonObject jsonObject = loadDoc.object();

    if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].isArray()) {
        CryptoPP::SecByteBlock chatConfigKey = getChatConfigKey(friendLogin);
        
        QJsonArray chatsArray = jsonObject["chatsArray"].toArray();
        QJsonArray newChatsArray;
        for (const QJsonValue& value : chatsArray) {
            if (value.isObject()) {
                QJsonObject chatObj = value.toObject();
                if (chatObj.contains("friend_login") &&
                    utility::AESDecrypt(chatConfigKey, chatObj["friend_login"].toString().toStdString()) != friendLogin) {
                    newChatsArray.append(chatObj);
                }
            }
        }

        if (newChatsArray.size() != chatsArray.size()) {
            jsonObject["chatsArray"] = newChatsArray;

            file.resize(0);
            file.write(QJsonDocument(jsonObject).toJson());
            qDebug() << "Chat with friend" << QString::fromStdString(friendLogin)
                << "removed from config";
        }
    }

    file.close();
}

bool ConfigManager::undoAutoLogin() {
    QString oldFileName = QString::fromStdString(utility::getConfigsAndPhotosDirectory()) +
        QString::fromStdString("/" + m_my_login_hash) + ".json";

    QFile file(oldFileName);

    if (!file.exists()) {
        qWarning() << "Auto-login file not found:" << oldFileName;
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file for reading:" << oldFileName;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON in file:" << oldFileName;
        return false;
    }

    QJsonObject jsonObj = doc.object();

    if (!jsonObj.contains("my_password_hash")) {
        qDebug() << "Password hash field not found in file:" << oldFileName;
        return true;
    }

    jsonObj.remove("my_password_hash");

    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't open file for writing:" << oldFileName;
        return false;
    }

    QJsonDocument newDoc(jsonObj);
    if (file.write(newDoc.toJson()) == -1) {
        qWarning() << "Failed to write to file:" << oldFileName;
        file.close();
        return false;
    }

    file.close();
    qDebug() << "Successfully removed password hash from:" << oldFileName;
    return true;
}



// private
void ConfigManager::loadLoginHash() {
    QFile file(QString::fromStdString(m_auto_login_path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the .json config file: config.json";
        return;
    }

    QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!loadDoc.isObject()) {
        qWarning() << "Invalid JSON in the file: config.json";
        return;
    }

    QJsonObject jsonObject = loadDoc.object();

    if (jsonObject.contains("my_login_hash")) {
        std::string loginHash = jsonObject["my_login_hash"].toString().toStdString();
        m_my_login_hash = loginHash;
    }
}

void ConfigManager::loadPasswordHash() {
    QFile file(QString::fromStdString(m_auto_login_path));
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the .json config file: config.json";
        return;
    }

    QJsonDocument loadDoc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!loadDoc.isObject()) {
        qWarning() << "Invalid JSON in the file: config.json";
        return;
    }

    QJsonObject jsonObject = loadDoc.object();

    if (jsonObject.contains("my_password_hash")) {
        std::string passwordHash = jsonObject["my_password_hash"].toString().toStdString();
        m_my_password_hash = passwordHash;
    }
}

bool ConfigManager::checkIsPasswordHashPresentInMyConfig() const {
    QString dir = QString::fromStdString(utility::getConfigsAndPhotosDirectory());
    QDir saveDir(dir);

    if (!saveDir.exists()) {
        if (!saveDir.mkpath(".")) {
            qWarning() << "Failed to create directory:" << dir;
            return false;
        }
    }

    bool isPresent = false;
    QStringList jsonFiles = saveDir.entryList(QStringList() << "*.json", QDir::Files);

    for (const QString& file : jsonFiles) {
        if (file == QString::fromStdString(m_my_login_hash + ".json")) {
            continue;
        }

        QFileInfo fileInfo(saveDir.filePath(file));
        QFile f(fileInfo.filePath());

        if (f.open(QIODevice::ReadOnly)) {
            QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
            f.close();

            if (doc.isObject() && doc.object().contains("my_password_hash")) {
                isPresent = true;
                break;
            }
        }
    }

    return isPresent;
}

CryptoPP::SecByteBlock ConfigManager::getChatConfigKey(const std::string& login) {
    auto chatOpt = m_client->findChat(utility::calculateHash(login)); 
    auto chat = *chatOpt;
    return chat->getChatConfigKey();
}
