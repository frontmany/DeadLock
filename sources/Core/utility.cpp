#include "utility.h"
#include "Windows.h"
#include "chat.h"
#include "base64_my.h"

#include "secblock.h"
#include <rsa.h>                 
#include <hex.h>                    
#include <osrng.h>       
#include <sha.h>                  
#include <filters.h>               
#include <base64.h>                 
#include <gcm.h>                 
#include <aes.h>                    
#include <modes.h>                  
#include <queue.h>                                
#include <cryptlib.h> 
#include <oaep.h> 


std::string utility::getFileSavePath(const std::string& fileName) {
    namespace fs = std::filesystem;

#ifdef _WIN32
    std::string downloadsPath = std::string(std::getenv("USERPROFILE")) + "\\Downloads\\";
#else
    std::string downloadsPath = std::string(std::getenv("HOME")) + "/Downloads/";
#endif

    std::string deadlockDir = downloadsPath + "Deadlock Messenger";
    if (!fs::exists(deadlockDir)) {
        fs::create_directory(deadlockDir);
    }

    std::string filePath = deadlockDir + "/" + fileName;
#ifdef _WIN32
    filePath = deadlockDir + "\\" + fileName;
#endif

    int counter = 1;
    while (fs::exists(filePath)) {
        size_t dotPos = fileName.find_last_of('.');
        std::string nameWithoutExt = fileName.substr(0, dotPos);
        std::string extension = (dotPos != std::string::npos) ? fileName.substr(dotPos) : "";

#ifdef _WIN32
        filePath = deadlockDir + "\\" + nameWithoutExt + " (" + std::to_string(counter) + ")" + extension;
#else
        filePath = deadlockDir + "/" + nameWithoutExt + " (" + std::to_string(counter) + ")" + extension;
#endif
        counter++;
    }

    return filePath;
}

std::string utility::getUpdateTemporaryPath(const std::string& fileName) {
    const std::filesystem::path tempDir = "./updaterTemporary";

    if (!std::filesystem::exists(tempDir)) {
        std::filesystem::create_directory(tempDir);
    }

    return (tempDir / (fileName + ".exe")).string();
}

std::string utility::getCurrentFullDateAndTime() {
    std::time_t now = std::time(0);
    std::tm* ltm = std::localtime(&now);

    std::stringstream ss;
    ss << std::setw(4) << std::setfill('0') << ltm->tm_year + 1900 << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mon + 1 << "-"
        << std::setw(2) << std::setfill('0') << ltm->tm_mday << " "
        << std::setw(2) << std::setfill('0') << ltm->tm_hour << ":"
        << std::setw(2) << std::setfill('0') << ltm->tm_min << ":"
        << std::setw(2) << std::setfill('0') << ltm->tm_sec;

    return "last seen: " + ss.str();
}

std::string utility::getConfigsAndPhotosDirectory() {
    std::string saveDirectory = "./Data_Air_Gram";

    if (!std::filesystem::exists(saveDirectory)) {
        std::filesystem::create_directories(saveDirectory);
    }

    return saveDirectory;
}

std::string utility::getAvatarPreviewsDirectory() {
    std::string saveDirectory = "./Avatars_Tmp";

    if (!std::filesystem::exists(saveDirectory)) {
        std::filesystem::create_directories(saveDirectory);
    }

    return saveDirectory;
}

std::string utility::generateId() {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "Ошибка инициализации COM: " << std::hex << hr << std::endl;

        return "";
    }

    GUID guid;
    HRESULT result = CoCreateGuid(&guid);
    if (FAILED(result)) {
        std::cerr << "Ошибка при генерации GUID: " << std::hex << result << std::endl;
        CoUninitialize();

        return "";
    }

    char buffer[37];
    snprintf(buffer, sizeof(buffer),
        "{%08X-%04X-%04X-%04X-%012llX}",
        guid.Data1, guid.Data2, guid.Data3,
        (guid.Data4[0] << 8) | guid.Data4[1],
        *(reinterpret_cast<const unsigned long long*>(guid.Data4 + 2)));
    CoUninitialize();

    return std::string(buffer);
}

