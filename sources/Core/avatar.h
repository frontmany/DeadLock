#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <locale>
#include <codecvt>

#include <filesystem>

#include "base64_my.h"
#include "rsa.h"

class Avatar {
public:
    Avatar(const CryptoPP::SecByteBlock& avatarsKey, const std::string& photoPath = "");

    const std::string& getPhotoPath() const { return m_photoPath; }
    const std::string& getBinaryData() const { return m_binaryData; }
    const std::size_t getSize() const { return m_size; }
    const std::size_t getEncryptedSize() const { return m_encryptedSize; }

private:
    void update(const CryptoPP::SecByteBlock& avatarsKey);

private:
    std::string m_binaryData;
    std::string m_photoPath;
    std::size_t m_size;
    std::size_t m_encryptedSize;
};