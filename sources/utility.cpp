#include"utility.h"




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

QString Utility::parseDate(QString fullDate) {
    if (fullDate == "online") {
        return fullDate;
    }
    if (fullDate == "recently") {
        return fullDate;
    }

    // Проверяем, содержит ли строка "last seen:"
    if (!fullDate.startsWith("last seen:")) {
        return "Invalid date format";
    }

    // Удаляем "last seen:" из строки
    fullDate = fullDate.mid(10).trimmed();

    // Получаем текущую дату и время
    QDateTime now = QDateTime::currentDateTime();
    int currentYear = now.date().year();

    // Разделяем входную строку на части
    QStringList dateParts = fullDate.split(" ");
    if (dateParts.size() < 2) {
        return "Invalid date format";
    }

    QString datePart = dateParts[0]; // Год, месяц, день
    QString timePart = dateParts[1]; // Время

    // Разделяем дату на год, месяц и день
    QStringList ymd = datePart.split("-");
    if (ymd.size() < 3) {
        return "Invalid date format";
    }

    int year = ymd[0].toInt();
    int month = ymd[1].toInt();
    int day = ymd[2].toInt();

    // Проверяем, совпадает ли год с текущим
    if (year == currentYear) {
        // Если год совпадает, возвращаем только время с секундами
        return "last seen: " + timePart;
    }
    else {
        // Если год не совпадает, возвращаем только год, месяц и день
        return "last seen: " + QString("%1-%2-%3").arg(year).arg(month, 2, 10, QChar('0')).arg(day, 2, 10, QChar('0'));
    }
}