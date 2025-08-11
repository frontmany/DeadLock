#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "file.h"

class Blob {
public:
    Blob() = default;
    Blob(const std::unordered_map<std::string, File>& relatedFiles,
         const std::string& timestamp,
         const std::string& caption,
         const std::string& blobUId,
         int filesCount);
    ~Blob() = default;

    const std::unordered_map<std::string, File>& getRelatedFilesMap() const;
    void setRelatedFilesMap(const std::unordered_map<std::string, File>& relatedFiles);

    const std::string& getTimestamp() const;
    void setTimestamp(const std::string& value);

    const std::string& getCaption() const;
    void setCaption(const std::string& value);

    const std::string& getBlobId() const;
    void setBlobId(const std::string& value);

    uint32_t getFilesCount() const;
    void setFilesCount(uint32_t value);

    bool getIsFromMe() const;
    void setIsFromMe(bool isFromMe);

    bool getIsRead() const;
    void setIsRead(bool isRead);

    bool getIsNeedToRetry() const;
    void setIsNeedToRetry(bool isNeedToRetry);

    bool getIsSending() const;
    void setIsSending(bool isSending);

    std::string serialize() const;
    static std::shared_ptr<Blob> deserialize(const std::string& data);

private:
    std::unordered_map<std::string, File> m_mapRelatedFiles;
    uint32_t m_filesCount{};

    std::string m_caption;
    std::string m_timestamp;
    std::string m_id;

    bool m_isFromMe;
    bool m_isRead;
    bool m_isNeedToRetry;
    bool m_isSending;
};