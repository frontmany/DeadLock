#include "photo.h"
#include <locale>
#include <codecvt>

Photo::Photo(const std::string& photoPath)
    : m_photoPath(photoPath), m_size(0) {
    if (photoPath != "") {
        updateSize();
    }
}

std::string Photo::wideStringToString(const WCHAR* wideStr) {
    if (!wideStr) {
        return ""; // Возвращаем пустую строку, если входной указатель нулевой
    }

    // Получаем размер буфера, необходимого для преобразования
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
    if (size_needed <= 0) {
        return ""; // Возвращаем пустую строку в случае ошибки
    }

    // Создаем строку с нужным размером
    std::string str(size_needed - 1, 0); // Исключаем завершающий нулевой символ

    // Выполняем преобразование
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &str[0], size_needed, nullptr, nullptr);

    return str;
}


void Photo::updateSize() {
    std::ifstream file(m_photoPath, std::ios::binary);
    if (file) {
        file.seekg(0, std::ios::end);
        m_size = static_cast<std::size_t>(file.tellg());
        file.close();
    }
    else {
        m_size = 0;
    }
}


std::string Photo::serialize() const {
    if (m_photoPath.empty()) {
        return "";
    }


#ifdef _WIN32
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    std::wstring wide_path = converter.from_bytes(m_photoPath);
    std::ifstream file(wide_path, std::ios::binary);
#else
    std::ifstream file(m_photoPath, std::ios::binary);
#endif

    if (!file) {
        throw std::runtime_error("Failed to open file: " + m_photoPath);
    }

    std::vector<char> buffer(m_size);
    file.read(buffer.data(), m_size);
    file.close();

    return base64_encode(std::string(buffer.data(), m_size), false);
}

void Photo::updateNameOnPC(const std::string& oldLogin, const std::string& newLogin) const {
    WCHAR username[256];
    DWORD username_len = sizeof(username) / sizeof(WCHAR);
    if (!GetUserNameW(username, &username_len)) {
        std::cout << "No User data" << std::endl;
        return; 
    }

    std::string usernameStr = wideStringToString(username);
    std::string saveDirectory = "C:/Users/" + usernameStr + "/Documents/Data_Air_Gram";
    std::string oldPath = saveDirectory + "/" + oldLogin + "myMainPhoto.png";
    std::string newPath = saveDirectory + "/" + newLogin + "myMainPhoto.png";

    if (rename(oldPath.c_str(), newPath.c_str()) != 0) {
        std::cout << "Failed to rename photo file from " << oldPath << " to " << newPath << std::endl;
    }
}

Photo* Photo::deserialize(const std::string& data, size_t size, std::string login) {
    if (data == "") {
        return new Photo;
    }

    std::istringstream iss(data);

    WCHAR username[256];
    DWORD username_len = sizeof(username) / sizeof(WCHAR);
    if (!GetUserNameW(username, &username_len)) {
        std::cout << "No User data" << std::endl;
    }

    std::string usernameStr = wideStringToString(username);
    std::string saveDirectory = "C:/Users/" + usernameStr + "/Documents/Data_Air_Gram";
    std::string tempPath = saveDirectory + "/" + login + "Photo.png";

    std::vector<char> buffer(size);

    iss.read(buffer.data(), size);
    std::filesystem::create_directories(saveDirectory);
    std::ofstream outFile(tempPath, std::ios::binary);
    if (outFile) {
        outFile.write(buffer.data(), size);
        outFile.close();
        std::cout << tempPath << std::endl;
    }
    else {
        std::cerr << "Open file Error" << std::endl;
    }


    return new Photo(tempPath);
}

Photo* Photo::updateOnPC(const std::string& data, size_t size, std::string oldLogin, std::string newLogin) {
    if (data == "") {
        return new Photo;
    }
    std::istringstream iss(data);

    WCHAR username[256];
    DWORD username_len = sizeof(username) / sizeof(WCHAR);
    if (!GetUserNameW(username, &username_len)) {
        std::cout << "No User data" << std::endl;
    }

    std::string usernameStr = wideStringToString(username);
    std::string saveDirectory = "C:/Users/" + usernameStr + "/Documents/Data_Air_Gram";
    std::string oldPath = saveDirectory + "/" + oldLogin + "Photo.png";
    std::string newPath = saveDirectory + "/" + newLogin + "Photo.png";


    if (std::filesystem::exists(oldPath)) {
        try {
            std::filesystem::remove(oldPath);
            std::cout << "Old login photo deleted: " << oldPath << std::endl;
        }
        catch (const std::filesystem::filesystem_error& e) {
            std::cerr << "Failed to delete old login photo: " << e.what() << std::endl;
        }
    }

    std::vector<char> buffer(size);

    iss.read(buffer.data(), size);
    std::filesystem::create_directories(saveDirectory);
    std::ofstream outFile(newPath, std::ios::binary);
    if (outFile) {
        outFile.write(buffer.data(), size);
        outFile.close();
        std::cout << newPath << std::endl;
    }
    else {
        std::cerr << "Open file Error" << std::endl;
    }

    return new Photo(newPath);
}