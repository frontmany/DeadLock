#include "database.h"
#include "utility.h"
#include <iostream>
#include <sstream>

#include "secblock.h"
#include"rsa.h" 

void Database::init(const std::string& loginHash) {
    int rc = sqlite3_open("your_database.db", &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        if (m_db) {
            sqlite3_close(m_db);
            m_db = nullptr;
        }
        return;
    }

    std::string sqlMessages = "CREATE TABLE IF NOT EXISTS MESSAGES_" + loginHash + " ("
        "LOGIN TEXT NOT NULL, "
        "MSGS TEXT NOT NULL, "
        "AES_KEY BLOB NOT NULL);";

    std::string sqlBlobs = "CREATE TABLE IF NOT EXISTS BLOBS_" + loginHash + " ("
        "BLOB_UID             TEXT    NOT NULL PRIMARY KEY,"
        "FILES_COUNT_IN_BLOB  INTEGER NOT NULL CHECK(FILES_COUNT_IN_BLOB > 0),"
        "FILES_RECEIVED       INTEGER NOT NULL DEFAULT 0,"
        "SERIALIZED_MESSAGE   TEXT    NOT NULL,"
        "AES_KEY              BLOB    NOT NULL,"
        "CHECK (FILES_RECEIVED <= FILES_COUNT_IN_BLOB));";

    std::string sqlRequestedFiles = "CREATE TABLE IF NOT EXISTS REQUESTED_FILES_" + loginHash + " ("
        "FILE_ID TEXT NOT NULL PRIMARY KEY);";

    char* errMsg = nullptr;

    rc = sqlite3_exec(m_db, sqlBlobs.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error in BLOBS table creation: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(m_db);
        m_db = nullptr;
        return;
    }
    else {
        std::cout << "Table BLOBS_" << loginHash << " created successfully" << std::endl;
    }

    rc = sqlite3_exec(m_db, sqlMessages.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error in MESSAGES table creation: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(m_db);
        m_db = nullptr;
        return;
    }
    else {
        std::cout << "Table MESSAGES_" << loginHash << " created successfully" << std::endl;
    }

    rc = sqlite3_exec(m_db, sqlRequestedFiles.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error in REQUESTED_FILES table creation: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        sqlite3_close(m_db);
        m_db = nullptr;
        return;
    }
    else {
        std::cout << "Table REQUESTED_FILES_" << loginHash << " created successfully" << std::endl;
    }
}



bool Database::addRequestedFile(const std::string& loginHash, const std::string& fileId) {
    if (!m_db) return false;

    std::string tableName = "REQUESTED_FILES_" + loginHash;
    std::string sql = "INSERT INTO " + tableName + " (FILE_ID) VALUES (?);";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, fileId.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to bind param: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute insert: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
}


bool Database::removeRequestedFile(const std::string& loginHash, const std::string& fileId) {
    if (!m_db) return false;

    std::string tableName = "REQUESTED_FILES_" + loginHash;
    std::string sql = "DELETE FROM " + tableName + " WHERE FILE_ID = ?;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare delete statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, fileId.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to bind param: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute delete: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::checkRequestedFile(const std::string& loginHash, const std::string& fileId) {
    if (!m_db) return false;

    std::string tableName = "REQUESTED_FILES_" + loginHash;
    std::string sql = "SELECT 1 FROM " + tableName + " WHERE FILE_ID = ? LIMIT 1;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    rc = sqlite3_bind_text(stmt, 1, fileId.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to bind param: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    rc = sqlite3_step(stmt);
    bool exists = (rc == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return exists;
}

std::vector<std::string> Database::getRequestedFiles(const std::string& loginHash) {
    std::vector<std::string> files;
    if (!m_db) return files;

    std::string tableName = "REQUESTED_FILES_" + loginHash;
    std::string sql = "SELECT FILE_ID FROM " + tableName + ";";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare select statement: " << sqlite3_errmsg(m_db) << std::endl;
        return files;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const unsigned char* text = sqlite3_column_text(stmt, 0);
        if (text) {
            files.emplace_back(reinterpret_cast<const char*>(text));
        }
    }

    if (rc != SQLITE_DONE) {
        std::cerr << "Error while iterating over rows: " << sqlite3_errmsg(m_db) << std::endl;
    }

    sqlite3_finalize(stmt);
    return files;
}





bool Database::addBlob(const CryptoPP::RSA::PublicKey& publicKey,
    const std::string& loginHash,
    const std::string& blobUid,
    int filesCountInBlob,
    int filesReceived,
    const std::string& serializedMessage)
{
    CryptoPP::SecByteBlock aesKey;
    utility::generateAESKey(aesKey);
    std::string encryptedAesKey = utility::RSAEncryptKey(publicKey, aesKey);


    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "INSERT INTO " + tableName +
        " (BLOB_UID, FILES_COUNT_IN_BLOB, FILES_RECEIVED, SERIALIZED_MESSAGE, AES_KEY) "
        "VALUES (?, ?, ?, ?, ?)";

    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, blobUid.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, filesCountInBlob);
    sqlite3_bind_int(stmt, 3, filesReceived);
    std::string encryptedMessage = utility::AESEncrypt(aesKey, serializedMessage);
    sqlite3_bind_text(stmt, 4, encryptedMessage.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 5, encryptedAesKey.data(), encryptedAesKey.size(), SQLITE_STATIC);

    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_CONSTRAINT) {
        std::cerr << "Blob with UID '" << blobUid << "' already exists in table " << tableName << std::endl;
        return false;
    }
    else if (rc != SQLITE_DONE) {
        std::cerr << "Failed to add blob: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    return true;
}

