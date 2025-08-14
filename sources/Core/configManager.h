#pragma once

#include <string>

#include <iostream>
#include <codecvt>
#include <locale>
#include <fstream>
#include <unordered_map>
#include <rsa.h>

#include "theme.h"

class Avatar;
class Database;
class Chat;
class KeysManager;

typedef std::shared_ptr<KeysManager> KeysManagerPtr;
typedef std::shared_ptr<Database> DatabasePtr;
typedef std::shared_ptr<Avatar> AvatarPtr;
typedef std::shared_ptr<Chat> ChatPtr;

class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;

    void save(const std::unordered_map<std::string, ChatPtr>& mapFriendUIDToChat, DatabasePtr database, bool isAutoLogin) const;
    bool load(std::unordered_map<std::string, ChatPtr>& mapFriendUIDToChat, const std::string& fileName, DatabasePtr database);

    std::optional<std::string> findAutoLoginConfigPath();
    void preloadLoginHash(const std::string& autoLoginPath);
    void preloadPasswordHash(const std::string& autoLoginPath);
    void preloadTheme(const std::string& autoLoginPath);
    void preloadIsHidden(const std::string& autoLoginPath);

    bool removePasswordHashFromConfig();
    bool removeFriendChatFromConfig(const std::string& friendUID);

    //GET && SET
    void setMyUID(const std::string& myUID) { m_myUID = myUID; }
    const std::string& getMyUID() const { return m_myUID; }
    
    void setMyPasswordHash(const std::string& passwordHash) { m_myPasswordHash = passwordHash; }
    const std::string& getMyPasswordHash() const { return m_myPasswordHash; }

    void setMyLoginHash(const std::string& loginHash) { m_myLoginHash = loginHash; }
    const std::string& getMyLoginHash() const { return m_myLoginHash; }

    void setMyLogin(const std::string& login) { m_myLogin = login; }
    const std::string& getMyLogin() const { return m_myLogin; }

    void setMyName(const std::string& name) { m_myName = name; }
    const std::string& getMyName() const { return m_myName; }

    void setAvatar(AvatarPtr avatar) { m_myAvatar = avatar; }
    AvatarPtr getAvatar() const { return m_myAvatar; }

    void setIsHasAvatar(bool isHasAvatar) { m_isHasAvatar = isHasAvatar; }
    const bool getIsHasAvatar() const { return  m_isHasAvatar; }

    void setTheme(bool isDarkTheme) { m_isDarkTheme = isDarkTheme; }
    bool getIsDarkTheme() { return m_isDarkTheme; }

    void setKeysManagerPtr(KeysManagerPtr keysManagerPtr) { m_keysManager = keysManagerPtr; }
    KeysManagerPtr getKeysManagerPtr() const { return m_keysManager; }

    void setCurrentVersionNumber(const std::string& versionNumber) { m_currentVersionNumber = versionNumber; }
    const std::string& getCurrentVersionNumber() const { return m_currentVersionNumber; }

    void setVersionNumberToUpdate(const std::string& versionNumber) { m_versionNumberToUpdate = versionNumber; }
    const std::string& getVersionNumberToUpdate() const { return m_versionNumberToUpdate; }

private:
    std::string m_loadedConfigPath;
    std::string m_currentVersionNumber;
    std::string m_versionNumberToUpdate;

    std::string m_myUID;
    std::string m_myPasswordHash;
    std::string m_myLoginHash;
    std::string m_myLogin;
    std::string m_myName;
    bool m_isHasAvatar;
    AvatarPtr m_myAvatar;

    bool m_isHidden;
    bool m_isDarkTheme;

    KeysManagerPtr m_keysManager;

    // JSON keys and fixed string literals
    static constexpr const char* CURRENT_VERSION_NUMBER = "currentVersionNumber";
    static constexpr const char* MY_UID = "myUID";
    static constexpr const char* PASSWORD_HASH = "passwordHash";
    static constexpr const char* LOGIN_HASH = "loginHash";
    static constexpr const char* LOGIN = "login";
    static constexpr const char* NAME = "name";
    static constexpr const char* IS_HAS_AVATAR = "isHasAvatar";
    static constexpr const char* MY_AVATAR_PATH = "myAvatarPath";
    static constexpr const char* IS_HIDDEN = "isHidden";
    static constexpr const char* IS_DARK_THEME = "isDarkTheme";
    static constexpr const char* PUBLIC_KEY = "publicKey";
    static constexpr const char* PRIVATE_KEY = "privateKey";
    static constexpr const char* ENCRYPTED_MAIN_CONFIG_KEY = "encryptedMainConfigKey";
    static constexpr const char* CHATS_ARRAY = "chatsArray";
    static constexpr const char* FRIEND_UID = "friendUID";

    static constexpr const char* ONE_STR = "1";
    static constexpr const char* ZERO_STR = "0";
    static constexpr const char* JSON_EXT = ".json";
};

