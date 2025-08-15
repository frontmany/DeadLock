#include "net_tasksManager.h"
#include "utility.h"

TasksManager::TasksManager() {
    try {
        m_tasksManager = std::make_unique<SQLite::Database>("tasksManager.db", SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE);
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "Database error: " << e.what() << std::endl;
        throw;
    }
}

void TasksManager::init(const std::string& myUID)
{
    m_myUID = myUID;

    if (!m_tasksManager) {
        throw SQLite::Exception("Database is not open or initialized");
    }

    try {
        SQLite::Transaction transaction(*m_tasksManager);

        std::string commonPacketsTasksTableName = constructTableName("SEND_COMMON_PACKET_TASKS", m_myUID);
        std::string sqlSendCommonPacketTasks = "CREATE TABLE IF NOT EXISTS " + commonPacketsTasksTableName + " ("
            "PACKET_TYPE INTEGER NOT NULL,"
            "PRIMARY KEY (PACKET_TYPE)"
            ");";
        m_tasksManager->exec(sqlSendCommonPacketTasks);


        std::string readConfirmationTasksTableName = constructTableName("SEND_READ_CONFIRMATION_TASKS", m_myUID);
        std::string sqlSendReadConfirmationTasks = "CREATE TABLE IF NOT EXISTS " + readConfirmationTasksTableName + " ("
            "ID         TEXT NOT NULL,"
            "FRIEND_UID TEXT NOT NULL,"
            "PRIMARY KEY (ID)"
            ");";
        m_tasksManager->exec(sqlSendReadConfirmationTasks);


        transaction.commit();
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[init] SQL error: " << e.what() << std::endl;
        throw;

    }
}

// SEND_COMMON_PACKET_TASKS
bool TasksManager::addSendCommonPacketTask(uint32_t packetType)
{
    try {
        const std::string tableName = constructTableName("SEND_COMMON_PACKET_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "INSERT OR IGNORE INTO " + tableName + " (PACKET_TYPE) VALUES (?)");
        stmt.bind(1, packetType);
        stmt.exec();
        return true;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[addSendCommonPacketTask] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool TasksManager::removeSendCommonPacketTask(uint32_t packetType)
{
    try {
        const std::string tableName = constructTableName("SEND_COMMON_PACKET_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "DELETE FROM " + tableName + " WHERE PACKET_TYPE = ?");
        stmt.bind(1, packetType);
        return stmt.exec() > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[removeSendCommonPacketTask] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool TasksManager::isSendCommonPacketTask(uint32_t packetType)
{
    try {
        const std::string tableName = constructTableName("SEND_COMMON_PACKET_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "SELECT 1 FROM " + tableName + " WHERE PACKET_TYPE = ? LIMIT 1");
        stmt.bind(1, packetType);
        return stmt.executeStep();
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[isSendCommonPacketTask] SQL error: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::vector<uint32_t>> TasksManager::getAllSendCommonPacketTasks()
{
    try {
        const std::string tableName = constructTableName("SEND_COMMON_PACKET_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "SELECT PACKET_TYPE FROM " + tableName);
        std::vector<uint32_t> result;
        while (stmt.executeStep()) {
            result.push_back(stmt.getColumn(0).getInt());
        }
        return result;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[getAllSendCommonPacketTasks] SQL error: " << e.what() << std::endl;
        return std::nullopt;
    }
}


// SEND_READ_CONFIRMATION_TASKS
bool TasksManager::addSendReadConfirmationTask(const std::string& id, const std::string& friendUID)
{
    try {
        const std::string tableName = constructTableName("SEND_READ_CONFIRMATION_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "INSERT OR IGNORE INTO " + tableName + " (ID, FRIEND_UID) VALUES (?, ?)");
        stmt.bind(1, id);
        stmt.bind(2, friendUID);
        stmt.exec();
        return true;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[addSendReadConfirmationTask] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool TasksManager::removeSendReadConfirmationTask(const std::string& id)
{
    try {
        const std::string tableName = constructTableName("SEND_READ_CONFIRMATION_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "DELETE FROM " + tableName + " WHERE ID = ?");
        stmt.bind(1, id);
        return stmt.exec() > 0;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[removeSendReadConfirmationTask] SQL error: " << e.what() << std::endl;
        return false;
    }
}

bool TasksManager::isSendReadConfirmationTask(const std::string& id)
{
    try {
        const std::string tableName = constructTableName("SEND_READ_CONFIRMATION_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "SELECT 1 FROM " + tableName + " WHERE ID = ? LIMIT 1");
        stmt.bind(1, id);
        return stmt.executeStep();
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[isSendReadConfirmationTask] SQL error: " << e.what() << std::endl;
        return false;
    }
}

std::optional<std::vector<std::pair<std::string, std::string>>> TasksManager::getAllSendReadConfirmationTasks()
{
    try {
        const std::string tableName = constructTableName("SEND_READ_CONFIRMATION_TASKS", m_myUID);
        SQLite::Statement stmt(*m_tasksManager,
            "SELECT ID, FRIEND_UID FROM " + tableName);
        std::vector<std::pair<std::string, std::string>> result;
        while (stmt.executeStep()) {
            const std::string id = stmt.getColumn(0).getString();
            const std::string friendUid = stmt.getColumn(1).getString();
            result.emplace_back(id, friendUid);
        }
        return result;
    }
    catch (const SQLite::Exception& e) {
        std::cerr << "[getAllSendReadConfirmationTasks] SQL error: " << e.what() << std::endl;
        return std::nullopt;
    }
}

















std::string TasksManager::constructTableName(const std::string& tableName, const std::string& myUID) const {
    if (myUID.empty()) {
        throw std::invalid_argument("user UID cannot be empty");
    }

    return tableName + myUID;
}
