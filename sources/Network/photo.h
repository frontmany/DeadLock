#include<iostream>
#include<fstream>
#include<vector>
#include<filesystem>
#include<windows.h>

class Photo {
public:
    Photo(const std::string& photoPath = "");

    const std::string& getPhotoPath() const { return m_photoPath; }
    void setPhotoPath(std::string& photoPath) { m_photoPath = photoPath; updateSize(); }
    const std::size_t getSize() const { return m_size; }
    void saveToFile() const;

    std::string serialize() const;
    static Photo* deserialize(const std::string& data);

private:
    static std::string wideStringToString(const WCHAR* wideStr);
    void updateSize();

private:
    std::string m_photoPath;
    std::size_t m_size;
};