std::string utility::parseDate(const std::string& fullDate) {
    if (fullDate == "last seen: N/A") {
        return fullDate;
    }
    if (fullDate == "online") {
        return fullDate;
    }
    if (fullDate == "recently") {
        return fullDate;
    }
    if (fullDate == "requested status of unknown user") {
        return "online";
    }
    if (fullDate.find("last seen:") != 0) {
        return "Invalid date format";
    }

    std::string dateStr = fullDate.substr(10);
    dateStr.erase(dateStr.begin(), std::find_if(dateStr.begin(), dateStr.end(), [](int ch) { return !std::isspace(ch); }));
    dateStr.erase(std::find_if(dateStr.rbegin(), dateStr.rend(), [](int ch) { return !std::isspace(ch); }).base(), dateStr.end());

    if (dateStr.empty()) {
        return "Invalid date format";
    }

    std::time_t now = std::time(nullptr);
    std::tm* localNow = std::localtime(&now);
    int currentYear = localNow->tm_year + 1900;

    size_t spacePos = dateStr.find(' ');
    if (spacePos == std::string::npos) {
        return "Invalid date format";
    }

    std::string datePart = dateStr.substr(0, spacePos);
    std::string timePart = dateStr.substr(spacePos + 1);

    std::vector<std::string> ymd;
    std::istringstream dateStream(datePart);
    std::string token;

    while (std::getline(dateStream, token, '-')) {
        ymd.push_back(token);
    }

    if (ymd.size() < 3) {
        return "Invalid date format";
    }

    int year, month, day;
    try {
        year = std::stoi(ymd[0]);
        month = std::stoi(ymd[1]);
        day = std::stoi(ymd[2]);
    }
    catch (...) {
        return "Invalid date format";
    }

    if (year == currentYear) {
        return "last seen: " + timePart;
    }
    else {
        std::ostringstream oss;
        oss << "last seen: " << year << "-"
            << std::setw(2) << std::setfill('0') << month << "-"
            << std::setw(2) << std::setfill('0') << day;
        return oss.str();
    }
}

std::string utility::calculateHash(const std::string& text) {
    using namespace CryptoPP;

    SHA256 hash;

    std::string digest;
    StringSource ss(
        text,
        true,
        new HashFilter(
            hash,
            new HexEncoder(
                new StringSink(digest)
            )
        )
    );

    return digest;
}

void utility::incrementAllChatLayoutIndexes(std::unordered_map<std::string, Chat*>& loginToChatMap) {
    for (auto& pair : loginToChatMap) {
        Chat* chatTmp = pair.second;
        chatTmp->setLayoutIndex(chatTmp->getLayoutIndex() + 1);
    }
}

void utility::increasePreviousChatIndexes(std::unordered_map<std::string, Chat*>& loginToChatMap, Chat* chat) {
    for (auto pair : loginToChatMap) {
        Chat* currentChat = pair.second;
        if (currentChat->getFriendLogin() == chat->getFriendLogin()) {
            continue;
        }
        else if (currentChat->getLayoutIndex() < chat->getLayoutIndex()) {
            currentChat->setLayoutIndex(currentChat->getLayoutIndex() + 1);
        }
    }
}

void utility::decreaseFollowingChatIndexes(std::unordered_map<std::string, Chat*>& loginToChatMap, Chat* chat) {
    for (auto [login, currentChat] : loginToChatMap) {
        if (currentChat->getFriendLogin() == chat->getFriendLogin()) {
            continue;
        }
        else if (currentChat->getLayoutIndex() > chat->getLayoutIndex()) {
            currentChat->setLayoutIndex(currentChat->getLayoutIndex() - 1);
        }
    }
}


std::string utility::getCurrentTime() {
    auto now = std::chrono::system_clock::now();

    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time_t), "%H:%M");

    return oss.str();
}

bool utility::isDarkMode() {
    HKEY hKey;
    const TCHAR* subKey = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");
    const TCHAR* valueNameApps = TEXT("AppsUseLightTheme");
    const TCHAR* valueNameSystem = TEXT("SystemUsesLightTheme");
    DWORD value;
    DWORD size = sizeof(value);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueEx(hKey, valueNameApps, NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0;
        }
        RegCloseKey(hKey);
    }
    return false;
}

qreal utility::getDeviceScaleFactor() {
    QScreen* screen = QApplication::primaryScreen();
    return screen->devicePixelRatio();
}

bool utility::isApplicationAlreadyRunning() {
    static QSharedMemory sharedMemory("YourAppUniqueKey_12345");
    if (sharedMemory.attach()) {
        return true;
    }

    if (sharedMemory.create(1)) {
        return false;
    }

    qWarning() << "Shared memory error:" << sharedMemory.errorString();
    return false;
}

int utility::getScaledSize(int baseSize) {
    static qreal scale = getDeviceScaleFactor();
    return static_cast<int>(baseSize / scale);
}

