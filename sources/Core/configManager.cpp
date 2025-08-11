#include "configManager.h"

#include "database.h"
#include "keysManager.h"
#include "utility.h"
#include "chat.h"
#include "avatar.h"
#include "client.h"

#include <json.hpp>                                     
#include <secblockfwd.h> 

ConfigManager::ConfigManager()
    : 
    m_currentVersionNumber(""),
    m_myUID(""),
    m_myPasswordHash(""),
    m_myLoginHash(""),
    m_myName(""),
    m_myAvatar(nullptr),
    m_isHasAvatar(false),
    m_isHidden(true),
    m_isDarkTheme(true)
{
}



void ConfigManager::save(const std::unordered_map<std::string, ChatPtr>& mapFriendUIDToChat, DatabasePtr database, bool isAutoLogin) const {
    CryptoPP::SecByteBlock mainConfigKey;
    utility::generateAESKey(mainConfigKey);

    nlohmann::json jsonObject;
    jsonObject["currentVersionNumber"] = utility::AESEncrypt(mainConfigKey, m_currentVersionNumber);
    jsonObject["myUID"] = utility::AESEncrypt(mainConfigKey, m_myUID);

    if (isAutoLogin) {
        jsonObject["passwordHash"] = m_myPasswordHash;
    }


    jsonObject["loginHash"] = m_myLoginHash;
    jsonObject["login"] = utility::AESEncrypt(mainConfigKey, m_myLogin);
    jsonObject["name"] = utility::AESEncrypt(mainConfigKey, m_myName);
    jsonObject["isHasAvatar"] = utility::AESEncrypt(mainConfigKey, (m_isHasAvatar ? "1" : "0"));

    if (m_isHasAvatar) {
        jsonObject["myAvatarPath"] = utility::AESEncrypt(mainConfigKey, m_myAvatar->getPath());
    }

    jsonObject["isHidden"] = m_isHidden;
    jsonObject["isDarkTheme"] = m_isDarkTheme;
    jsonObject["publicKey"] = utility::encryptWithServerKey(utility::serializePublicKey(m_keysManager->getMyPublicKey()), m_keysManager->getServerSpecialKey());
    jsonObject["privateKey"] = utility::encryptWithServerKey(utility::serializePrivateKey(m_keysManager->getMyPrivateKey()), m_keysManager->getServerSpecialKey());
    jsonObject["encryptedMainConfigKey"] = utility::RSAEncryptKey(m_keysManager->getMyPublicKey(), mainConfigKey);

    nlohmann::json chatsArray = nlohmann::json::array();
    for (const auto& chatPair : mapFriendUIDToChat) {
        chatsArray.push_back(chatPair.second->serialize(m_keysManager->getMyPublicKey(), m_myUID, database));
    }
    jsonObject["chatsArray"] = chatsArray;

    std::string fileName = m_myUID + ".json";
    std::string dir = utility::getConfigsAndPhotosDirectory();
    std::filesystem::path fullPath = std::filesystem::path(dir) / fileName;

    std::ofstream file(fullPath);
    if (file.is_open()) {
        file << jsonObject.dump(4);
        file.close();
        std::cout << "Successfully saved user data to: " << fullPath << std::endl;
    }
    else {
        std::cerr << "Failed to open file for writing: " << fullPath << std::endl;
    }
}

