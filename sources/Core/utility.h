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
#include <iomanip>
#include <algorithm>

class Chat;

namespace utility {
	std::string hashPassword(std::string password);
	std::string getCurrentDateTime();
	std::string generateId();
	std::string getSaveDir();
	std::string parseDate(const std::string& fulldate);
	std::string getTimeStamp();

	bool isDarkMode();

	void incrementAllChatLayoutIndexes(std::unordered_map<std::string, Chat*>& loginToChatMap);
	void increasePreviousChatIndexes(std::unordered_map<std::string, Chat*>& loginToChatMap, Chat* chat);
}