#include "utility.h"
#include "chat.h"
#include "Windows.h" 
#include <cstdlib>
#include <QSharedMemory>




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



std::string utility::getCurrentDateTime() {
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

std::string utility::getSaveDir() {
    std::string saveDirectory = "./Data_Air_Gram";

    if (!std::filesystem::exists(saveDirectory)) {
        std::filesystem::create_directories(saveDirectory);
    }

    return saveDirectory;
}

std::string utility::generateId() {
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "������ ������������� COM: " << std::hex << hr << std::endl;

        return "";
    }

    GUID guid;
    HRESULT result = CoCreateGuid(&guid);
    if (FAILED(result)) {
        std::cerr << "������ ��� ��������� GUID: " << std::hex << result << std::endl;
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

std::string utility::hashPassword(std::string password) {
    std::hash<std::string> hashFunction;
    return std::to_string(hashFunction(password));
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


std::string utility::getTimeStamp() {
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