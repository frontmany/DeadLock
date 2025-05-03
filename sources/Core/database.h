#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

#include"sqlite/sqlite3.h" 
#include"message.h" 

typedef int (*sqlite3_open_t)(const char* filename, void** ppDb);
typedef int (*sqlite3_exec_t)(void* db, const char* sql, int (*callback)(void*, int, char**, char**), void* arg, char** errmsg);
typedef int (*sqlite3_close_t)(void* db);
typedef char* (*sqlite3_errmsg_t)(void* db);
typedef int (*sqlite3_changes_t)(void* db);
typedef void (*sqlite3_free_t)(void* p);
typedef int (*sqlite3_open_t)(const char* filename, void** ppDb);
typedef int (*sqlite3_prepare_v2_t)(void* db, const char* zSql, int nByte, void** ppStmt, const char** pzTail);
typedef int (*sqlite3_bind_text_t)(void* stmt, int idx, const char* zValue, int nByte, void (*xDel)(void*));
typedef int (*sqlite3_step_t)(void* stmt);
typedef const unsigned char* (*sqlite3_column_text_t)(void* stmt, int iCol);
typedef int (*sqlite3_column_int_t)(void* stmt, int iCol);
typedef int (*sqlite3_finalize_t)(void* stmt);

class User;
class Photo;

class Database {
public:
	Database() = default;
	void init();
	void saveMessages(const std::string& login, std::vector<Message*> messages) const;
	std::vector<Message*> loadMessages(const std::string& login, std::vector<Message*>& messages) const ;

private:
	std::string friendsToString(const std::vector<std::string>& friends);
	std::vector<std::string> stringToFriends(const std::string& friendsString);

	sqlite3_open_t sqlite3_open;
	sqlite3_exec_t sqlite3_exec;
	sqlite3_close_t sqlite3_close;
	sqlite3_errmsg_t sqlite3_errmsg;
	sqlite3_changes_t sqlite3_changes;
	sqlite3_free_t sqlite3_free;
	sqlite3_prepare_v2_t sqlite3_prepare_v2;
	sqlite3_bind_text_t sqlite3_bind_text;
	sqlite3_step_t sqlite3_step;
	sqlite3_column_text_t sqlite3_column_text;
	sqlite3_column_int_t sqlite3_column_int;
	sqlite3_finalize_t sqlite3_finalize;

private:
	const std::string delimiter = "--8d45f2a1-3c7b-4e9d-a2f6-1b0c9e3d5a7f--";
	void* m_db;
};