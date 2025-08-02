#include "avatar.h"
#include "utility.h"

Avatar::Avatar(const CryptoPP::SecByteBlock& avatarsKey, const std::string& photoPath)
    : m_photoPath(photoPath), m_size(0) {
    if (photoPath != "") {
        update(avatarsKey);
    }
}

void Avatar::update(const CryptoPP::SecByteBlock& avatarsKey) {
    std::ifstream file(m_photoPath, std::ios::binary);
    if (!file) {
        m_size = 0;
        return;
    }

    try {
        std::string encryptedData(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );
        m_encryptedSize = encryptedData.size();

        m_binaryData = utility::AESDecrypt(avatarsKey, encryptedData);
       
        m_size = m_binaryData.size();
    }
    catch (...) {
        std::cerr << "error (avatar)\n";
        m_size = 0;
    }
}