bool utility::isHasInternetConnection() {
#ifdef _WIN32
    const char* ping_cmd = "ping -n 1 1.1.1.1 > nul";
#else
    const char* ping_cmd = "ping -c 1 1.1.1.1 > /dev/null";
#endif

    int result = std::system(ping_cmd);
    return (result == 0);
}




using namespace CryptoPP;


void utility::generateRSAKeyPair(CryptoPP::RSA::PrivateKey& privateKey, CryptoPP::RSA::PublicKey& publicKey) {
    CryptoPP::AutoSeededRandomPool rng;

    privateKey.Initialize(rng, 3072); 

    publicKey = CryptoPP::RSA::PublicKey(privateKey);
}

void utility::generateAESKey(SecByteBlock& key) {
    AutoSeededRandomPool rng;
    key = SecByteBlock(32);
    rng.GenerateBlock(key, key.size());
}

std::string utility::RSAEncryptKey(const CryptoPP::RSA::PublicKey& publicKey, const CryptoPP::SecByteBlock& data) {
    try {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::RSAES<CryptoPP::OAEP<CryptoPP::SHA256>>::Encryptor encryptor(publicKey);

        std::string cipher;
        CryptoPP::StringSource(
            data.data(),
            data.size(),
            true,
            new CryptoPP::PK_EncryptorFilter(
                rng,
                encryptor,
                new CryptoPP::StringSink(cipher)
            )
        );

        std::string base64Cipher;
        CryptoPP::StringSource(
            cipher,
            true,
            new CryptoPP::Base64Encoder(
                new CryptoPP::StringSink(base64Cipher),
                false 
            )
        );

        return base64Cipher;
    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error(std::string("RSA encryption error: ") + e.what());
    }
}

CryptoPP::SecByteBlock utility::RSADecryptKey(const CryptoPP::RSA::PrivateKey& privateKey,
    const std::string& base64Cipher) {
    try {
        CryptoPP::AutoSeededRandomPool rng;
        CryptoPP::RSAES<CryptoPP::OAEP<CryptoPP::SHA256>>::Decryptor decryptor(privateKey);

        std::string cipher;
        CryptoPP::StringSource(
            base64Cipher,
            true,
            new CryptoPP::Base64Decoder(
                new CryptoPP::StringSink(cipher)
            )
        );

        if (cipher.size() != privateKey.GetModulus().ByteCount()) {
            throw std::runtime_error("Invalid cipher size after Base64 decoding");
        }

        CryptoPP::SecByteBlock decrypted(decryptor.MaxPlaintextLength(cipher.size()));

        CryptoPP::DecodingResult result = decryptor.Decrypt(
            rng,
            reinterpret_cast<const CryptoPP::byte*>(cipher.data()),
            cipher.size(),
            decrypted.data()
        );

        if (!result.isValidCoding) {
            throw std::runtime_error("Failed to decrypt RSA data");
        }

        decrypted.resize(result.messageLength);
        return decrypted;

    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error(std::string("RSA decryption error: ") + e.what());
    }
}

std::string utility::AESEncrypt(const SecByteBlock& key, const std::string& plain) {
    AutoSeededRandomPool rng;

    const size_t ivSize = 12;
    byte iv[ivSize];
    rng.GenerateBlock(iv, ivSize);

    GCM<AES>::Encryption enc;
    enc.SetKeyWithIV(key, key.size(), iv, ivSize);

    std::string cipher;
    AuthenticatedEncryptionFilter ef(enc, new StringSink(cipher));
    StringSource ss(plain, true, new Redirector(ef));

    std::string binaryResult;
    binaryResult.assign(reinterpret_cast<const char*>(iv), ivSize);
    binaryResult += cipher;

    std::string base64Result;
    CryptoPP::StringSource ss2(
        binaryResult, true,
        new CryptoPP::Base64Encoder(
            new CryptoPP::StringSink(base64Result),
            false
        )
    );

    return base64Result;
}

std::string utility::AESDecrypt(const SecByteBlock& key, const std::string& cipher) {
    std::string cipherDecoded;
    CryptoPP::StringSource ss1(
        cipher, true,
        new CryptoPP::Base64Decoder(
            new CryptoPP::StringSink(cipherDecoded)
        )
    );

    const size_t ivSize = 12;
    if (cipherDecoded.size() < ivSize + 16 + 1)
        throw std::runtime_error("Invalid ciphertext");

    byte iv[ivSize];
    std::memcpy(iv, cipherDecoded.data(), ivSize);

    std::string encrypted = cipherDecoded.substr(ivSize);

    GCM<AES>::Decryption dec;
    dec.SetKeyWithIV(key, key.size(), iv, ivSize);

    std::string plain;
    AuthenticatedDecryptionFilter df(dec, new StringSink(plain));

    try {
        StringSource ss2(encrypted, true, new Redirector(df));
    }
    catch (const Exception& e) {
        throw std::runtime_error("Decryption failed: " + std::string(e.what()));
    }

    return plain;
}

