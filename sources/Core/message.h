#pragma once

#include <string>
#include <vector>
#include <memory>


class Message {
public:
    Message() = default;
    Message(const std::string& message,
        const std::string& timestamp,
        const std::string& id,
        bool isFromMe,
        bool isRead);

    const std::string& getMessage() const;
    void setMessage(const std::string& message);

    const std::string& getTimestamp() const;
    void setTimestamp(const std::string& timestamp);

    const std::string& getId() const;
    void setId(const std::string& id);

    bool getIsFromMe() const;
    void setIsFromMe(bool isFromMe);

    bool getIsRead() const;
    void setIsRead(bool isRead);

    bool getIsNeedToRetry() const;
    void setIsNeedToRetry(bool isNeedToRetry);

    bool getIsSending() const;
    void setIsSending(bool isSending);

    std::string serialize() const;
    static std::shared_ptr<Message> deserialize(const std::string& data);

private:
	std::string m_message;
	std::string m_timestamp;
	std::string m_id;

	bool m_isFromMe;
	bool m_isRead;
	bool m_isNeedToRetry;
	bool m_isSending;
};
