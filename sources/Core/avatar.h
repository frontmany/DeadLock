#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <locale>
#include <codecvt>

#include <filesystem>

#include "rsa.h"

class Avatar {
public:
    Avatar(const CryptoPP::SecByteBlock& avatarsKey, const std::string& avatarPath);

    void setNewPath(const std::string& newAvatarPath);

    const std::string& getPath() const { return m_avatarPath; }
    const std::size_t getSize() const { return m_size; }
    const std::string& getBinaryData() const { return m_binaryData; }
    const std::size_t getEncryptedSize() const { return m_encryptedSize; }

private:
    void update(const CryptoPP::SecByteBlock& avatarsKey);

private:
    std::string m_binaryData;
    std::string m_avatarPath;
    std::size_t m_size;
    std::size_t m_encryptedSize;
};