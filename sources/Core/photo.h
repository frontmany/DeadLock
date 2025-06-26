#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>

#include "base64_my.h"

class Photo {
public:
    Photo(const std::string& photoPath = "");

    const std::string& getPhotoPath() const { return m_photoPath; }
    void setPhotoPath(const std::string& photoPath) { m_photoPath = photoPath; updateSize(); }

    const std::string& getBinaryData() const { return m_binaryData; }
    void setBinaryData(const std::string& data) { m_binaryData = data; }

    std::string serialize() const;
    static Photo* deserializeAndSaveOnDisc(const std::string& data, std::string login);
    static Photo* deserializeWithoutSaveOnDisc(const std::string& data);

    const std::size_t getSize() const { return m_size; }

private:
    void updateSize();

private:
    std::string m_photoPath;
    std::size_t m_size;
    std::string m_binaryData;
};