#pragma once

#include <string>

struct FileDataTemporaryContainer {
	std::string blobId;
	std::string fileId;
	std::string fileName;
	std::string filePath;
	std::string friendUID;
	uint32_t fileSize;
};