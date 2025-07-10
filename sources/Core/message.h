#pragma once

#include <string>
#include <vector>

#include "fileWrapper.h"



class Message {
public:
	Message() = default;

	Message(const std::string& message, const std::string& timestamp, const std::string& id, bool isFromMe, bool isRead = false)
	: m_message(message), m_timestamp(timestamp), m_id(id), m_is_from_me(isFromMe), m_is_read(isRead){}

	const std::string& getMessage() const { return m_message; }
	void setMessage(const std::string& message) { m_message = message; }

	const std::string& getTimestamp() const { return m_timestamp; }
	void setTimestamp(const std::string& timestamp) { m_timestamp = timestamp; }

	void setId(const std::string& id) { m_id = id; }
	const std::string& getId() const { return m_id; }

	void setIsSend(bool isFromMe) { m_is_from_me = isFromMe; }
	bool getIsSend() const { return m_is_from_me; }

	void setIsRead(bool isRead) { m_is_read = isRead; }
	bool getIsRead() const { return m_is_read; }

	void setIsNeedToRetry(bool isNeedToRetry) { m_is_need_to_retry = isNeedToRetry; }
	bool getIsNeedToRetry() const { return m_is_need_to_retry; }

	void setIsSending(bool isSending) { m_is_sending = isSending; }
	bool getIsSending() const { return m_is_sending; }

	size_t getRelatedFilesCount() { return m_vec_related_files.size(); }
	void addRelatedFile(const fileWrapper& fileWrapper) { m_vec_related_files.emplace_back(fileWrapper); }
	std::vector<fileWrapper>& getRelatedFiles() { return m_vec_related_files; }

	std::string serialize() const;
	static Message* deserialize(const std::string& data);

	void setRelatedFilePaths(std::vector<fileWrapper> relatedFilesVec);

private:
	std::vector<fileWrapper> m_vec_related_files;

	std::string m_message;
	std::string m_timestamp;
	std::string m_id;
	bool		m_is_from_me;
	bool		m_is_read;
	bool		m_is_need_to_retry = false;
	bool		m_is_sending = false;
};
