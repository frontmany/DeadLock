#include"database.h" 
#include "photo.h" 
#include"utility.h" 


void Database::init() {
    m_sqlite3_dll = LoadLibrary(L"sqlite3.dll");
    if (!m_sqlite3_dll) {
        std::cerr << "Error loading sqlite3.dll" << std::endl;
        return;
    }

    sqlite3_open = (sqlite3_open_t)GetProcAddress(m_sqlite3_dll, "sqlite3_open");
    sqlite3_exec = (sqlite3_exec_t)GetProcAddress(m_sqlite3_dll, "sqlite3_exec");
    sqlite3_close = (sqlite3_close_t)GetProcAddress(m_sqlite3_dll, "sqlite3_close");
    sqlite3_errmsg = (sqlite3_errmsg_t)GetProcAddress(m_sqlite3_dll, "sqlite3_errmsg");
    sqlite3_changes = (sqlite3_changes_t)GetProcAddress(m_sqlite3_dll, "sqlite3_changes");
    sqlite3_free = (sqlite3_free_t)GetProcAddress(m_sqlite3_dll, "sqlite3_free");
    sqlite3_prepare_v2 = (sqlite3_prepare_v2_t)GetProcAddress(m_sqlite3_dll, "sqlite3_prepare_v2");
    sqlite3_bind_text = (sqlite3_bind_text_t)GetProcAddress(m_sqlite3_dll, "sqlite3_bind_text");
    sqlite3_step = (sqlite3_step_t)GetProcAddress(m_sqlite3_dll, "sqlite3_step");
    sqlite3_column_text = (sqlite3_column_text_t)GetProcAddress(m_sqlite3_dll, "sqlite3_column_text");
    sqlite3_column_int = (sqlite3_column_int_t)GetProcAddress(m_sqlite3_dll, "sqlite3_column_int");
    sqlite3_finalize = (sqlite3_finalize_t)GetProcAddress(m_sqlite3_dll, "sqlite3_finalize");

    // Проверка загрузки функций
    if (!sqlite3_open || !sqlite3_exec || !sqlite3_close || !sqlite3_errmsg || !sqlite3_changes) {
        std::cerr << "Error obtaining function addresses for SQLite" << std::endl;
        FreeLibrary(m_sqlite3_dll);
        return;
    }

    // Инициализация базы данных
    int rc = sqlite3_open("your_database.db", &m_db);
    if (rc != SQLITE_OK) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(m_db) << std::endl;
        FreeLibrary(m_sqlite3_dll);
        return;
    }

    char* zErrMsg = nullptr;
    const char* sql = "CREATE TABLE IF NOT EXISTS MESSAGES("
        "LOGIN          TEXT              NOT NULL,"
        "MSGS           TEXT              NOT NULL);";

    rc = sqlite3_exec(m_db, sql, 0, 0, &zErrMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << zErrMsg << std::endl;
        sqlite3_free(zErrMsg); // Освобождение памяти для сообщения об ошибке
        sqlite3_close(m_db);    // Закрытие базы данных
        FreeLibrary(m_sqlite3_dll); // Освобождение библиотеки
        return;
    }
    else {
        std::cout << "Table MESSAGES created successfully" << std::endl;
    }

    // Закрытие библиотеки, если вы не собираетесь использовать ее больше
    // FreeLibrary(m_sqlite3_dll); // Возможно, вам нужно оставить библиотеку открытой для дальнейшего использования
}


void Database::saveMessages(const std::string& login, std::vector<Message*> messages) const {
    if (messages.empty() || login.empty()) {
        std::cerr << "No messages to save or login is empty" << std::endl;
        return;
    }

    // Сериализация сообщений
    std::ostringstream oss;
    for (size_t i = 0; i < messages.size(); ++i) {
        oss << messages[i]->serialize();
        if (i < messages.size() - 1) {
            oss << ',';
        }
    }
    std::string text = oss.str();

    // Проверка наличия записи
    const char* selectSql = "SELECT MSGS FROM MESSAGES WHERE LOGIN = ?;";
    sqlite3_stmt* selectStmt = nullptr;

    int rc = sqlite3_prepare_v2(m_db, selectSql, -1, (void**)&selectStmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare SELECT statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    rc = sqlite3_bind_text(selectStmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to bind login in SELECT statement: " << sqlite3_errmsg(m_db) << std::endl;
        sqlite3_finalize(selectStmt);
        return;
    }

    bool recordExists = (sqlite3_step(selectStmt) == SQLITE_ROW);
    sqlite3_finalize(selectStmt);

    // Выбор SQL-запроса в зависимости от наличия записи
    const char* sql;
    if (recordExists) {
        sql = "UPDATE MESSAGES SET MSGS = ? WHERE LOGIN = ?;";
    }
    else {
        sql = "INSERT INTO MESSAGES (LOGIN, MSGS) VALUES (?, ?);";
    }

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(m_db, sql, -1, (void**)&stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return;
    }

    // Привязка параметров
    if (recordExists) {
        rc = sqlite3_bind_text(stmt, 1, text.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind messages: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return;
        }

        rc = sqlite3_bind_text(stmt, 2, login.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind login: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return;
        }
    }
    else {
        rc = sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind login: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return;
        }

        rc = sqlite3_bind_text(stmt, 2, text.c_str(), -1, SQLITE_TRANSIENT);
        if (rc != SQLITE_OK) {
            std::cerr << "Failed to bind messages: " << sqlite3_errmsg(m_db) << std::endl;
            sqlite3_finalize(stmt);
            return;
        }
    }

    // Выполнение запроса
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Execution failed: " << sqlite3_errmsg(m_db) << std::endl;
    }
    else {
        std::cout << (recordExists ? "Messages updated successfully" : "Messages added successfully") << std::endl;
    }

    sqlite3_finalize(stmt);
}


std::vector<Message*> Database::loadMessages(const std::string& login) const {
    std::vector<Message*> messages;

    const char* sql = "SELECT MSGS FROM MESSAGES WHERE LOGIN = ?;";
    sqlite3_stmt* stmt = nullptr;
    int rc;

    rc = sqlite3_prepare_v2(m_db, sql, -1, (void**)&stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(m_db) << std::endl;
        return messages;
    }

    sqlite3_bind_text(stmt, 1, login.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* messageText = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        if (messageText) {
            std::string messagesStr(messageText);
            std::istringstream ss(messagesStr);
            std::string messagePart;

            while (std::getline(ss, messagePart, ',')) {
                Message* message = Message::deserialize(messagePart);
                if (message) {
                    messages.push_back(message);
                }
            }
        }
    }
    else {
        std::cerr << "No messages found for user: " << login << std::endl;
    }

    sqlite3_finalize(stmt);
    return messages;
}


std::string Database::friendsToString(const std::vector<std::string>& friends) {
    std::stringstream ss;
    for (size_t i = 0; i < friends.size(); ++i) {
        ss << friends[i];
        if (i < friends.size() - 1) {
            ss << ",";
        }
    }
    return ss.str();
}

std::vector<std::string> Database::stringToFriends(const std::string& friendsString) {
    std::vector<std::string> friends;
    std::stringstream ss(friendsString);
    std::string friendLogin;
    while (std::getline(ss, friendLogin, ',')) {
        friends.push_back(friendLogin);
    }
    return friends;
}

