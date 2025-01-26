#pragma once

#include<iostream>
#include<sstream>
#include<string>
#include <fstream>
#include <cstring>

#include "photo.h"

class User {
public:
    User(std::string login, std::string password, std::string name)
        : m_login(login), m_password(password),
        m_name(name){}

    User() {}

    const std::string& getLogin() const { return m_login; }
    void setLogin(const std::string& login) {m_login = login; }

    const std::string& getPassword() const { return m_password; }
    void setPassword(const std::string& password) { m_password = password; }

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    const Photo& getPhoto() const { return m_photo; }
    void setPhoto(const Photo& photo) { m_photo = photo;}

    const bool getIsHasPhoto() const { return m_isHasPhoto; }
    void setIsHasPhoto(bool isHasPhoto) { m_isHasPhoto = isHasPhoto; }

    const std::string& getLastSeen() const { return m_lastSeen; }
    void setLastSeen(const std::string& lastSeen) { m_lastSeen = lastSeen; }

    std::vector<std::pair<std::string, std::string>>& getUserFriendsVec();

private:
    bool				m_isHasPhoto = false;

    std::string			m_lastSeen;
    std::string			m_name;
    std::string			m_login;
    std::string			m_password;
    Photo			    m_photo;
    std::vector<std::pair<std::string, std::string>> m_vec_user_friends_logins;
};
 
