#include<iostream>
#include<fstream>
#include<vector>
#include<filesystem>
#include<windows.h>
#include "base64.h"

class Photo {
public:
    Photo(const std::string& photoPath = "");

    const std::string& getPhotoPath() const { return m_photoPath; }
    void setPhotoPath(const std::string& photoPath) { m_photoPath = photoPath; updateSize(); }
    const std::size_t getSize() const { return m_size; }

    std::string serialize() const;
    static Photo* deserialize(const std::string& data, size_t size, std::string login);
    static Photo* updateOnPC(const std::string& data, size_t size, std::string oldLogin, std::string newLogin);
    void updateNameOnPC(const std::string& oldLogin, const std::string& newLogin) const;

    static std::string wideStringToString(const WCHAR* wideStr);
private:
    void updateSize();

private:
    std::string m_photoPath;
    std::size_t m_size;
};