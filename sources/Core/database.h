#pragma once

#include <optional>
#include <memory>

#include "SQLiteCpp/Transaction.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "rsa.h"

class User;
class Blob;
class Message;

typedef std::shared_ptr<Blob> BlobPtr;
typedef std::shared_ptr<Message> MessagePtr;

class Database {
public:
	Database();
	~Database() = default;

	void init(const std::string& myUID);

	// REQUESTED_FILES_IDS
	bool addRequestedFileId(const std::string& fileId);
	bool removeRequestedFileId(const std::string& fileId);
	bool isRequestedFileId(const std::string& fileId);
	std::optional<std::vector<std::string>> getAllRequestedFilesIds();


	// BLOB_BUFFERS
	bool addBlobBuffer(const CryptoPP::RSA::PublicKey& myPpublicKey, const std::string& blobUid, const std::string& senderUid, const std::string& timestamp, int filesCountInBlob, const std::string& caption);
	bool removeBlobBuffer(const std::string& blobUid);
	bool isBlobBuffer(const std::string& blobUid);
	bool incrementReceivedFilesCountInBlobBuffer(const std::string& blobUid);
	std::optional<int> getReceivedFilesCountInBlobBuffer(const std::string& blobUid);
	std::optional<std::pair<std::string, BlobPtr>> getBlobBuffer(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& blobUid);
	

	// BLOB_BUFFER_FILES
	bool addFileToBlobBuffer(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& blobUid, const std::string& fileId, const std::string& fileName, const std::string& fileSize, const std::string& filePath);
	    bool removeFileFromBlobBuffer(const std::string& fileId);
	    bool isFileInBlobBuffer(const std::string& fileId);


	// BLOBS
	bool addBlob(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& friendUID, BlobPtr blob);
	BlobPtr getBlob(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& blobUid);
	bool removeBlob(const std::string& blobUid);
	bool saveBlobs(const CryptoPP::RSA::PublicKey& publicKey, const std::string& friendUID, const std::vector<BlobPtr>& blobs);
	void loadBlobs(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& friendUID, std::vector<BlobPtr>& blobs) const;
	bool deleteAllBlobs(const std::string& friendUID) const;


	// MESSAGES
	bool addMessage(const CryptoPP::RSA::PublicKey& myPublicKey, const std::string& friendUID, MessagePtr message);
	std::optional<std::string> getMessage(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& id);
	bool removeMessage(const std::string& id);
	bool saveMessages(const CryptoPP::RSA::PublicKey& publicKey, const std::string& friendUID, const std::vector<MessagePtr>& messages);
	void loadMessages(const CryptoPP::RSA::PrivateKey& privateKey, const std::string& friendUID, std::vector<MessagePtr>& messages) const;
	bool deleteAllMessages(const std::string& friendUID) const;

private:
	std::string constructTableName(const std::string& tableName, const std::string& loginHash) const;

private:
	SQLite::Database m_db;
    std::string m_myUID;
};