#include "database.h"
#include "photo.h"
#include "utility.h"
#include <iostream>
#include <sstream>

void Database::init(const std::string& login) {
    int rc = sqlite3_open("your_database.db", &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS MESSAGES_" + login + " ("
        "LOGIN TEXT NOT NULL, "
        "MSGS TEXT NOT NULL);";

    char* zErrMsg = nullptr;
    rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        sqlite3_close(m_db);
        m_db = nullptr;
        return;
    }

    std::cout << "Table MESSAGES_" << login << " created successfully" << std::endl;
}

void Database::saveMessages(const std::string& myLogin, const std::string& friendLogin, std::vector<Message*> messages) const {
    if (!m_db || messages.empty() || myLogin.empty()) {
        std::cerr << "Invalid database state or input parameters" << std::endl;
        return;
    }

    std::ostringstream oss;
    for (size_t i = 0; i < messages.size(); ++i) {
        oss << messages[i]->serialize();
        if (i < messages.size() - 1) {
            oss << delimiter;
        }
    }
    std::string text = oss.str();

    bool recordExists = false;
    std::string selectSql = "SELECT 1 FROM MESSAGES_" + myLogin + " WHERE LOGIN = ? LIMIT 1;";
    sqlite3_stmt* selectStmt = nullptr;

    if (sqlite3_prepare_v2(m_db, selectSql.c_str(), -1, &selectStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(selectStmt, 1, friendLogin.c_str(), -1, SQLITE_TRANSIENT);
        recordExists = (sqlite3_step(selectStmt) == SQLITE_ROW);
        sqlite3_finalize(selectStmt);
    }

    std::string sql;
    if (recordExists) {
        sql = "UPDATE MESSAGES_" + myLogin + " SET MSGS = ? WHERE LOGIN = ?;";
    }
    else {
        sql = "INSERT INTO MESSAGES_" + myLogin + " (LOGIN, MSGS) VALUES (?, ?);";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    if (recordExists) {
        sqlite3_bind_text(stmt, 1, text.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, friendLogin.c_str(), -1, SQLITE_TRANSIENT);
    }
    else {
        sqlite3_bind_text(stmt, 1, friendLogin.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, text.c_str(), -1, SQLITE_TRANSIENT);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(m_db) << std::endl;
    }

    sqlite3_finalize(stmt);
}

std::vector<Message*> Database::loadMessages(const std::string& myLogin, const std::string& friendLogin, std::vector<Message*>& messages) const {
    if (!m_db || myLogin.empty()) {
        std::cerr << "Invalid database state or login" << std::endl;
        return messages;
    }

    std::string sql = "SELECT MSGS FROM MESSAGES_" + myLogin + " WHERE LOGIN = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return messages;
    }

    sqlite3_bind_text(stmt, 1, friendLogin.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* messageText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (messageText) {
            std::string messagesStr(messageText);
            size_t pos = 0;
            size_t delimiter_len = delimiter.length();

            while (true) {
                size_t next_pos = messagesStr.find(delimiter, pos);
                std::string messagePart = messagesStr.substr(pos, next_pos - pos);

                Message* message = Message::deserialize(messagePart);
                if (message) {
                    messages.push_back(message);
                }

                if (next_pos == std::string::npos) break;
                pos = next_pos + delimiter_len;
            }
        }
    }
    else {
        std::cerr << "No messages found for user: " << friendLogin << std::endl;
    }

    sqlite3_finalize(stmt);
    return messages;
}

void Database::deleteAllMessages(const std::string& myLogin, const std::string& friendLogin) const {
    if (!m_db || myLogin.empty() || friendLogin.empty()) {
        std::cerr << "Invalid database state or login parameters" << std::endl;
        return;
    }

    std::string deleteSql = "DELETE FROM MESSAGES_" + myLogin + " WHERE LOGIN = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, deleteSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, friendLogin.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to delete messages: " << sqlite3_errmsg(m_db) << std::endl;
    }
    else {
        std::cout << "Successfully deleted all messages with " << friendLogin << std::endl;
    }

    sqlite3_finalize(stmt);
}

Database::~Database() {
    if (m_db) {
        sqlite3_close(m_db);
    }
}
