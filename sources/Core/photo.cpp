#include "photo.h"
#include "utility.h"

Photo::Photo(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& photoPath)
    : m_photoPath(photoPath), m_size(0), m_private_key(privateKey) {
    if (photoPath != "") {
        updateSize();
    }
}

void Photo::updateSize() {
    std::ifstream file(m_photoPath, std::ios::binary);
    if (!file) return;

    try {
        std::string encryptedKey;
        if (!std::getline(file, encryptedKey)) return;

        std::string encryptedData(
            (std::istreambuf_iterator<char>(file)),
            std::istreambuf_iterator<char>()
        );

        CryptoPP::SecByteBlock aesKey = utility::RSADecryptKey(m_private_key, encryptedKey);

        std::string decryptedData = utility::AESDecrypt(aesKey, encryptedData);
        m_size = decryptedData.size();
    }
    catch (...) {
        m_size = 0;
    }
}

std::string Photo::encryptForServerBase64(const CryptoPP::RSA::PublicKey& serverPublicKey) const {
    if (m_photoPath.empty()) {
        std::cerr << "Error: file path is empty\n";
        return "";
    }

    std::ifstream file(m_photoPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open photo file: " + m_photoPath);
    }

    std::string oldEncryptedAesKey;
    if (!std::getline(file, oldEncryptedAesKey)) {
        std::cerr << "Error: cannot read the encrypted AES key from file\n";
        return "";
    }

    std::string oldEncryptedData(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
    file.close();
    CryptoPP::SecByteBlock oldAesKey = utility::RSADecryptKey(m_private_key, oldEncryptedAesKey);
    std::string decryptedFileData = utility::AESDecrypt(oldAesKey, oldEncryptedData);

    try {
        CryptoPP::SecByteBlock newAesKey;
        utility::generateAESKey(newAesKey);

        std::string newEncryptedData = utility::AESEncrypt(newAesKey, decryptedFileData);

        std::string newEncryptedAesKey = utility::RSAEncryptKey(serverPublicKey, newAesKey);

        std::string combined = newEncryptedAesKey + "\n" + newEncryptedData;

        return combined;
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Encryption failed: " + std::string(e.what()));
    }
}

Photo* Photo::deserializeAndSaveOnDisc(const CryptoPP::RSA::PrivateKey& privateKey, const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& data, std::string login) {
    if (data.empty()) {
        return nullptr;
    }

    std::string saveDirectory = "./Data_Air_Gram";
    std::string path = saveDirectory + "/" + utility::calculateHash(login) + ".dph";

    if (!std::filesystem::exists(saveDirectory)) {
        std::filesystem::create_directories(saveDirectory);
    }

    try {
        std::ofstream outFile(path, std::ios::binary);
        if (!outFile) {
            throw std::runtime_error("Failed to open file for writing");
        }

        outFile.write(data.data(), data.size());
        outFile.close();

        std::cout << "Photo saved successfully: " << path << std::endl;
        return new Photo(privateKey, path);

    }
    catch (const std::exception& e) {
        std::cerr << "Failed to save photo: " << e.what() << std::endl;
        return nullptr;
    }
}

Photo* Photo::deserializeWithoutSaveOnDisc(const CryptoPP::RSA::PrivateKey& privateKey, const CryptoPP::RSA::PublicKey& serverPublicKey, const std::string& data) {
    std::string decryptedServerData = decryptPhotoData(privateKey, data);
    
    Photo* photo = new Photo;
    photo->m_binaryData = decryptedServerData;
    photo->m_size = decryptedServerData.size();

    return photo;
}

std::string Photo::decryptPhotoData(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& encryptedPackage) {
    try {
        size_t newline_pos = encryptedPackage.find('\n');
        if (newline_pos == std::string::npos) {
            throw std::runtime_error("Invalid encrypted package format");
        }

        std::string encryptedKey = encryptedPackage.substr(0, newline_pos);
        std::string encryptedData = encryptedPackage.substr(newline_pos + 1);

        CryptoPP::SecByteBlock aesKey = utility::RSADecryptKey(privateKey, encryptedKey);

        return utility::AESDecrypt(aesKey, encryptedData);
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Decryption failed: " + std::string(e.what()));
    }
}