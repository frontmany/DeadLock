#include"utility.h"



bool Utility::verifyPassword(const std::string& password, const std::string& storedHash) {

    try {
        std::string salt = extractSalt(storedHash);
        std::string storedHashedPassword = extractHash(storedHash);
        std::string hashedPassword = hashPassword(password, salt);

        return hashedPassword == storedHashedPassword;
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Error verifying password: " << e.what() << std::endl;
        return false; // Or handle the error differently
    }
}


std::string Utility::getCurrentDateTime() {
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

// Convert a byte array to a hex string
std::string Utility::byteArrayToHexString(const BYTE* data, size_t dataLength) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < dataLength; ++i) {
        ss << std::setw(2) << static_cast<int>(data[i]);
    }
    return ss.str();
}

// New generateSalt function (generates 16 bytes = 32 hex characters)
std::string Utility::generateSalt() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, 255);

    BYTE saltBytes[16]; // 16 bytes = 128 bits
    for (int i = 0; i < 16; ++i) {
        saltBytes[i] = static_cast<BYTE>(distribution(gen));
    }

    return byteArrayToHexString(saltBytes, 16); // Convert byte array to hex string
}

// Function to extract the salt from the stored hash (assuming salt is stored as the first 32 characters = 16 bytes hex)
std::string Utility::extractSalt(const std::string& storedHash) {
    if (storedHash.length() < 33) {
        throw std::runtime_error("Invalid stored hash format (salt missing)");
    }
    return storedHash.substr(0, 32); // Salt is the first 32 characters
}

// Function to extract the hash from the stored hash (assuming salt is stored as the first 32 characters, and then ':' and then hash)
std::string Utility::extractHash(const std::string& storedHash) {

    size_t delimiterPos = storedHash.find(':');
    if (delimiterPos == std::string::npos) {
        throw std::runtime_error("Invalid stored hash format (delimiter missing)");
    }
    if (delimiterPos + 1 >= storedHash.length()) {
        throw std::runtime_error("Invalid stored hash format (hash missing)");
    }
    return storedHash.substr(delimiterPos + 1); // Hash is after the salt and ':'
}


// Rewritten bcryptHash function (still doesn't truly use bcrypt but uses SHA256 with a salt)
std::string Utility::hashPassword(const std::string& password, const std::string& salt) {
    // **WARNING: This is NOT true bcrypt!  It's SHA256 with a salt.**
    // Proper bcrypt implementation requires using a dedicated bcrypt library.

    BCRYPT_ALG_HANDLE hAlg = nullptr;
    BCRYPT_HASH_HANDLE hHash = nullptr;
    DWORD hashLength = 0;
    BYTE* hash = nullptr;

    std::string saltedPassword = salt + password; // Salt before the password

    // Open the algorithm provider for SHA256
    NTSTATUS status = BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, 0);


    // Create the hash object
    status = BCryptCreateHash(hAlg, &hHash, nullptr, 0, nullptr, 0, 0); // No salt to BCryptCreateHash


    // Hash the data (salted password)
    status = BCryptHashData(hHash, (PBYTE)saltedPassword.c_str(), saltedPassword.length(), 0);


    // Get the hash length
    status = BCryptGetProperty(hAlg, BCRYPT_HASH_LENGTH, (PBYTE)&hashLength, sizeof(DWORD), nullptr, 0);


    // Allocate memory for the hash
    hash = new BYTE[hashLength];

    // Finish the hash
    status = BCryptFinishHash(hHash, hash, hashLength, 0);


    // Convert hash to hex string
    std::string hashedPassword = byteArrayToHexString(hash, hashLength);

    // Clean up
    delete[] hash;
    BCryptDestroyHash(hHash);
    BCryptCloseAlgorithmProvider(hAlg, 0);

    return hashedPassword;
}

std::string Utility::wideStringToString(const WCHAR* wideStr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &str[0], size_needed, nullptr, nullptr);
    return str;
}

QString Utility::getSaveDir() {
    WCHAR username[256];
    DWORD username_len = sizeof(username) / sizeof(username[0]);

    if (!GetUserNameW(username, &username_len)) {
        std::cout << "No User data" << std::endl;
        return QString();
    }

    std::string usernameStr = wideStringToString(username);
    usernameStr.erase(std::remove(usernameStr.begin(), usernameStr.end(), '\0'), usernameStr.end());

    std::string saveDirectory = "C:\\Users\\" + usernameStr + "\\Documents\\Data_Air_Gram";


    if (!std::filesystem::exists(saveDirectory)) {
        std::filesystem::create_directories(saveDirectory);
    }

    return QString::fromStdString(saveDirectory);
}

std::string Utility::getCurrentTime() {
    auto now = std::chrono::system_clock::now();

    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time_t), "%H:%M");

    return oss.str();
}

std::string Utility::generateId() {
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
