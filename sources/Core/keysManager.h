#pragma once

#include <rsa.h>

class KeysManager {
public:
    KeysManager() = default;

    // GET & SET
    const CryptoPP::SecByteBlock& getAvatarsKey() const;
    void setAvatarsKey(const CryptoPP::SecByteBlock& avatarsKey);

    const CryptoPP::RSA::PrivateKey& getMyPrivateKey() const;
    void setMyPrivateKey(const CryptoPP::RSA::PrivateKey& myPrivateKey);

    const CryptoPP::RSA::PublicKey& getMyPublicKey() const;
    void setMyPublicKey(const CryptoPP::RSA::PublicKey& myPublicKey);

    const CryptoPP::RSA::PublicKey& getServerPublicKey() const;
    void setServerPublicKey(const CryptoPP::RSA::PublicKey& serverPublicKey);

    void setServerSpecialKey(const std::string& serverSpecialKey) { m_serverSpecialKey = serverSpecialKey; }
    const std::string& getServerSpecialKey() const { return m_serverSpecialKey; }

private:
    CryptoPP::SecByteBlock m_avatarsKey;
    CryptoPP::RSA::PrivateKey m_myPrivateKey;
    CryptoPP::RSA::PublicKey m_myPublicKey;
    CryptoPP::RSA::PublicKey m_serverPublicKey;
    std::string m_serverSpecialKey;
};