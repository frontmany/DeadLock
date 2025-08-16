#pragma once 
#include <memory>
#include <string>

#include "rsa.h"

class File;

typedef std::shared_ptr<File> FilePtr;

struct FileTransferData {
	FilePtr file;
	std::string myUID;
	std::string friendUID;
	std::string timestamp;
	std::string filesCountInBlob;
	std::optional<std::string> caption;
	CryptoPP::RSA::PublicKey friendKey;
};