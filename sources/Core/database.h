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

	void updateTableName(const std::string& oldLoginHash,
		const std::string& newLoginHash);

	void updateFriendLoginHash(const std::string& myLoginHash,
		const std::string& oldLoginHash,
		const std::string& newLoginHash);

	void init(const std::string& loginHash);

	void saveMessages(const CryptoPP::RSA::PublicKey& publicKey,
		const std::string& myLogin,
		const std::string& friendLogin,
		std::vector<Message*> messages) const;
	
	std::vector<Message*> loadMessages(const CryptoPP::RSA::PrivateKey& privateKey,
		const std::string& myLogin,
		const std::string& friendLogin,
		std::vector<Message*>& messages) const;

	void deleteAllMessages(const std::string& myLogin,
		const std::string& friendLogin) const;

private:
	const std::string c_delimiter = "--8d45f2a1-3c7b-4e9d-a2f6-1b0c9e3d5a7f--";
	sqlite3* m_db;
};