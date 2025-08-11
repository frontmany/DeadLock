#include "keysManager.h"

const CryptoPP::SecByteBlock& KeysManager::getAvatarsKey() const {
    return m_avatarsKey;
}

void KeysManager::setAvatarsKey(const CryptoPP::SecByteBlock& avatarsKey) {
    m_avatarsKey = avatarsKey;
}



const CryptoPP::RSA::PrivateKey& KeysManager::getMyPrivateKey() const {
    return m_myPrivateKey;
}

void KeysManager::setMyPrivateKey(const CryptoPP::RSA::PrivateKey& myPrivateKey) {
    m_myPrivateKey = myPrivateKey;
}



const CryptoPP::RSA::PublicKey& KeysManager::getMyPublicKey() const {
    return m_myPublicKey;
}

void KeysManager::setMyPublicKey(const CryptoPP::RSA::PublicKey& myPublicKey) {
    m_myPublicKey = myPublicKey;
}



const CryptoPP::RSA::PublicKey& KeysManager::getServerPublicKey() const {
    return m_serverPublicKey;
}

void KeysManager::setServerPublicKey(const CryptoPP::RSA::PublicKey& serverPublicKey) {
    m_serverPublicKey = serverPublicKey;
}