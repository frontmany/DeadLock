#pragma once

#include <QScreen>
#include <QApplication>
#include <QSharedMemory>

#include <iostream>
#include <ctime>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>
#include <random>
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include <cassert>  

#include <rsa.h> 

class Chat;

typedef std::shared_ptr<Chat> ChatPtr;

namespace utility {
    // Utility functions
    std::string calculateHash(const std::string& text);
    std::string generateId();
    std::string getConfigsAndPhotosDirectory();
    std::string getAvatarPreviewsDirectory();
    std::string parseDate(const std::string& fulldate);
    std::string escape(const std::string& s);
    std::string unescape(const std::string& s);

    // time
    std::string currentGMTTime();
    std::string localTimeFromGMT(const std::string& gmtTimeStr);

    // System functions
    qreal getDeviceScaleFactor();
    bool isApplicationAlreadyRunning();
    int getScaledSize(int baseSize);
    bool isDarkMode();
    bool isHasInternetConnection();
    std::string getFileSavePath(const std::string& fileName);
    std::string getUpdateTemporaryPath(const std::string& fileName);

    // Chat management
    void incrementAllChatLayoutIndexes(std::unordered_map<std::string, ChatPtr>& uidToChatMap);
    void increasePreviousChatIndexes(std::unordered_map<std::string, ChatPtr>& uidToChatMap, ChatPtr chat);
    void decreaseFollowingChatIndexes(std::unordered_map<std::string, ChatPtr>& uidToChatMap, ChatPtr chat);

    // Cryptography functions
    void generateRSAKeyPair(CryptoPP::RSA::PrivateKey& privateKey, CryptoPP::RSA::PublicKey& publicKey);
    void generateAESKey(CryptoPP::SecByteBlock& key);

    // RSA operations
    std::string RSAEncryptKey(const CryptoPP::RSA::PublicKey& publicKey, const CryptoPP::SecByteBlock& data);
    CryptoPP::SecByteBlock RSADecryptKey(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& cipher);

    // AES operations
    std::string AESEncrypt(const CryptoPP::SecByteBlock& key, const std::string& plain); // return base64 string
    std::string AESDecrypt(const CryptoPP::SecByteBlock& key, const std::string& cipher64);
    std::array<char, 8220> AESEncrypt(const CryptoPP::SecByteBlock& key, const std::array<char, 8192>& bytesArray);
    std::array<char, 8192> AESDecrypt(const CryptoPP::SecByteBlock& key, const std::array<char, 8220>& cipherBytesArray);

    // Key serialization
    std::string serializeAESKey(const CryptoPP::SecByteBlock& key);
    CryptoPP::SecByteBlock deserializeAESKey(const std::string& keyStr);
    std::string serializePublicKey(const CryptoPP::RSA::PublicKey& key);
    CryptoPP::RSA::PublicKey deserializePublicKey(const std::string& keyStr);
    std::string serializePrivateKey(const CryptoPP::RSA::PrivateKey& key);
    CryptoPP::RSA::PrivateKey deserializePrivateKey(const std::string& keyStr);

    // Key validation
    bool validateKeys(const CryptoPP::RSA::PublicKey& publicKey, const CryptoPP::RSA::PrivateKey& privateKey);
    bool validatePublicKey(const CryptoPP::RSA::PublicKey& key);
    bool validatePrivateKey(const CryptoPP::RSA::PrivateKey& key);

    // Server crypto operations
    std::string encryptWithServerKey(const std::string& plaintext, const std::string& keyStr); // return base64 string
    std::string decryptWithServerKey(const std::string& ciphertext, const std::string& keyStr);
}