std::array<char, 8220> utility::AESEncrypt(const CryptoPP::SecByteBlock& key,
    const std::array<char, 8192>& bytesArray) {
    CryptoPP::AutoSeededRandomPool rng;
    const size_t ivSize = 12;
    std::array<CryptoPP::byte, ivSize> iv;
    rng.GenerateBlock(iv.data(), ivSize);

    CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
    enc.SetKeyWithIV(key, key.size(), iv.data(), ivSize);

    std::string cipher;
    CryptoPP::AuthenticatedEncryptionFilter ef(enc, new CryptoPP::StringSink(cipher));

    CryptoPP::StringSource ss(
        reinterpret_cast<const CryptoPP::byte*>(bytesArray.data()),
        bytesArray.size(),
        true,
        new CryptoPP::Redirector(ef)
    );

    std::array<char, 8220> result;
    if (ivSize + cipher.size() > 8220 || ivSize + cipher.size() < 8220) {
        throw std::runtime_error("Encrypted data too large for output array");
    }
    else if (ivSize + cipher.size() == 8220){
        std::memcpy(result.data(), iv.data(), ivSize);
        std::memcpy(result.data() + ivSize, cipher.data(), cipher.size());
    }
    return result;
}

std::array<char, 8192> utility::AESDecrypt(const CryptoPP::SecByteBlock& key,
    const std::array<char, 8220>& cipherBytesArray) 
{
    const size_t ivSize = 12;
    if (cipherBytesArray.size() < ivSize + 16 + 1) {
        throw std::runtime_error("Invalid ciphertext");
    }

    std::array<CryptoPP::byte, ivSize> iv;
    std::memcpy(iv.data(), cipherBytesArray.data(), ivSize);

    std::string encrypted;
    encrypted.assign(cipherBytesArray.data() + ivSize, cipherBytesArray.size() - ivSize);

    CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
    dec.SetKeyWithIV(key, key.size(), iv.data(), ivSize);

    std::string plain;
    CryptoPP::AuthenticatedDecryptionFilter df(dec, new CryptoPP::StringSink(plain));

    try {
        CryptoPP::StringSource ss(encrypted, true, new CryptoPP::Redirector(df));
    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Decryption failed: " + std::string(e.what()));
    }

    std::array<char, 8192> result;
    if (plain.size() > 8192) {
        throw std::runtime_error("Decrypted data too large for output array");
    }
    std::memcpy(result.data(), plain.data(), plain.size());

    return result;
}

std::string utility::encryptWithServerKey(const std::string& plaintext, const std::string& keyStr) {
    try {
        if (keyStr.empty() || !keyStr.data()) { 
            throw std::runtime_error("Key is empty or invalid");
        }
        if (plaintext.empty()) {
            throw std::runtime_error("Plaintext must not be empty");
        }

        CryptoPP::SecByteBlock key(16);
        size_t keyLength = std::min<size_t>(keyStr.size(), key.size());
        std::memcpy(key, keyStr.data(), keyLength);

        if (keyStr.size() < key.size()) {
            std::memset(key + keyLength, 0, key.size() - keyLength);
        }

        CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };

        std::string ciphertext;
        CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption encryptor;
        encryptor.SetKeyWithIV(key, key.size(), iv);

        CryptoPP::StringSource(plaintext, true,
            new CryptoPP::StreamTransformationFilter(encryptor,
                new CryptoPP::StringSink(ciphertext)
            ));

        return base64_encode(ciphertext);
    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Crypto++ error: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Error: " + std::string(e.what()));
    }
}

std::string utility::decryptWithServerKey(const std::string& ciphertext, const std::string& keyStr) {
    try {
        std::string decodedCiphertext = base64_decode(ciphertext);

        if (keyStr.empty()) {
            throw std::runtime_error("Key must not be empty");
        }
        if (decodedCiphertext.empty()) {
            throw std::runtime_error("Ciphertext must not be empty");
        }

        CryptoPP::SecByteBlock key(16);
        size_t keyLength = std::min<size_t>(keyStr.size(), key.size());
        std::memcpy(key, keyStr.data(), keyLength);

        if (keyStr.size() < key.size()) {
            std::memset(key + keyLength, 0, key.size() - keyLength);
        }

        CryptoPP::byte iv[CryptoPP::AES::BLOCKSIZE] = { 0 };

        std::string decrypted;
        CryptoPP::CBC_Mode<CryptoPP::AES>::Decryption decryptor;
        decryptor.SetKeyWithIV(key, key.size(), iv);

        CryptoPP::StringSource(decodedCiphertext, true,
            new CryptoPP::StreamTransformationFilter(decryptor,
                new CryptoPP::StringSink(decrypted)
            ));

        return decrypted;
    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Crypto++ decryption error: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        throw std::runtime_error("Decryption error: " + std::string(e.what()));
    }
}


