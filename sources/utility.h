#define WIN32_LEAN_AND_MEAN
#include <Windows.h> 
#include <guiddef.h>
#include <objbase.h>

#include <QString>
#include <QDateTime>
#include <QStringList>

#include <iostream>
#include <ctime>
#include <sstream>
#include <string>
#include <filesystem>
#include <vector>
#include <random>


#include <bcrypt.h>

namespace Utility {
	std::string wideStringToString(const WCHAR* wideStr);

	std::string getCurrentDateTime();
	std::string byteArrayToHexString(const BYTE* data, size_t dataLength);
	std::string getCurrentTime();
	std::string generateId();

	QString getSaveDir();
	QString parseDate(QString fulldate);
}