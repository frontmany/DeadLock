#pragma once

#include <string>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <iostream>
#include <QString>
#include <codecvt>
#include <locale>
#include <QFile>
#include <QDir>
#include <unordered_map>
#include <rsa.h>

class Photo;
class Chat;
class Database;
class Client;


class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() { std::cout << "config manager destroed\n"; }
    void updateInConfigFriendLogin(const std::string& oldLogin, const std::string& newLogin);
    void updateConfigFileName(const std::string& oldLogin, const std::string& newLoginHash);
    void deleteFriendChatInConfig(const std::string& friendLogin);

    bool checkIsAutoLogin();
    bool undoAutoLogin();

    void save(const CryptoPP::RSA::PublicKey& myPublicKey, const CryptoPP::RSA::PrivateKey& myPrivateKey, const std::string& serverKey, std::unordered_map<std::string, Chat*> mapFriendLoginHashToChat, bool isHidden, Database* database);
    bool load(const std::string& fileName, const std::string& specialServerKey, Database* database);

    void loadLoginHash();
    void loadPasswordHash();
    
    //GET && SET
    bool getIsAutoLogin() { return m_is_auto_login; };
    void setIsNeedToAutoLogin(bool  isNeedToAutoLogin) { m_is_auto_login = isNeedToAutoLogin; }

    bool isUndoAutoLogin() { return m_is_undo_auto_login; };
    void setNeedToUndoAutoLogin(bool  isNeedToUndoAutoLogin) { m_is_undo_auto_login = isNeedToUndoAutoLogin; }

    void setMyPasswordHash(const std::string& passwordHash) { m_my_password_hash = passwordHash; }
    const std::string& getMyPasswordHash() const { return m_my_password_hash; }

    void setMyLogin(const std::string& login) { m_my_login = login; }
    const std::string& getMyLogin() const { return m_my_login; }

    void setMyLoginHash(const std::string& loginHash) { m_my_login_hash = loginHash; }
    const std::string& getMyLoginHash() const { return m_my_login_hash; }

    void setMyName(const std::string& name) { m_my_name = name; }
    const std::string& getMyName() const { return m_my_name; }

    void setPhoto(Photo* photo) { m_my_photo = photo; }
    const Photo* getPhoto() const { return m_my_photo; }

    void setIsHasPhoto(bool isHasPhoto) { m_is_has_photo = isHasPhoto; }
    const bool getIsHasPhoto() const { return m_is_has_photo; }

    void setClient(Client* client) { m_client = client; }
    Client* getClient() const { return m_client; }

private:
    bool checkIsPasswordHashPresentInMyConfig() const;
    CryptoPP::SecByteBlock getChatConfigKey(const std::string& login);

private:
    CryptoPP::SecByteBlock m_AESE_configKey;

    std::string m_my_password_hash;
    std::string m_auto_login_path;
    std::string m_my_login_hash;
    bool m_is_undo_auto_login;
    std::string m_my_login;
    std::string m_my_name;
    bool m_is_auto_login;
    bool m_is_has_photo;
    Photo* m_my_photo;

    Client* m_client;
};