std::string utility::serializeAESKey(const CryptoPP::SecByteBlock& key) {
    std::string encoded;

    CryptoPP::StringSource ss(
        key.data(),
        key.size(),
        true,
        new CryptoPP::Base64Encoder(
            new CryptoPP::StringSink(encoded),
            false 
        )
    );

    return encoded;
}

CryptoPP::SecByteBlock utility::deserializeAESKey(const std::string& keyStr) {
    try {
        std::string decoded;

        CryptoPP::StringSource ss(
            keyStr,
            true,
            new CryptoPP::Base64Decoder(
                new CryptoPP::StringSink(decoded)
            )
        );

        CryptoPP::SecByteBlock key(
            reinterpret_cast<const CryptoPP::byte*>(decoded.data()),
            decoded.size()
        );

        return key;
    }
    catch (const CryptoPP::Exception& e) {
        throw std::runtime_error("Failed to deserialize AES key: " + std::string(e.what()));
    }
}

std::string utility::serializePublicKey(const RSA::PublicKey& key) {
    std::string encoded;
    ByteQueue queue;
    key.Save(queue);

    StringSink sink(encoded);
    Base64Encoder encoder(new Redirector(sink), false);
    queue.CopyTo(encoder);
    encoder.MessageEnd();

    return encoded;
}

RSA::PublicKey utility::deserializePublicKey(const std::string& keyStr) {
    try {
        ByteQueue queue;
        StringSource ss(keyStr, true,
            new Base64Decoder(
                new Redirector(queue)
            ));

        RSA::PublicKey key;
        key.Load(queue);

        return key;
    }
    catch (const Exception& e) {
        throw std::runtime_error("Failed to deserialize public key: " + std::string(e.what()));
    }
}

std::string utility::serializePrivateKey(const RSA::PrivateKey& key) {
    std::string encoded;
    ByteQueue queue;
    key.Save(queue);

    StringSink sink(encoded);
    Base64Encoder encoder(new Redirector(sink), false);
    queue.CopyTo(encoder);
    encoder.MessageEnd();

    return encoded;
}

RSA::PrivateKey utility::deserializePrivateKey(const std::string& keyStr) {
    try {
        ByteQueue queue;
        StringSource ss(keyStr, true,
            new Base64Decoder(
                new Redirector(queue)
            ));

        RSA::PrivateKey key;
        key.Load(queue);
        return key;
    }
    catch (const Exception& e) {
        throw std::runtime_error("Failed to deserialize private key: " + std::string(e.what()));
    }
}

bool utility::validateKeys(const RSA::PublicKey& publicKey, const RSA::PrivateKey& privateKey)  {
    try {
        AutoSeededRandomPool rng;
        std::string testMsg = "test message";
        std::string encrypted, decrypted;

        RSAES<OAEP<SHA256>>::Encryptor e(publicKey);
        StringSource ss1(testMsg, true,
            new PK_EncryptorFilter(rng, e,
                new StringSink(encrypted)
            )
        );

        RSAES<OAEP<SHA256>>::Decryptor d(privateKey);
        StringSource ss2(encrypted, true,
            new PK_DecryptorFilter(rng, d,
                new StringSink(decrypted)
            )
        );

        return testMsg == decrypted;
    }
    catch (...) {
        return false;
    }
}

bool utility::validatePrivateKey(const RSA::PrivateKey& key) {
    AutoSeededRandomPool rng;
    return key.GetModulus().BitCount() >= 2048 &&
        !key.GetPrivateExponent().IsZero() &&
        key.Validate(rng, 3);
}

bool utility::validatePublicKey(const RSA::PublicKey& key) {
    try {
        CryptoPP::AutoSeededRandomPool rng;

        const bool validSize = key.GetModulus().BitCount() >= 2048;

        const bool validExponent = !key.GetPublicExponent().IsZero();

        const bool validStructure = key.Validate(rng, 3);

        return validSize && validExponent && validStructure;

    }
    catch (...) {
        return false;
    }
}

