#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include"sqlite/sqlite3.h" 
#include"message.h" 

class User;
class Photo;

class Database {
public:
	Database() = default;
	~Database();

	void init(const std::string& login);
	void saveMessages(const std::string& myLogin, const std::string& friendLogin, std::vector<Message*> messages) const;
	std::vector<Message*> loadMessages(const std::string& myLogin, const std::string& friendLogin, std::vector<Message*>& messages) const;
	void deleteAllMessages(const std::string& myLogin, const std::string& friendLogin) const;

private:
	const std::string delimiter = "--8d45f2a1-3c7b-4e9d-a2f6-1b0c9e3d5a7f--";
	sqlite3* m_db;
};