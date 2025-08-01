#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <locale>
#include <codecvt>

#include <filesystem>

#include "base64_my.h"
#include "rsa.h"

class Photo {
public:
    Photo(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& photoPath = "");

    const std::string& getPhotoPath() const { return m_photoPath; }
    void setPhotoPath(const std::string& photoPath) { m_photoPath = photoPath; updateSize(); }

    const std::string& getBinaryData() const { return m_binaryData; }
    void setBinaryData(const std::string& data) { m_binaryData = data; }

    static Photo* deserializeAndSaveOnDisc(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& data, std::string login);
    static Photo* deserializeWithoutSaveOnDisc(const CryptoPP::RSA::PrivateKey& privateKey, const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& data);

    std::string encryptForServerBase64(const CryptoPP::RSA::PublicKey& serverPublicKey) const;
    const std::size_t getSize() const { return m_size; }
    void loadBinaryDataFromPc();

private:
    Photo() { m_size = 0; }
    static std::string decryptPhotoData(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& encryptedPackage);
    void updateSize();

private:
    CryptoPP::RSA::PrivateKey m_private_key;
    std::string m_binaryData;
    std::string m_photoPath;
    std::size_t m_size;
};