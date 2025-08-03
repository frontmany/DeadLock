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
    };
}