#include "photo.h"

Photo::Photo(const std::string& photoPath)
    : m_photoPath(photoPath), m_size(-1) {
    if (photoPath != "") {
        updateSize();
        saveToFile();
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

    std::ifstream file(m_photoPath, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + m_photoPath);
    }

    std::vector<char> buffer(m_size);
    file.read(buffer.data(), m_size);
    file.close();

    // Кодируем бинарные данные в Base64
    return base64_encode(std::string(buffer.data(), m_size), false);
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


void Photo::saveToFile() const {
    WCHAR username[256];
    DWORD username_len = sizeof(username) / sizeof(WCHAR);
    if (GetUserNameW(username, &username_len)) {
        std::string usernameStr = wideStringToString(username);
        std::string saveDirectory = "C:\\Users\\" + usernameStr + "\\Documents\\Data_Air_Gram\\Photos";
        std::filesystem::path filePath(m_photoPath);
        std::string fileName = filePath.filename().string();
        std::filesystem::path fullPath = std::filesystem::path(saveDirectory) / fileName;
        std::filesystem::path dir = fullPath.parent_path();

        if (!dir.empty() && !std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        std::ifstream src(m_photoPath, std::ios::binary);
        std::ofstream dst(fullPath.string(), std::ios::binary);
        if (src && dst) {
            dst << src.rdbuf();
        }
        else {
            std::cerr << "Open file Error" << std::endl;
        }
    }
    else {
        std::cerr << "No User data" << std::endl;
    }
}

