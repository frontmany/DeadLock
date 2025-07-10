# pragma once

#include<fstream>
#include<string>
#include<iostream>

#include "net_file.h"
#include "queryType.h"

struct fileWrapper {
    fileWrapper() = default;

    fileWrapper(bool isPresent, net::file<QueryType> file) {
        this->isPresent = isPresent;
        this->file = file;
        isNeedToRetry = false;
        isSending = false;
    }

    bool isNeedToRetry;
    bool isSending;
	bool isPresent;
    net::file<QueryType> file;

    bool checkFilePresence() {
        std::ifstream fileStream(file.filePath);
        isPresent = fileStream.good();
        return isPresent;
    }
};