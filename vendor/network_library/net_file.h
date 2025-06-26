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
    };


    template <typename T>
    struct owned_file {
        std::shared_ptr<files_connection<T>> remote = nullptr;
        file<T> file;
    };
}