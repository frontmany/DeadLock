#pragma once

#include <string>
#include <memory>
#include <rsa.h>

namespace net
{
    struct File {
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
        bool isAvatar = false;
        std::string ifFileIsAvatarFriendLoginHashesList;

        File() = default;

        File& operator=(const File& other) {
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

        File& operator=(File&& other) noexcept {
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

        File(File&& other) noexcept
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

        File(const File& other) noexcept
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
}