#include "database.h"
#include "photo.h"
#include "utility.h"
#include <iostream>
#include <sstream>

#include "secblock.h"
#include"rsa.h" 

void Database::init(const std::string& loginHash) {
    int rc = sqlite3_open("your_database.db", &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS MESSAGES_" + loginHash + " ("
        "LOGIN TEXT NOT NULL, "
        "MSGS TEXT NOT NULL, "
        "AES_KEY BLOB NOT NULL);";

    char* zErrMsg = nullptr;
    rc = sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        sqlite3_close(m_db);
        m_db = nullptr;
        return;
    }

    std::cout << "Table MESSAGES_" << loginHash << " created successfully" << std::endl;
}

void Database::updateTableName(const std::string& myOldLoginHash, const std::string& myNewLoginHash) {
    if (!m_db) {
        std::cerr << "Database not initialized" << std::endl;
        return;
    }

    char* zErrMsg = nullptr;
    int rc = sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error starting transaction: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return;
    }

    std::string checkSql = "SELECT name FROM sqlite_master WHERE type='table' AND name='MESSAGES_" + myOldLoginHash + "';";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(m_db, checkSql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error preparing check statement: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Table MESSAGES_" << myOldLoginHash << " doesn't exist" << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }
    sqlite3_finalize(stmt);

    std::string renameSql = "ALTER TABLE MESSAGES_" + myOldLoginHash + " RENAME TO MESSAGES_" + myNewLoginHash + ";";
    rc = sqlite3_exec(m_db, renameSql.c_str(), nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error renaming table: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    rc = sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error committing transaction: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return;
    }

    std::cout << "Table renamed from MESSAGES_" << myOldLoginHash
        << " to MESSAGES_" << myNewLoginHash << " successfully" << std::endl;
}

void Database::updateFriendLoginHash(const std::string& myLoginHash,
    const std::string& oldFriendLoginHash,
    const std::string& newFriendLoginHash) {
    if (!m_db) {
        std::cerr << "Database not initialized" << std::endl;
        return;
    }

    char* zErrMsg = nullptr;
    int rc = sqlite3_exec(m_db, "BEGIN TRANSACTION;", nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error starting transaction: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return;
    }

    std::string checkSql = "SELECT name FROM sqlite_master WHERE type='table' AND name='MESSAGES_" + myLoginHash + "';";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(m_db, checkSql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error preparing check statement: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        std::cerr << "Table MESSAGES_" << myLoginHash << " doesn't exist" << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }
    sqlite3_finalize(stmt);

    std::string updateSql = "UPDATE MESSAGES_" + myLoginHash +
        " SET LOGIN = ? " 
        "WHERE LOGIN = ?";

    rc = sqlite3_prepare_v2(m_db, updateSql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error preparing update statement: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    sqlite3_bind_text(stmt, 1, newFriendLoginHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, oldFriendLoginHash.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "SQL error updating friend login hash: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_exec(m_db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return;
    }

    sqlite3_finalize(stmt);

    rc = sqlite3_exec(m_db, "COMMIT;", nullptr, nullptr, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error committing transaction: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg);
        return;
    }

    std::cout << "Updated friend login hash from " << oldFriendLoginHash
        << " to " << newFriendLoginHash << " in table MESSAGES_"
        << myLoginHash << std::endl;
}

void Database::saveMessages(const CryptoPP::RSA::PublicKey& publicKey,
    const std::string& myLogin,
    const std::string& friendLogin,
    std::vector<Message*> messages) const
{
    if (!m_db || messages.empty() || myLogin.empty()) {
        std::cerr << "Invalid database state or input parameters" << std::endl;
        return;
    }

    std::ostringstream oss;
    for (size_t i = 0; i < messages.size(); ++i) {
        oss << messages[i]->serialize();
        if (i < messages.size() - 1) {
            oss << c_delimiter;
        }
    }

    CryptoPP::SecByteBlock aesKey;
    utility::generateAESKey(aesKey);
    std::string encryptedAesKey = utility::RSAEncrypt(publicKey, aesKey);

    std::string encryptedMessages = utility::AESEncrypt(aesKey, oss.str());
    std::string friendLoginHash = utility::calculateHash(friendLogin);
    std::string myLoginHash = utility::calculateHash(myLogin);

    bool recordExists = false;
    std::string selectSql = "SELECT 1 FROM MESSAGES_" + myLoginHash +
        " WHERE LOGIN = ? LIMIT 1;";
    sqlite3_stmt* selectStmt = nullptr;

    if (sqlite3_prepare_v2(m_db, selectSql.c_str(), -1, &selectStmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(selectStmt, 1, friendLoginHash.c_str(), -1, SQLITE_TRANSIENT);
        recordExists = (sqlite3_step(selectStmt) == SQLITE_ROW);
        sqlite3_finalize(selectStmt);
    }

    std::string sql;
    if (recordExists) {
        sql = "UPDATE MESSAGES_" + myLoginHash +
            " SET MSGS = ?, AES_KEY = ? WHERE LOGIN = ?;";
    }
    else {
        sql = "INSERT INTO MESSAGES_" + myLoginHash +
            " (LOGIN, MSGS, AES_KEY) VALUES (?, ?, ?);";
    }

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    if (recordExists) {
        sqlite3_bind_text(stmt, 1, encryptedMessages.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_blob(stmt, 2, encryptedAesKey.data(), encryptedAesKey.size(), SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 3, friendLoginHash.c_str(), -1, SQLITE_TRANSIENT);
    }
    else {
        sqlite3_bind_text(stmt, 1, friendLoginHash.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_text(stmt, 2, encryptedMessages.c_str(), -1, SQLITE_TRANSIENT);
        sqlite3_bind_blob(stmt, 3, encryptedAesKey.data(), encryptedAesKey.size(), SQLITE_TRANSIENT);
    }

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(m_db) << std::endl;
    }

    sqlite3_finalize(stmt);
}

std::vector<Message*> Database::loadMessages(const CryptoPP::RSA::PrivateKey& privateKey,
                                          const std::string& myLogin,
                                          const std::string& friendLogin,
                                          std::vector<Message*>& messages) const 
{
    if (!m_db || myLogin.empty()) {
        std::cerr << "Invalid database state or login" << std::endl;
        return messages;
    }

    std::string myLoginHash = utility::calculateHash(myLogin);
    std::string friendLoginHash = utility::calculateHash(friendLogin);

    std::string sql = "SELECT MSGS, AES_KEY FROM MESSAGES_" + myLoginHash + 
                     " WHERE LOGIN = ?;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return messages;
    }

    sqlite3_bind_text(stmt, 1, friendLoginHash.c_str(), -1, SQLITE_TRANSIENT);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char* encryptedMsg = sqlite3_column_text(stmt, 0);
        const void* encryptedKeyBlob = sqlite3_column_blob(stmt, 1);
        int keySize = sqlite3_column_bytes(stmt, 1);

        if (encryptedMsg && encryptedKeyBlob && keySize > 0) {
            std::string encryptedKeyStr(
                reinterpret_cast<const char*>(encryptedKeyBlob),
                keySize);

           CryptoPP::SecByteBlock aesKey;
            try {
                aesKey = utility::RSADecrypt(privateKey, encryptedKeyStr);
               

                std::string decryptedText = utility::AESDecrypt(
                    aesKey, 
                    reinterpret_cast<const char*>(encryptedMsg));

                size_t pos = 0;
                while (pos < decryptedText.length()) {
                    size_t next_pos = decryptedText.find(c_delimiter, pos);
                    if (next_pos == std::string::npos) next_pos = decryptedText.length();
                    
                    std::string messagePart = decryptedText.substr(pos, next_pos - pos);
                    if (Message* message = Message::deserialize(messagePart)) {
                        messages.push_back(message);
                    }
                    
                    pos = next_pos + (next_pos < decryptedText.length() ? c_delimiter.length() : 0);
                }
            } catch (const std::exception& e) {
                std::cerr << "Decryption failed: " << e.what() << std::endl;
            }
        }
    } else {
        std::cerr << "No messages found for user: " << friendLogin << std::endl;
    }

    sqlite3_finalize(stmt);
    return messages;
}

void Database::deleteAllMessages(const std::string& myLogin,
    const std::string& friendLogin) const
{
    if (!m_db || myLogin.empty() || friendLogin.empty()) {
        std::cerr << "Invalid database state or login parameters" << std::endl;
        return;
    }

    std::string myLoginHash = utility::calculateHash(myLogin);
    std::string friendLoginHash = utility::calculateHash(friendLogin);

    std::string deleteSql = "DELETE FROM MESSAGES_" + myLoginHash +
        " WHERE LOGIN = ?;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, deleteSql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: "
            << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, friendLoginHash.c_str(),
        -1, SQLITE_TRANSIENT);

    int rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to delete messages: "
            << sqlite3_errmsg(m_db) << std::endl;
    }
    else {
        int changes = sqlite3_changes(m_db);
        std::cout << "Successfully deleted " << changes
            << " messages with " << friendLogin << std::endl;
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(m_db, "VACUUM;", nullptr, nullptr, nullptr);
}

Database::~Database() {
    if (m_db) {
        sqlite3_close(m_db);
    }
}
