#include "photo.h"
#include <locale>
#include <codecvt>

Photo::Photo(const std::string& photoPath)
    : m_photoPath(photoPath), m_size(0) {
    if (photoPath != "") {
        updateSize();
    }
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

    return base64_encode(std::string(buffer.data(), m_size), false);
}

Photo* Photo::deserialize(const std::string& data, std::string login) {
    if (data.empty()) {
        return new Photo;
    }

    std::string saveDirectory = "./Data_Air_Gram";
    std::string tempPath = saveDirectory + "/" + login + "Photo.png";

    // Создаем директорию, если её нет
    std::filesystem::create_directories(saveDirectory);

    // Записываем все данные в файл
    std::ofstream outFile(tempPath, std::ios::binary);
    if (outFile) {
        outFile.write(data.data(), data.size());  // Используем data.size()
        outFile.close();
        std::cout << "Photo saved to: " << tempPath << std::endl;
    }
    else {
        std::cerr << "Failed to open file: " << tempPath << std::endl;
        return new Photo;  // Возвращаем пустое фото в случае ошибки
    }

    return new Photo(tempPath);
}