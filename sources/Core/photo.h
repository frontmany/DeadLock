#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include "base64.h"

class Photo {
public:
    Photo(const std::string& photoPath = "");

    const std::string& getPhotoPath() const { return m_photoPath; }
    void setPhotoPath(const std::string& photoPath) { m_photoPath = photoPath; updateSize(); }
    const std::size_t getSize() const { return m_size; }

    std::string serialize() const;
    static Photo* deserialize(const std::string& data, std::string login);

private:
    void updateSize();

private:
    std::string m_photoPath;
    std::size_t m_size;
};