bool ConfigManager::load(std::unordered_map<std::string, ChatPtr>& mapFriendUIDToChat, const std::string& fileName, DatabasePtr database) {
    try {
        std::string dir = utility::getConfigsAndPhotosDirectory();
        std::filesystem::path fullPath = std::filesystem::path(dir) / fileName;

        std::ifstream file(fullPath);
        if (!file.is_open()) {
            std::cerr << "Couldn't open the .json config file: " << fileName << std::endl;
            return false;
        }

        nlohmann::json jsonObject;
        file >> jsonObject;
        file.close();

        std::string encryptedPublicKeyStr = jsonObject["publicKey"].get<std::string>();
        m_keysManager->setMyPublicKey(utility::deserializePublicKey(
            utility::decryptWithServerKey(encryptedPublicKeyStr, m_keysManager->getServerSpecialKey())));

        std::string encryptedPrivateKeyStr = jsonObject["privateKey"].get<std::string>();
        m_keysManager->setMyPrivateKey(utility::deserializePrivateKey(
            utility::decryptWithServerKey(encryptedPrivateKeyStr, m_keysManager->getServerSpecialKey())));

        if (!utility::validateKeys(m_keysManager->getMyPublicKey(), m_keysManager->getMyPrivateKey())) {
            std::cerr << "Error: Your keys do not match!" << std::endl;
            return false;
        }

        CryptoPP::SecByteBlock mainConfigKey = utility::RSADecryptKey(
            m_keysManager->getMyPrivateKey(),
            jsonObject["encryptedMainConfigKey"].get<std::string>());

        m_currentVersionNumber = utility::AESDecrypt(mainConfigKey, jsonObject["currentVersionNumber"].get<std::string>());
        m_myUID = utility::AESDecrypt(mainConfigKey, jsonObject["myUID"].get<std::string>());

        if (jsonObject.contains("passwordHash")) {
            m_myPasswordHash = jsonObject["passwordHash"].get<std::string>();
        }

        m_myLoginHash = jsonObject["loginHash"].get<std::string>();
        m_myName = utility::AESDecrypt(mainConfigKey, jsonObject["name"].get<std::string>());
        m_isHasAvatar = utility::AESDecrypt(mainConfigKey, jsonObject["isHasAvatar"].get<std::string>()) == "1";

        if (m_isHasAvatar && jsonObject.contains("myAvatarPath")) {
            std::string avatarPath = utility::AESDecrypt(mainConfigKey, jsonObject["myAvatarPath"].get<std::string>());
            m_myAvatar = std::make_shared<Avatar>(m_keysManager->getAvatarsKey(), avatarPath);
        }

        m_isHidden = jsonObject["isHidden"].get<bool>();
        m_isDarkTheme = jsonObject["isDarkTheme"].get<bool>();

        if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].is_array()) {
            for (const auto& chatJson : jsonObject["chatsArray"]) {
                ChatPtr chat = Chat::deserialize(
                    m_keysManager->getMyPrivateKey(),
                    m_myUID,
                    chatJson,
                    database,
                    m_keysManager->getAvatarsKey());
                if (chat) {
                    mapFriendUIDToChat[chat->getFriendUID()] = chat;
                }
            }
        }

        m_loadedConfigPath = fullPath.string();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        return false;
    }
    catch (...) {
        std::cerr << "Unknown error loading config" << std::endl;
        return false;
    }
}

