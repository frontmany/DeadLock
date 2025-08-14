#include "blob.h"

#include "utility.h"

Blob::Blob(const std::unordered_map<std::string, File>& relatedFiles,
           const std::string& timestamp,
           const std::string& caption,
           const std::string& blobId,
           int filesCount)
    : m_mapRelatedFiles(relatedFiles),
      m_timestamp(timestamp),
      m_caption(caption),
      m_id(blobId),
      m_filesCount(filesCount) 
{
}

const std::unordered_map<std::string, FilePtr>& Blob::getRelatedFilesMap() const { return m_mapRelatedFiles; }
void Blob::setRelatedFilesMap(const std::unordered_map<std::string, FilePtr>& relatedFiles) { m_mapRelatedFiles = relatedFiles; }

const std::string& Blob::getTimestamp() const { return m_timestamp; }
void Blob::setTimestamp(const std::string& value) { m_timestamp = value; }

const std::string& Blob::getCaption() const { return m_caption; }
void Blob::setCaption(const std::string& value) { m_caption = value; }

const std::string& Blob::getBlobId() const { return m_id; }
void Blob::setBlobId(const std::string& value) { m_id = value; }

uint32_t Blob::getFilesCount() const { return m_filesCount; }
void Blob::setFilesCount(uint32_t value) { m_filesCount = value; }

bool Blob::getIsFromMe() const { return m_isFromMe; }
void Blob::setIsFromMe(bool isFromMe) { m_isFromMe = isFromMe; }

bool Blob::getIsRead() const { return m_isRead; }
void Blob::setIsRead(bool isRead) { m_isRead = isRead; }

bool Blob::getIsNeedToRetry() const { return m_isNeedToRetry; }
void Blob::setIsNeedToRetry(bool isNeedToRetry) { m_isNeedToRetry = isNeedToRetry; }

bool Blob::getIsSending() const { return m_isSending; }
void Blob::setIsSending(bool isSending) { m_isSending = isSending; }

std::string Blob::serialize() const {
    std::ostringstream oss;

    oss << utility::escape(m_timestamp) << "|"
        << utility::escape(m_caption) << "|"
        << utility::escape(m_id) << "|"
        << m_filesCount << "|"
        << (m_isFromMe ? "true" : "false") << "|"
        << (m_isRead ? "true" : "false") << "|"
        << ((m_isSending || m_isNeedToRetry) ? "true" : "false") << "|";

    oss << m_mapRelatedFiles.size() << "|";
    for (const auto& [_, file] : m_mapRelatedFiles) {
        oss << file.serialize() << "|";
    }

    return oss.str();
}

std::shared_ptr<Blob> Blob::deserialize(const std::string& data) {
    std::vector<std::string> tokens;
    std::string current;
    bool escaped = false;

    for (char c : data) {
        if (escaped) {
            current += c;
            escaped = false;
        }
        else if (c == '\\') {
            escaped = true;
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

    const size_t REQUIRED_BEFORE_FILES_TOKENS = 8;
    if (tokens.size() < REQUIRED_BEFORE_FILES_TOKENS) {
        return nullptr;
    }

    try {
        auto blob = std::make_shared<Blob>();
        size_t index = 0;

        blob->m_timestamp = utility::unescape(tokens[index++]);
        blob->m_caption = utility::unescape(tokens[index++]);
        blob->m_id = utility::unescape(tokens[index++]);
        blob->m_filesCount = static_cast<uint32_t>(std::stoul(tokens[index++]));
        blob->m_isFromMe = (tokens[index++] == "true");
        blob->m_isRead = (tokens[index++] == "true");
        blob->m_isNeedToRetry = (tokens[index++] == "true");
        blob->m_isSending = false;

        const size_t file_count = std::stoul(tokens[index++]);

        const size_t FILE_TOKENS = 6;
        if (tokens.size() < index + file_count * FILE_TOKENS) {
            return nullptr;
        }

        for (size_t i = 0; i < file_count; ++i) {
            std::string fileStr = tokens[index] + "|" + tokens[index + 1] + "|" + tokens[index + 2] + "|" +
                                  tokens[index + 3] + "|" + tokens[index + 4] + "|" + tokens[index + 5];
            index += FILE_TOKENS;

            auto filePtr = File::deserialize(fileStr);
            if (filePtr) {
                blob->m_mapRelatedFiles.emplace(filePtr->getId(), *filePtr);
            }
        }

        return blob;
    }
    catch (...) {
        return nullptr;
    }
}
