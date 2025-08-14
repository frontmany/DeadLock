#include "database.h"

#include "utility.h"
#include "message.h"
#include "blob.h"
#include "file.h"

Database::Database() :
    m_db("your_database.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE)
{
}

void Database::init(const std::string& myUID) 
{
    m_myUID = myUID;

    try {
        SQLite::Transaction transaction(m_db);
        

        std::string requestedFilesTableName = constructTableName("REQUESTED_FILES_IDS", m_myUID);
        std::string sqlRequestedFiles = "CREATE TABLE IF NOT EXISTS " + requestedFilesTableName + " ("
            "FILE_ID TEXT NOT NULL PRIMARY KEY);";
        m_db.exec(sqlRequestedFiles);


        std::string blobsTableName = constructTableName("BLOB_BUFFERS", m_myUID);
        std::string sqlBlobs = "CREATE TABLE IF NOT EXISTS " + blobsTableName + " ("
        "BLOB_UID             TEXT    NOT NULL PRIMARY KEY,"
        "SENDER_UID           TEXT    NOT NULL,"
        "TIMESTAMP            TEXT    NOT NULL,"
        "FILES_COUNT_IN_BLOB  INTEGER NOT NULL CHECK(FILES_COUNT_IN_BLOB > 0),"
        "RECEIVED_FILES_COUNT INTEGER NOT NULL DEFAULT 0,"
        "CAPTION              TEXT    NOT NULL,"
        "CAPTION_KEY          TEXT    NOT NULL,"
        "CHECK (FILES_RECEIVED <= FILES_COUNT_IN_BLOB));";
        m_db.exec(sqlBlobs);


        std::string blobFilesTableTableName = constructTableName("BLOB_BUFFER_FILES", m_myUID);
        std::string sqlBlobFiles = "CREATE TABLE IF NOT EXISTS " + blobFilesTableTableName +" ("
            "BLOB_UID      TEXT    NOT NULL,"
            "FILE_NAME     TEXT    NOT NULL,"
            "FILE_SIZE     TEXT    NOT NULL,"
            "FILE_PATH     TEXT    NOT NULL,"
            "FILE_KEY      TEXT    NOT NULL,"
            "FILE_ID       TEXT    NOT NULL);";
        m_db.exec(sqlBlobFiles);


        std::string blobsTableName = constructTableName("BLOBS", m_myUID);
        std::string sqlBlobs = "CREATE TABLE IF NOT EXISTS " + blobsTableName + " ("
            "ID          TEXT    NOT NULL PRIMARY KEY,"
            "FRIEND_UID  TEXT    NOT NULL,"
            "TIMESTAMP   TEXT    NOT NULL,"
            "BLOB        TEXT    NOT NULL,"
            "BLOB_KEY    TEXT    NOT NULL);";
        m_db.exec(sqlBlobs);


        std::string messagesTableName = constructTableName("MESSAGES", m_myUID);
        std::string sqlMessages = "CREATE TABLE IF NOT EXISTS " + messagesTableName + " ("
            "ID          TEXT NOT NULL PRIMARY KEY,"
            "FRIEND_UID  TEXT    NOT NULL,"
            "TIMESTAMP   TEXT NOT NULL,"
            "MESSAGE     TEXT NOT NULL,"
            "MESSAGE_KEY TEXT NOT NULL);";
        m_db.exec(sqlMessages);


        std::string messageSendTasksTableName = constructTableName("SEND_MESSAGE_TASKS", m_myUID);
        std::string sqlSendMessageTasks = "CREATE TABLE IF NOT EXISTS " + messageSendTasksTableName + " ("
            "MESSAGE_ID        TEXT NOT NULL,"
            "FRIEND_PUBLIC_KEY TEXT NOT NULL,"
            "PRIMARY KEY (MESSAGE_ID, FRIEND_PUBLIC_KEY)" 
            ");";
        m_db.exec(sqlSendMessageTasks);

        std::string fileSendTasksTableName = constructTableName("SEND_FILE_TASKS", m_myUID);
        std::string sqlSendFileTasks = "CREATE TABLE IF NOT EXISTS " + fileSendTasksTableName + " ("
            "FILE_ID           TEXT NOT NULL,"
            "FRIEND_PUBLIC_KEY TEXT NOT NULL,"
            "PRIMARY KEY (FILE_ID, FRIEND_PUBLIC_KEY)"
            ");";
        m_db.exec(sqlSendFileTasks);

        transaction.commit();
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[init] SQL error: " << e.what() << std::endl;
        throw;

    }
}


// REQUESTED_FILES_IDS
bool Database::addRequestedFileId(const std::string& fileId) {
    try {
        std::string tableName = constructTableName("REQUESTED_FILES_IDS", m_myUID);
        SQLite::Statement query(m_db,
            "INSERT INTO " + tableName + " (FILE_ID) VALUES (?);");

        query.bind(1, fileId); 
        query.exec();         

        return true;          

    }
    catch (const SQLite::Exception& e) {
        std::cerr << "SQL error adding requested file: " << e.what()
            << " (code: " << e.getErrorCode() << ")" << std::endl;
        return false;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding requested file: " << e.what() << std::endl;
        return false;
    }
}

bool Database::removeRequestedFileId(const std::string& fileId) {
    try {
        std::string tableName = constructTableName("REQUESTED_FILES_IDS", m_myUID);

        SQLite::Statement query(m_db,
            "DELETE FROM " + tableName + " WHERE FILE_ID = ?");

        query.bind(1, fileId);
        return query.exec() > 0;

    }
    catch (const SQLite::Exception& e) {
        std::cerr << "Error removing file: " << e.what() << std::endl;
        return false;
    }
}

bool Database::isRequestedFileId(const std::string& fileId) {
    try {
        std::string tableName = constructTableName("REQUESTED_FILES_IDS", m_myUID);

        SQLite::Statement query(m_db,
            "SELECT 1 FROM " + tableName + " WHERE FILE_ID = ? LIMIT 1");

        query.bind(1, fileId);
        return query.executeStep();

    }
    catch (const SQLite::Exception& e) {
        std::cerr << "Error checking file: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::vector<std::string>> Database::getAllRequestedFilesIds() {
    std::vector<std::string> files;

    try {
        std::string tableName = constructTableName("REQUESTED_FILES_IDS", m_myUID);

        SQLite::Statement query(m_db,
            "SELECT FILE_ID FROM " + tableName);

        while (query.executeStep()) {
            files.push_back(query.getColumn(0).getString());
        }

    }
    catch (const SQLite::Exception& e) {
        std::cerr << "Error getting files: " << e.what() << std::endl;
        return std::nullopt;
    }

    return files;
}


// BLOB_BUFFERS
bool Database::addBlobBuffer(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& blobUid, const std::string& senderUid, const std::string& timestamp, int filesCountInBlob, const std::string& caption)
{
    try {
        std::string tableName = constructTableName("BLOB_BUFFERS", m_myUID);
        SQLite::Statement query(m_db,
            "INSERT INTO " + tableName +
            " (BLOB_UID, SENDER_UID, TIMESTAMP, FILES_COUNT_IN_BLOB, RECEIVED_FILES_COUNT, CAPTION, CAPTION_KEY) "
            "VALUES (?, ?, ?, ?, 0, ?, ?)");

        CryptoPP::SecByteBlock aesKey;
        utility::generateAESKey(aesKey);
        const std::string encryptedCaption = utility::AESEncrypt(aesKey, caption);

        const std::string encryptedAesKey = utility::RSAEncryptKey(myPublicKey, aesKey);

        query.bind(1, blobUid);
        query.bind(2, senderUid);
        query.bind(3, timestamp);
        query.bind(4, filesCountInBlob);
        query.bind(5, encryptedCaption);
        query.bind(6, encryptedAesKey);

        query.exec();
        return true;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[addBlobBuffer] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::removeBlobBuffer(const std::string& blobUid) {
    try {
        SQLite::Transaction txn(m_db);

        const std::string buffersTable = constructTableName("BLOB_BUFFERS", m_myUID);
        const std::string filesTable = constructTableName("BLOB_BUFFER_FILES", m_myUID);

        {
            SQLite::Statement delFiles(m_db,
                "DELETE FROM " + filesTable + " WHERE BLOB_UID = ?");
            delFiles.bind(1, blobUid);
            delFiles.exec();
        }

        int changes = 0;
        {
            SQLite::Statement delBlob(m_db,
                "DELETE FROM " + buffersTable + " WHERE BLOB_UID = ?");
            delBlob.bind(1, blobUid);
            changes = delBlob.exec();
        }

        txn.commit();
        return changes > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[removeBlobBuffer] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::isBlobBuffer(const std::string& blobUid) {
    try {
        std::string tableName = constructTableName("BLOB_BUFFERS", m_myUID);
        SQLite::Statement query(m_db,
            "SELECT 1 FROM " + tableName + " WHERE BLOB_UID = ? LIMIT 1");
        query.bind(1, blobUid);
        return query.executeStep();
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[isBlobBuffer] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::incrementReceivedFilesCountInBlobBuffer(const std::string& blobUid) {
    try {
        std::string tableName = constructTableName("BLOB_BUFFERS", m_myUID);
        SQLite::Statement query(m_db,
            "UPDATE " + tableName +
            " SET RECEIVED_FILES_COUNT = RECEIVED_FILES_COUNT + 1 "
            "WHERE BLOB_UID = ? AND RECEIVED_FILES_COUNT < FILES_COUNT_IN_BLOB");
        query.bind(1, blobUid);
        return query.exec() > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[incrementReceivedFilesCountInBlobBuffer] SQL error: " << e.what() << std::endl;
        return false;
    }
}

std::optional<int> Database::getReceivedFilesCountInBlobBuffer(const std::string& blobUid) {
    try {
        std::string tableName = constructTableName("BLOB_BUFFERS", m_myUID);
        SQLite::Statement query(m_db,
            "SELECT RECEIVED_FILES_COUNT FROM " + tableName + " WHERE BLOB_UID = ?");
        query.bind(1, blobUid);
        if (query.executeStep()) {
            return query.getColumn(0).getInt();
        }
        return std::nullopt;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[getReceivedFilesCountInBlobBuffer] SQL error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

BlobPtr Database::getBlobBuffer(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& blobUid)
{
    try {
        const std::string buffersTable = constructTableName("BLOB_BUFFERS", m_myUID);
        SQLite::Statement blobStmt(m_db,
            "SELECT TIMESTAMP, FILES_COUNT_IN_BLOB, CAPTION, CAPTION_KEY FROM " + buffersTable +
            " WHERE BLOB_UID = ?");
        blobStmt.bind(1, blobUid);

        std::string timestamp;
        int filesCount = 0;
        std::string encryptedCaption;
        std::string encryptedCaptionKey;

        if (!blobStmt.executeStep()) {
            return nullptr;
        }

        timestamp = blobStmt.getColumn(0).getString();
        filesCount = blobStmt.getColumn(1).getInt();
        encryptedCaption = blobStmt.getColumn(2).getString();
        encryptedCaptionKey = blobStmt.getColumn(3).getString();

        std::string decryptedCaption;
        try {
            CryptoPP::SecByteBlock aesKey = utility::RSADecryptKey(privateKey, encryptedCaptionKey);
            decryptedCaption = utility::AESDecrypt(aesKey, encryptedCaption);
        }
        catch (const std::exception& e) {
            std::cerr << "[getBlobBuffer] Caption decrypt error: " << e.what() << std::endl;
            decryptedCaption.clear();
        }

        const std::string filesTable = constructTableName("BLOB_BUFFER_FILES", m_myUID);
        SQLite::Statement filesStmt(m_db,
            "SELECT FILE_ID, FILE_NAME, FILE_SIZE, FILE_PATH FROM " + filesTable +
            " WHERE BLOB_UID = ?");
        filesStmt.bind(1, blobUid);

        std::unordered_map<std::string, File> relatedFiles;
        while (filesStmt.executeStep()) {
            const std::string fileId = filesStmt.getColumn(0).getString();
            const std::string fileName = filesStmt.getColumn(1).getString();
            const std::string fileSizeStr = filesStmt.getColumn(2).getString();
            const std::string filePath = filesStmt.getColumn(3).getString();

            uint32_t fileSize = 0;
            try { fileSize = static_cast<uint32_t>(std::stoul(fileSizeStr)); } catch (...) { fileSize = 0; }

            File file(fileSize, filePath, fileName, fileId, false, false, true);
            relatedFiles.emplace(fileId, std::move(file));
        }

        BlobPtr blob= std::make_shared<Blob>(relatedFiles, timestamp, decryptedCaption, blobUid, filesCount);
        return blob;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[getBlobBuffer] SQL error: " << e.what() << std::endl;
        return nullptr;
    }
}


// BLOB_BUFFER_FILES
bool Database::addFileToBlobBuffer(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& blobUid, const std::string& fileId, const std::string& fileName, const std::string& fileSize, const std::string& filePath)
{
    try {
        const std::string tableName = constructTableName("BLOB_BUFFER_FILES", m_myUID);
        SQLite::Statement stmt(m_db,
            "INSERT INTO " + tableName +
            " (BLOB_UID, FILE_NAME, FILE_SIZE, FILE_PATH, FILE_KEY, FILE_ID) VALUES (?, ?, ?, ?, ?, ?)"
        );

        CryptoPP::SecByteBlock aesKey;
        utility::generateAESKey(aesKey);
        const std::string encryptedAesKey = utility::RSAEncryptKey(myPublicKey, aesKey);

        stmt.bind(1, blobUid);
        stmt.bind(2, fileName);
        stmt.bind(3, fileSize);
        stmt.bind(4, filePath);
        stmt.bind(5, encryptedAesKey);
        stmt.bind(6, fileId);
        stmt.exec();
        return true;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[addBufferFile] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::removeFileFromBlobBuffer(const std::string& fileId)
{
    try {
        const std::string tableName = constructTableName("BLOB_BUFFER_FILES", m_myUID);
        SQLite::Statement stmt(m_db,
            "DELETE FROM " + tableName + " WHERE FILE_ID = ?");
        stmt.bind(1, fileId);

        return stmt.exec() > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[removeBufferFile] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::isFileInBlobBuffer(const std::string& fileId)
{
    try {
        const std::string tableName = constructTableName("BLOB_BUFFER_FILES", m_myUID);
        SQLite::Statement stmt(m_db,
            "SELECT 1 FROM " + tableName + " WHERE FILE_ID = ? LIMIT 1");
        stmt.bind(1, fileId);

        return stmt.executeStep();
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[isBufferFile] SQL error: " << e.what() << std::endl;
        return false;
    }
}


// BLOBS
bool Database::addBlob(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& friendUID, BlobPtr blob)
{
    try {
        CryptoPP::SecByteBlock aesKey;
        utility::generateAESKey(aesKey);

        const std::string serialized = blob->serialize();
        const std::string encPayload = utility::AESEncrypt(aesKey, serialized);
        const std::string encAesKey = utility::RSAEncryptKey(myPublicKey, aesKey);

        const std::string tableName = constructTableName("BLOBS", m_myUID);
        SQLite::Statement stmt(m_db,
            "INSERT INTO " + tableName + " (ID, FRIEND_UID, TIMESTAMP, BLOB, BLOB_KEY) VALUES (?, ?, ?, ?, ?)");

        stmt.bind(1, blob->getBlobId());
        stmt.bind(2, friendUID);
        stmt.bind(3, blob->getTimestamp());
        stmt.bind(4, encPayload);
        stmt.bind(5, encAesKey);

        stmt.exec();
        return true;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[addBlob] SQL error: " << e.what() << std::endl;
        return false;
    }
}

BlobPtr Database::getBlob(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& blobUid)
{
    try {
        const std::string tableName = constructTableName("BLOBS", m_myUID);
        SQLite::Statement stmt(m_db,
            "SELECT FRIEND_UID, TIMESTAMP, BLOB, BLOB_KEY FROM " + tableName + " WHERE ID = ? LIMIT 1");
        stmt.bind(1, blobUid);

        if (!stmt.executeStep()) {
            return nullptr;
        }

        const std::string friendUID = stmt.getColumn(0).getString();
        const std::string timestamp = stmt.getColumn(1).getString();
        const std::string encPayload = stmt.getColumn(2).getString();
        const std::string encAesKey = stmt.getColumn(3).getString();

        CryptoPP::SecByteBlock aesKey = utility::RSADecryptKey(privateKey, encAesKey);
        const std::string payload = utility::AESDecrypt(aesKey, encPayload);

        BlobPtr blob = Blob::deserialize(payload);
        return blob;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[getBlob] SQL error: " << e.what() << std::endl;
        return nullptr;
    }
    catch (const std::exception& e) {
        std::cerr << "[getBlob] Decrypt/deserialize error: " << e.what() << std::endl;
        return nullptr;
    }
}

bool Database::removeBlob(const std::string& blobUid) {
    try {
        const std::string tableName = constructTableName("BLOBS", m_myUID);
        SQLite::Statement stmt(m_db,
            "DELETE FROM " + tableName + " WHERE ID = ?");
        stmt.bind(1, blobUid);
        return stmt.exec() > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[removeBlob] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::saveBlobs(const CryptoPP::RSA::PublicKey& publicKey, const std::string& friendUID, const std::vector<BlobPtr>& blobs)
{
    if (blobs.empty()) 
        return true;

    try {
        SQLite::Transaction txn(m_db);
        const std::string tableName = constructTableName("BLOBS", m_myUID);
        SQLite::Statement insertStmt(m_db, "INSERT OR REPLACE INTO " + tableName + " (ID, FRIEND_UID, TIMESTAMP, BLOB, BLOB_KEY) VALUES (?, ?, ?, ?, ?)");

        for (const auto blobPtr : blobs) {
            if (!blobPtr) continue;
            CryptoPP::SecByteBlock aesKey; utility::generateAESKey(aesKey);
            const std::string encPayload = utility::AESEncrypt(aesKey, blobPtr->serialize());
            const std::string encAesKey = utility::RSAEncryptKey(publicKey, aesKey);

            insertStmt.bind(1, blobPtr->getBlobId());
            insertStmt.bind(2, friendUID);
            insertStmt.bind(3, blobPtr->getTimestamp());
            insertStmt.bind(4, encPayload);
            insertStmt.bind(5, encAesKey);
            insertStmt.exec();
            insertStmt.reset();
            insertStmt.clearBindings();
        }
        txn.commit();
        return true;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[saveBlobs] SQL error: " << e.what() << std::endl;
        return false;
    }
}

void Database::loadBlobs(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& friendUID, std::vector<BlobPtr>& blobs) const
{
    blobs.clear();
    try {
        const std::string tableName = constructTableName("BLOBS", m_myUID);
        SQLite::Statement stmt(m_db, "SELECT ID, TIMESTAMP, BLOB, BLOB_KEY FROM " + tableName + " WHERE FRIEND_UID = ? ORDER BY TIMESTAMP ASC");
        stmt.bind(1, friendUID);
        while (stmt.executeStep()) {
            const std::string id = stmt.getColumn(0).getString();
            const std::string timestamp = stmt.getColumn(1).getString();
            const std::string encPayload = stmt.getColumn(2).getString();
            const std::string encAesKey = stmt.getColumn(3).getString();
            CryptoPP::SecByteBlock aesKey = utility::RSADecryptKey(privateKey, encAesKey);
            const std::string payload = utility::AESDecrypt(aesKey, encPayload);
            auto blobPtr = Blob::deserialize(payload);
            if (blobPtr) {
                blobs.push_back(blobPtr);
            }
        }
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[loadBlobs] SQL error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "[loadBlobs] Decrypt/deserialize error: " << e.what() << std::endl;
    }
}

bool Database::deleteAllBlobs(const std::string& friendUID) const
{
    try {
        const std::string tableName = constructTableName("BLOBS", m_myUID);
        SQLite::Statement stmt(m_db, "DELETE FROM " + tableName + " WHERE FRIEND_UID = ?");
        stmt.bind(1, friendUID);
        return stmt.exec() > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[deleteAllBlobs] SQL error: " << e.what() << std::endl;
        return false;
    }
}


// MESSAGES
bool Database::addMessage(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& friendUID, MessagePtr message)
{
    try {
        CryptoPP::SecByteBlock aesKey;
        utility::generateAESKey(aesKey);
        const std::string serialized = message->serialize();
        const std::string encPayload = utility::AESEncrypt(aesKey, serialized);
        const std::string encAesKey = utility::RSAEncryptKey(myPublicKey, aesKey);

        const std::string tableName = constructTableName("MESSAGES", m_myUID);
        SQLite::Statement stmt(m_db,
            "INSERT INTO " + tableName + " (ID, FRIEND_UID, TIMESTAMP, MESSAGE, MESSAGE_KEY) VALUES (?, ?, ?, ?, ?)" );
        stmt.bind(1, message->getId());
        stmt.bind(2, friendUID);
        stmt.bind(3, message->getTimestamp());
        stmt.bind(4, encPayload);
        stmt.bind(5, encAesKey);
        stmt.exec();
        return true;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[addMessage] SQL error: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::string> Database::getMessage(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& id)
{
    try {
        const std::string tableName = constructTableName("MESSAGES", m_myUID);
        SQLite::Statement stmt(m_db,
            "SELECT MESSAGE, MESSAGE_KEY FROM " + tableName + " WHERE ID = ? LIMIT 1");
        stmt.bind(1, id);
        if (!stmt.executeStep()) {
            return std::nullopt;
        }
        const std::string encPayload = stmt.getColumn(0).getString();
        const std::string encAesKey = stmt.getColumn(1).getString();
        CryptoPP::SecByteBlock aesKey = utility::RSADecryptKey(privateKey, encAesKey);
        const std::string payload = utility::AESDecrypt(aesKey, encPayload);
        return payload;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[getMessage] SQL error: " << e.what() << std::endl;
        return std::nullopt;
    }
    catch (const std::exception& e) {
        std::cerr << "[getMessage] Decrypt error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool Database::removeMessage(const std::string& id)
{
    try {
        const std::string tableName = constructTableName("MESSAGES", m_myUID);
        SQLite::Statement stmt(m_db,
            "DELETE FROM " + tableName + " WHERE ID = ?");
        stmt.bind(1, id);
        return stmt.exec() > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[removeMessage] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool Database::saveMessages(const CryptoPP::RSA::PublicKey& publicKey, const std::string& friendUID, const std::vector<MessagePtr>& messages)
{
    if (messages.empty())
        return true;

    try {
        SQLite::Transaction txn(m_db);
        const std::string tableName = constructTableName("MESSAGES", m_myUID);

        SQLite::Statement insertStmt(m_db, "INSERT OR REPLACE INTO " + tableName + " (ID, FRIEND_UID, TIMESTAMP, MESSAGE, MESSAGE_KEY) VALUES (?, ?, ?, ?, ?)");

        for (const auto msgPtr : messages) {
            if (!msgPtr) continue;
            CryptoPP::SecByteBlock aesKey; utility::generateAESKey(aesKey);
            const std::string encPayload = utility::AESEncrypt(aesKey, msgPtr->serialize());
            const std::string encAesKey = utility::RSAEncryptKey(publicKey, aesKey);

            insertStmt.bind(1, msgPtr->getId());
            insertStmt.bind(2, friendUID);
            insertStmt.bind(3, msgPtr->getTimestamp());
            insertStmt.bind(4, encPayload);
            insertStmt.bind(5, encAesKey);
            insertStmt.exec();
            insertStmt.reset();
            insertStmt.clearBindings();
        }

        txn.commit();
        return true;
    } catch (const SQLite::Exception& e) {
        std::cerr << "[saveMessages] SQL error: " << e.what() << std::endl;
        return false;
    }
}

void Database::loadMessages(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& friendUID, std::vector<MessagePtr>& messages) const
{
    messages.clear();
    try {
        const std::string tableName = constructTableName("MESSAGES", m_myUID);
        SQLite::Statement stmt(m_db, "SELECT ID, TIMESTAMP, MESSAGE, MESSAGE_KEY FROM " + tableName + " WHERE FRIEND_UID = ? ORDER BY TIMESTAMP ASC");
        stmt.bind(1, friendUID);
        while (stmt.executeStep()) {
            const std::string id = stmt.getColumn(0).getString();
            const std::string timestamp = stmt.getColumn(1).getString();
            const std::string encPayload = stmt.getColumn(2).getString();
            const std::string encAesKey = stmt.getColumn(3).getString();
            CryptoPP::SecByteBlock aesKey = utility::RSADecryptKey(privateKey, encAesKey);
            const std::string payload = utility::AESDecrypt(aesKey, encPayload);
            auto msgPtr = Message::deserialize(payload);
            if (msgPtr) {
                messages.push_back(msgPtr);
            }
        }
    } catch (const SQLite::Exception& e) {
        std::cerr << "[loadMessages] SQL error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[loadMessages] Decrypt/deserialize error: " << e.what() << std::endl;
    }
}

bool Database::deleteAllMessages(const std::string& friendUID) const
{
    try {
        const std::string tableName = constructTableName("MESSAGES", m_myUID);
        SQLite::Statement stmt(m_db, "DELETE FROM " + tableName + " WHERE FRIEND_UID = ?");
        stmt.bind(1, friendUID);
        return stmt.exec() > 0;
    } catch (const SQLite::Exception& e) {
        std::cerr << "[deleteAllMessages] SQL error: " << e.what() << std::endl;
        return false;
    }
}








std::string Database::constructTableName(const std::string& tableName, const std::string& myUID) const {
    if (myUID.empty()) {
        throw std::invalid_argument("user UID cannot be empty");
    }

    return tableName + myUID;
}
