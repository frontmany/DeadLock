#include"message.h"

#include <sstream>
#include <string>
#include <iostream>

std::string Message::serialize() const {
    std::ostringstream oss;

    auto escape = [](const std::string& s) {
        std::string result;
        for (char c : s) {
            if (c == '|') {
                result += "\\|";
            }
            else if (c == '\\') {
                result += "\\\\";
            }
            else {
                result += c;
            }
        }
        return result;
        };


    oss << escape(m_message) << "|"
        << escape(m_timestamp) << "|"
        << escape(m_id) << "|"
        << (m_is_from_me ? "true" : "false") << "|"
        << (m_is_read ? "true" : "false") << "|";

    oss << std::to_string(m_vec_related_files.size()) << "|";


    for (const auto& file_entry : m_vec_related_files) {
        const auto& file = file_entry.file;  

        oss << (file_entry.isPresent ? "true" : "false") << "|"
            << escape(file.blobUID) << "|"
            << escape(file.senderLogin) << "|"
            << escape(file.receiverLogin) << "|"
            << escape(file.filePath) << "|"
            << escape(file.id) << "|"
            << escape(file.timestamp) << "|"
            << file.fileSize << "|"
            << escape(file.caption) << "|";
    }

    return oss.str();
}


void Message::setRelatedFilePaths(std::vector<fileWrapper> relatedFilesVec) {
    m_vec_related_files = std::move(relatedFilesVec);
}


Message* Message::deserialize(const std::string& data) {
    auto unescape = [](const std::string& s) {
        std::string result;
        for (size_t i = 0; i < s.size(); ++i) {
            if (s[i] == '\\' && i + 1 < s.size()) {
                if (s[i + 1] == '|' || s[i + 1] == '\\') {
                    result += s[i + 1];
                    ++i;
                    continue;
                }
            }
            result += s[i];
        }
        return result;
        };

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


    const size_t MIN_TOKENS = 6; 
    if (tokens.size() < MIN_TOKENS) {
        return nullptr;
    }

    Message* msg = new Message();
    size_t index = 0;

    try {
        msg->m_message = unescape(tokens[index++]);
        msg->m_timestamp = unescape(tokens[index++]);
        msg->m_id = unescape(tokens[index++]);
        msg->m_is_from_me = (tokens[index++] == "true");
        msg->m_is_read = (tokens[index++] == "true");

        size_t file_count = std::stoul(tokens[index++]);

        const size_t FIELDS_PER_FILE = 9;
        for (size_t i = 0; i < file_count; ++i) {
            if (index + FIELDS_PER_FILE - 1 >= tokens.size()) {
                throw std::runtime_error("Not enough tokens for file data");
            }
            /*
            fileWrapper file_entry;
            file_entry.isPresent = (tokens[index++] == "true");

            file_entry.file.blobUID = unescape(tokens[index++]);
            file_entry.file.senderLogin = unescape(tokens[index++]);
            file_entry.file.receiverLogin = unescape(tokens[index++]);
            file_entry.file.filePath = unescape(tokens[index++]);
            file_entry.file.id = unescape(tokens[index++]);
            file_entry.file.timestamp = unescape(tokens[index++]);
            file_entry.file.fileSize = std::stoul(tokens[index++]);
            file_entry.file.caption = unescape(tokens[index++]);

            msg->m_vec_related_files.push_back(file_entry);
            */
        }
    }
    catch (...) {
        delete msg;
        return nullptr;
    }

    return msg;
}