#pragma once

#include <string>
#include <memory>
#include <rsa.h>

namespace net
{
    template <typename T>
    class files_connection;

    template <typename T>
    struct file {
        CryptoPP::RSA::PublicKey friendPublicKey;
        std::string blobUID;
        std::string senderLoginHash;
        std::string receiverLoginHash;
        std::string filePath;
        std::string fileName;
        std::string id;
        std::string timestamp;
        std::string caption;
        std::string fileSize;
        std::string filesInBlobCount;
        std::string encryptedKey;

        file() = default;

        file& operator=(const file& other) {
            if (this != &other) {
                friendPublicKey = other.friendPublicKey;
                blobUID = other.blobUID;
                senderLoginHash = other.senderLoginHash;
                receiverLoginHash = other.receiverLoginHash;
                filePath = other.filePath;
                fileName = other.fileName;
                id = other.id;
                timestamp = other.timestamp;
                caption = other.caption;
                fileSize = other.fileSize;
                filesInBlobCount = other.filesInBlobCount;
                encryptedKey = other.encryptedKey;
            }

            return *this; 
        }

        file& operator=(file&& other) noexcept {
            if (this != &other) {
                friendPublicKey = std::move(other.friendPublicKey);
                blobUID = std::move(other.blobUID);
                senderLoginHash = std::move(other.senderLoginHash);
                receiverLoginHash = std::move(other.receiverLoginHash);
                filePath = std::move(other.filePath);
                fileName = std::move(other.fileName);
                id = std::move(other.id);
                timestamp = std::move(other.timestamp);
                caption = std::move(other.caption);
                fileSize = std::move(other.fileSize);
                filesInBlobCount = std::move(other.filesInBlobCount);
                encryptedKey = std::move(other.encryptedKey);
            }

            return *this;
        }

        file(file&& other) noexcept
            : friendPublicKey(std::move(other.friendPublicKey)),
            blobUID(std::move(other.blobUID)),
            senderLoginHash(std::move(other.senderLoginHash)),
            receiverLoginHash(std::move(other.receiverLoginHash)),
            filePath(std::move(other.filePath)),
            fileName(std::move(other.fileName)),
            id(std::move(other.id)),
            timestamp(std::move(other.timestamp)),
            caption(std::move(other.caption)),
            fileSize(std::move(other.fileSize)),
            filesInBlobCount(std::move(other.filesInBlobCount)),
            encryptedKey(std::move(other.encryptedKey)){
        }

        file(const file& other) noexcept
            : friendPublicKey(other.friendPublicKey),
            blobUID(other.blobUID),
            senderLoginHash(other.senderLoginHash),
            receiverLoginHash(other.receiverLoginHash),
            filePath(other.filePath),
            fileName(other.fileName),
            id(other.id),
            timestamp(other.timestamp),
            caption(other.caption),
            fileSize(other.fileSize),
            filesInBlobCount(other.filesInBlobCount),
            encryptedKey(other.encryptedKey) {
        }
    };


    template <typename T>
    struct owned_file {
        std::shared_ptr<files_connection<T>> remote = nullptr;
        file<T> file;
    };
}