bool Database::removeBlob(const std::string& loginHash, const std::string& blobUid) {
    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "DELETE FROM " + tableName + " WHERE BLOB_UID = ?";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, blobUid.c_str(), -1, SQLITE_STATIC);
    bool result = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return result;
}

bool Database::isBlobExists(const std::string& loginHash, const std::string& blobUid) {
    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "SELECT 1 FROM " + tableName + " WHERE BLOB_UID = ? LIMIT 1";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, blobUid.c_str(), -1, SQLITE_STATIC);
    bool exists = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return exists;
}

std::string Database::getSerializedMessage(const CryptoPP::RSA::PrivateKey& privateKey,
    const std::string& loginHash,
    const std::string& blobUid)
{
    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "SELECT AES_KEY, SERIALIZED_MESSAGE FROM " + tableName + " WHERE BLOB_UID = ?";

    sqlite3_stmt* stmt = nullptr;
    std::string encryptedMessage;
    std::string encryptedAesKey;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return "";
    }

    sqlite3_bind_text(stmt, 1, blobUid.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const void* aesKeyBlob = sqlite3_column_blob(stmt, 0);
        int aesKeySize = sqlite3_column_bytes(stmt, 0);
        if (aesKeyBlob && aesKeySize > 0) {
            encryptedAesKey = std::string(reinterpret_cast<const char*>(aesKeyBlob), aesKeySize);
        }


        const unsigned char* msgText = sqlite3_column_text(stmt, 1);
        if (msgText) {
            encryptedMessage = reinterpret_cast<const char*>(msgText);
        }
    }
    sqlite3_finalize(stmt);

    if (encryptedAesKey.empty() || encryptedMessage.empty()) {
        std::cerr << "Blob data not found or incomplete for UID: " << blobUid << std::endl;
        return "";
    }

    CryptoPP::SecByteBlock aesKey;
    try {
        aesKey = utility::RSADecryptKey(privateKey, encryptedAesKey);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to decrypt AES key: " << e.what() << std::endl;
        return "";
    }

    std::string decryptedMessage;
    try {
        decryptedMessage = utility::AESDecrypt(aesKey, encryptedMessage);
    }
    catch (const std::exception& e) {
        //here
        std::cerr << "Failed to decrypt message: " << e.what() << std::endl;
        return "";
    }

    return decryptedMessage;
}

bool Database::updateSerializedMessage(const CryptoPP::RSA::PrivateKey& privateKey,
    const std::string& loginHash,
    const std::string& blobUid,
    const std::string& message)
{
    std::string encryptedAesKey = getEncryptedAesKey(loginHash, blobUid);
    if (encryptedAesKey.empty()) {
        std::cerr << "Failed to get AES key for blob: " << blobUid << std::endl;
        return false;
    }

    CryptoPP::SecByteBlock aesKey;
    try {
        aesKey = utility::RSADecryptKey(privateKey, encryptedAesKey);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to decrypt AES key: " << e.what() << std::endl;
        return false;
    }

    std::string encryptedMessage;
    try {
        encryptedMessage = utility::AESEncrypt(aesKey, message);
    }
    catch (const std::exception& e) {
        std::cerr << "Failed to encrypt message: " << e.what() << std::endl;
        return false;
    }

    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "UPDATE " + tableName + " SET SERIALIZED_MESSAGE = ? WHERE BLOB_UID = ?";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, encryptedMessage.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, blobUid.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);

    CryptoPP::SecureWipeBuffer(aesKey.data(), aesKey.size());

    return success;
}

std::string Database::getEncryptedAesKey(const std::string& loginHash,
    const std::string& blobUid)
{
    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "SELECT AES_KEY FROM " + tableName + " WHERE BLOB_UID = ?";

    sqlite3_stmt* stmt = nullptr;
    std::string encryptedAesKey;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return "";
    }

    sqlite3_bind_text(stmt, 1, blobUid.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const void* blob = sqlite3_column_blob(stmt, 0);
        int size = sqlite3_column_bytes(stmt, 0);
        if (blob && size > 0) {
            encryptedAesKey.assign(static_cast<const char*>(blob), size);
        }
    }
    sqlite3_finalize(stmt);
    return encryptedAesKey;
}

bool Database::incrementFilesReceivedCounter(const std::string& loginHash, const std::string& blobUid) {
    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "UPDATE " + tableName + " SET FILES_RECEIVED = FILES_RECEIVED + 1 WHERE BLOB_UID = ?";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, blobUid.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

int Database::getReceivedFilesCount(const std::string& loginHash,
    const std::string& blobUid)
{
    std::string tableName = "BLOBS_" + loginHash;
    std::string sql = "SELECT FILES_RECEIVED FROM " + tableName + " WHERE BLOB_UID = ?";

    sqlite3_stmt* stmt = nullptr;
    int count = -1; 

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return -1;
    }

    sqlite3_bind_text(stmt, 1, blobUid.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0); 
    }
    else {
        std::cerr << "Blob " << blobUid << " not found in table " << tableName << std::endl;
    }

    sqlite3_finalize(stmt);
    return count;
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
    std::string encryptedAesKey = utility::RSAEncryptKey(publicKey, aesKey);

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
                aesKey = utility::RSADecryptKey(privateKey, encryptedKeyStr);
               

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
