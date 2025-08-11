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
    jsonObject[CURRENT_VERSION_NUMBER] = utility::AESEncrypt(mainConfigKey, m_currentVersionNumber);
    jsonObject[MY_UID] = utility::AESEncrypt(mainConfigKey, m_myUID);

    if (isAutoLogin) {
        jsonObject[PASSWORD_HASH] = m_myPasswordHash;
    }


    jsonObject[LOGIN_HASH] = m_myLoginHash;
    jsonObject[LOGIN] = utility::AESEncrypt(mainConfigKey, m_myLogin);
    jsonObject[NAME] = utility::AESEncrypt(mainConfigKey, m_myName);
    jsonObject[IS_HAS_AVATAR] = utility::AESEncrypt(mainConfigKey, (m_isHasAvatar ? ONE_STR : ZERO_STR));

    if (m_isHasAvatar) {
        jsonObject[MY_AVATAR_PATH] = utility::AESEncrypt(mainConfigKey, m_myAvatar->getPath());
    }

    jsonObject[IS_HIDDEN] = m_isHidden;
    jsonObject[IS_DARK_THEME] = m_isDarkTheme;
    jsonObject[PUBLIC_KEY] = utility::encryptWithServerKey(utility::serializePublicKey(m_keysManager->getMyPublicKey()), m_keysManager->getServerSpecialKey());
    jsonObject[PRIVATE_KEY] = utility::encryptWithServerKey(utility::serializePrivateKey(m_keysManager->getMyPrivateKey()), m_keysManager->getServerSpecialKey());
    jsonObject[ENCRYPTED_MAIN_CONFIG_KEY] = utility::RSAEncryptKey(m_keysManager->getMyPublicKey(), mainConfigKey);

    nlohmann::json chatsArray = nlohmann::json::array();
    for (const auto& chatPair : mapFriendUIDToChat) {
        chatsArray.push_back(chatPair.second->serialize(m_keysManager->getMyPublicKey(), m_myUID, database));
    }
    jsonObject[CHATS_ARRAY] = chatsArray;

    std::string fileName = m_myUID + JSON_EXT;
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

        std::string encryptedPublicKeyStr = jsonObject[PUBLIC_KEY].get<std::string>();
        m_keysManager->setMyPublicKey(utility::deserializePublicKey(
            utility::decryptWithServerKey(encryptedPublicKeyStr, m_keysManager->getServerSpecialKey())));

        std::string encryptedPrivateKeyStr = jsonObject[PRIVATE_KEY].get<std::string>();
        m_keysManager->setMyPrivateKey(utility::deserializePrivateKey(
            utility::decryptWithServerKey(encryptedPrivateKeyStr, m_keysManager->getServerSpecialKey())));

        if (!utility::validateKeys(m_keysManager->getMyPublicKey(), m_keysManager->getMyPrivateKey())) {
            std::cerr << "Error: Your keys do not match!" << std::endl;
            return false;
        }

        CryptoPP::SecByteBlock mainConfigKey = utility::RSADecryptKey(
            m_keysManager->getMyPrivateKey(),
            jsonObject[ENCRYPTED_MAIN_CONFIG_KEY].get<std::string>());

        m_currentVersionNumber = utility::AESDecrypt(mainConfigKey, jsonObject[CURRENT_VERSION_NUMBER].get<std::string>());
        m_myUID = utility::AESDecrypt(mainConfigKey, jsonObject[MY_UID].get<std::string>());

        if (jsonObject.contains(PASSWORD_HASH)) {
            m_myPasswordHash = jsonObject[PASSWORD_HASH].get<std::string>();
        }

        m_myLoginHash = jsonObject[LOGIN_HASH].get<std::string>();
        m_myName = utility::AESDecrypt(mainConfigKey, jsonObject[NAME].get<std::string>());
        m_isHasAvatar = utility::AESDecrypt(mainConfigKey, jsonObject[IS_HAS_AVATAR].get<std::string>()) == ONE_STR;

        if (m_isHasAvatar && jsonObject.contains(MY_AVATAR_PATH)) {
            std::string avatarPath = utility::AESDecrypt(mainConfigKey, jsonObject[MY_AVATAR_PATH].get<std::string>());
            m_myAvatar = std::make_shared<Avatar>(m_keysManager->getAvatarsKey(), avatarPath);
        }

        m_isHidden = jsonObject[IS_HIDDEN].get<bool>();
        m_isDarkTheme = jsonObject[IS_DARK_THEME].get<bool>();

        if (jsonObject.contains(CHATS_ARRAY) && jsonObject[CHATS_ARRAY].is_array()) {
            for (const auto& chatJson : jsonObject[CHATS_ARRAY]) {
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
            if (entry.path().extension() == JSON_EXT) {
                std::ifstream file(entry.path());
                if (!file.is_open()) {
                    std::cerr << "Couldn't open JSON file: " << entry.path() << std::endl;
                    continue;
                }

                nlohmann::json jsonObject;
                try {
                    file >> jsonObject;
                    if (jsonObject.contains(PASSWORD_HASH)) {
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
    if (jsonObject.contains(CHATS_ARRAY) && jsonObject[CHATS_ARRAY].is_array()) {
        auto& chatsArray = jsonObject[CHATS_ARRAY];
        for (auto it = chatsArray.begin(); it != chatsArray.end(); ) {
            if (it->contains(FRIEND_UID) && it->at(FRIEND_UID).get<std::string>() == friendUID) {
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

    if (!jsonObj.contains(PASSWORD_HASH)) {
        std::cout << "[removePasswordHashFromConfig] Password hash not found" << std::endl;
        return true;
    }

    jsonObj.erase(PASSWORD_HASH);

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
        if (jsonObject.contains(LOGIN_HASH)) {
            m_myLoginHash = jsonObject[LOGIN_HASH].get<std::string>();
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
        if (jsonObject.contains(IS_DARK_THEME)) {
            m_isDarkTheme = jsonObject[IS_DARK_THEME].get<bool>();
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
        if (jsonObject.contains(IS_HIDDEN)) {
            m_isHidden = jsonObject[IS_HIDDEN].get<bool>();
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
        if (jsonObject.contains(PASSWORD_HASH)) {
            m_myPasswordHash = jsonObject[PASSWORD_HASH].get<std::string>();
        }
    }
    catch (const nlohmann::json::parse_error& e) {
        std::cerr << "Invalid JSON: " << e.what() << std::endl;
    }
}