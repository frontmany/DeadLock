# pragma once

#include <cstdint>
#include <string>
#include <memory>


class File {
public:
    File() = default;
    File(uint32_t fileSize,
         const std::string& filePath,
         const std::string& fileName,
         const std::string& id,
         bool isNeedToRetry = false,
         bool isSending = false,
         bool isPresent = false);

    uint32_t getFileSize() const;
    void setFileSize(uint32_t fileSize);

    const std::string& getBlobId() const;
    void setBlobId(const std::string& id);

    const std::string& getFilePath() const;
    void setFilePath(const std::string& filePath);

    const std::string& getFileName() const;
    void setFileName(const std::string& fileName);

    const std::string& getId() const;
    void setId(const std::string& id);

    bool getIsNeedToRetry() const;
    void setIsNeedToRetry(bool value);

    bool getIsSending() const;
    void setIsSending(bool value);

    bool getIsPresent() const;
    void setIsPresent(bool value);

    std::string serialize() const;
    static std::shared_ptr<File> deserialize(const std::string& data);

private:
    uint32_t m_fileSize{};
    std::string m_blobId;
    std::string m_filePath;
    std::string m_fileName;
    std::string m_id;

    bool isNeedToRetry{false};
    bool isSending{false};
    bool isPresent{false};
};