std::optional<std::string> ConfigManager::findAutoLoginConfigPath() {
    std::string dir = utility::getConfigsAndPhotosDirectory();

    try {
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.path().extension() == ".json") {
                std::ifstream file(entry.path());
                if (!file.is_open()) {
                    std::cerr << "Couldn't open JSON file: " << entry.path() << std::endl;
                    continue;
                }

                nlohmann::json jsonObject;
                try {
                    file >> jsonObject;
                    if (jsonObject.contains("passwordHash")) {
                        std::cout << "Auto-login configuration available in file: " << entry.path() << std::endl;
                        return entry.path().string();
                    }
                }
                catch (const nlohmann::json::parse_error& e) {
                    std::cerr << "Invalid JSON format in file: " << entry.path() << " - " << e.what() << std::endl;
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    std::cout << "No valid auto-login configuration found" << std::endl;
    return std::nullopt;
}

bool ConfigManager::removeFriendChatFromConfig(const std::string& friendUID) {
    std::ifstream inFile(m_loadedConfigPath);
    if (!inFile.is_open()) {
        std::cerr << "[removeFriendChatFromConfig] Failed to open config file: " << m_loadedConfigPath << std::endl;
        return false;
    }

    nlohmann::json jsonObject;
    try {
        inFile >> jsonObject;
        inFile.close();
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[removeFriendChatFromConfig] Invalid JSON: " << e.what() << std::endl;
        return false;
    }

    bool isModified = false;
    if (jsonObject.contains("chatsArray") && jsonObject["chatsArray"].is_array()) {
        auto& chatsArray = jsonObject["chatsArray"];
        for (auto it = chatsArray.begin(); it != chatsArray.end(); ) {
            if (it->contains("friendUID") && it->at("friendUID").get<std::string>() == friendUID) {
                it = chatsArray.erase(it);
                isModified = true;
                break;
            }
            else {
                ++it;
            }
        }
    }

    if (isModified) {
        std::ofstream outFile(m_loadedConfigPath);
        if (!outFile.is_open()) {
            std::cerr << "[removeFriendChatFromConfig] Failed to write updated config" << std::endl;
            return false;
        }
        outFile << jsonObject.dump(4);
        std::cout << "Successfully removed chat with friend UID: " << friendUID << std::endl;
    }

    return isModified;
}

bool ConfigManager::removePasswordHashFromConfig() {
    if (!std::filesystem::exists(m_loadedConfigPath)) {
        std::cerr << "Auto-login file not found: " << m_loadedConfigPath << std::endl;
        return false;
    }

    std::ifstream inFile(m_loadedConfigPath);
    if (!inFile.is_open()) {
        std::cerr << "[removePasswordHashFromConfig] Couldn't open file: " << m_loadedConfigPath << std::endl;
        return false;
    }

    nlohmann::json jsonObj;
    try {
        inFile >> jsonObj;
        inFile.close();
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "[removePasswordHashFromConfig] Invalid JSON: " << e.what() << std::endl;
        return false;
    }

    if (!jsonObj.contains("passwordHash")) {
        std::cout << "[removePasswordHashFromConfig] Password hash not found" << std::endl;
        return true;
    }

    jsonObj.erase("passwordHash");

    std::ofstream outFile(m_loadedConfigPath);
    if (!outFile.is_open()) {
        std::cerr << "[removePasswordHashFromConfig] Couldn't open file for writing" << std::endl;
        return false;
    }

    outFile << jsonObj.dump(4);
    std::cout << "Successfully removed password hash from: " << m_loadedConfigPath << std::endl;
    return true;
}








void ConfigManager::preloadLoginHash(const std::string& autoLoginPath) {
    std::ifstream file(autoLoginPath);
    if (!file.is_open()) {
        std::cerr << "Couldn't open config file: " << autoLoginPath << std::endl;
        return;
    }

    try {
        nlohmann::json jsonObject;
        file >> jsonObject;
        if (jsonObject.contains("loginHash")) {
            m_myLoginHash = jsonObject["loginHash"].get<std::string>();
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Invalid JSON: " << e.what() << std::endl;
    }
}

void ConfigManager::preloadTheme(const std::string& autoLoginPath) {
    std::ifstream file(autoLoginPath);
    if (!file.is_open()) {
        std::cerr << "Couldn't open config file: " << autoLoginPath << std::endl;
        return;
    }

    try {
        nlohmann::json jsonObject;
        file >> jsonObject;
        if (jsonObject.contains("isDarkTheme")) {
            m_isDarkTheme = jsonObject["isDarkTheme"].get<bool>();
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Invalid JSON: " << e.what() << std::endl;
    }
}

void ConfigManager::preloadIsHidden(const std::string& autoLoginPath) {
    std::ifstream file(autoLoginPath);
    if (!file.is_open()) {
        std::cerr << "Couldn't open config file: " << autoLoginPath << std::endl;
        return;
    }

    try {
        nlohmann::json jsonObject;
        file >> jsonObject;
        if (jsonObject.contains("isHidden")) {
            m_isHidden = jsonObject["isHidden"].get<bool>();
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Invalid JSON: " << e.what() << std::endl;
    }
}

void ConfigManager::preloadPasswordHash(const std::string& autoLoginPath) {
    std::ifstream file(autoLoginPath);
    if (!file.is_open()) {
        std::cerr << "Couldn't open config file: " << autoLoginPath << std::endl;
        return;
    }

    try {
        nlohmann::json jsonObject;
        file >> jsonObject;
        if (jsonObject.contains("passwordHash")) {
            m_myPasswordHash = jsonObject["passwordHash"].get<std::string>();
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Invalid JSON: " << e.what() << std::endl;
    }
}