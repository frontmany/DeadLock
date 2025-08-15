#pragma once

#include <string>
#include <memory>
#include <optional>

#include "SQLiteCpp/Transaction.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "rsa.h"

class Message;

class TasksManager {
public:
	TasksManager();
	~TasksManager() = default;

	void init(const std::string& myUID);

	// SEND_COMMON_PACKET_TASKS
	bool addSendCommonPacketTask(uint32_t packetType);
	bool removeSendCommonPacketTask(uint32_t packetType);
	bool isSendCommonPacketTask(uint32_t packetType);
	std::optional<std::vector<uint32_t>> getAllSendCommonPacketTasks();

	// SEND_READ_CONFIRMATION_TASKS
	bool addSendReadConfirmationTask(const std::string& id, const std::string& friendUID);
	bool removeSendReadConfirmationTask(const std::string& id);
	bool isSendReadConfirmationTask(const std::string& id);
	std::optional<std::vector<std::pair<std::string, std::string>>> getAllSendReadConfirmationTasks();

private:
	std::string constructTableName(const std::string& tableName, const std::string& loginHash) const;

private:
	std::unique_ptr<SQLite::Database> m_tasksManager;
	std::string m_myUID;
};