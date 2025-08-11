#include "file.h"

#include "utility.h"

#include <sstream>
#include <vector>

File::File(uint32_t fileSize,
           const std::string& filePath,
           const std::string& fileName,
           const std::string& id,
           bool isNeedToRetry,
           bool isSending,
           bool isPresent)
    : m_fileSize(fileSize),
      m_filePath(filePath),
      m_fileName(fileName),
      m_id(id),
      isNeedToRetry(isNeedToRetry),
      isSending(isSending),
      isPresent(isPresent) 
{
}

uint32_t File::getFileSize() const { return m_fileSize; }
void File::setFileSize(uint32_t fileSize) { m_fileSize = fileSize; }

const std::string& File::getFilePath() const { return m_filePath; }
void File::setFilePath(const std::string& filePath) { m_filePath = filePath; }

const std::string& File::getFileName() const { return m_fileName; }
void File::setFileName(const std::string& fileName) { m_fileName = fileName; }

const std::string& File::getId() const { return m_id; }
void File::setId(const std::string& id) { m_id = id; }

bool File::getIsNeedToRetry() const { return isNeedToRetry; }
void File::setIsNeedToRetry(bool value) { isNeedToRetry = value; }

bool File::getIsSending() const { return isSending; }
void File::setIsSending(bool value) { isSending = value; }

bool File::getIsPresent() const { return isPresent; }
void File::setIsPresent(bool value) { isPresent = value; }

std::string File::serialize() const {
    std::ostringstream oss;

    oss << m_fileSize << "|"
        << utility::escape(m_filePath) << "|"
        << utility::escape(m_fileName) << "|"
        << utility::escape(m_id) << "|"
        << (isPresent ? "true" : "false") << "|"
        << ((isSending || isNeedToRetry) ? "true" : "false") << "|";

    return oss.str();
}

std::shared_ptr<File> File::deserialize(const std::string& data) {
    std::vector<std::string> tokens;
    bool escape = false;
    std::string current;
    for (char c : data) {
        if (escape) {
            current += c;
            escape = false;
        }
        else if (c == '\\') {
            escape = true;
        }
        else if (c == '|') {
            tokens.push_back(current);
            current.clear();
        }
        else {
            current += c;
        }
    }
    if (!current.empty()) {
        tokens.push_back(current);
    }

    const size_t REQUIRED_TOKENS = 6;
    if (tokens.size() < REQUIRED_TOKENS) {
        return nullptr;
    }

    try {
        uint32_t fileSize = static_cast<uint32_t>(std::stoul(tokens[0]));
        std::string filePath = utility::unescape(tokens[1]);
        std::string fileName = utility::unescape(tokens[2]);
        std::string id = utility::unescape(tokens[3]);
        bool isPresent = (tokens[4] == "true");
        bool isNeedToRetry = (tokens[5] == "true");

        auto file = std::make_shared<File>(
            fileSize, filePath, fileName, id,
            isNeedToRetry,
            false,
            isPresent
        );
        return file;
    }
    catch (...) {
        return nullptr;
    }
}
