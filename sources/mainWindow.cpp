#include "mainWindow.h"
#include "clientSide.h"
#include "chatsListComponent.h"
#include <filesystem>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    if (isDarkMode()) {
        m_theme = DARK;
    }
    else {
        m_theme = LIGHT;
    }

    m_chatsWidget = nullptr;
    m_client = new ClientSide(m_chatsWidget);
    m_client->init();
    m_client->connectTo("192.168.1.49", 54000);

    setupLoginWidget();
}


MainWindow::~MainWindow() {
    delete m_chatsWidget;
}


bool MainWindow::isDarkMode() {
    HKEY hKey;
    const TCHAR* subKey = TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize");
    const TCHAR* valueNameApps = TEXT("AppsUseLightTheme");
    const TCHAR* valueNameSystem = TEXT("SystemUsesLightTheme");
    DWORD value;
    DWORD size = sizeof(value);

    if (RegOpenKeyEx(HKEY_CURRENT_USER, subKey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        if (RegQueryValueEx(hKey, valueNameApps, NULL, NULL, (LPBYTE)&value, &size) == ERROR_SUCCESS) {
            RegCloseKey(hKey);
            return value == 0;
        }
        RegCloseKey(hKey);
    }
    return false;
}


void MainWindow::onLogin(bool isLoggedIn) {
    if (isLoggedIn) {
        setupChatsWidget();
    }
}


void MainWindow::setupLoginWidget() {
    m_loginWidget = new LoginWidget(this, this, m_client);
    m_loginWidget->setTheme(m_theme);
    setCentralWidget(m_loginWidget);
}


void MainWindow::setupChatsWidget() {
    QString directoryPath = getSaveDir();

    QDir dir(directoryPath);
    if (!dir.exists()) {
        qWarning() << "The directory was not found:" << directoryPath;
    }
    QStringList fileList = dir.entryList(QDir::Files);

    if (fileList.size() > 0) {
        for (const QString& fileName : fileList) {
            if (fileName.left(fileName.length() - 5) == QString::fromStdString(m_client->getMyInfo().getLogin())) {
                m_chatsWidget = ChatsWidget::deserialize(fileName, this, m_client, m_theme);
                m_chatsWidget->setTheme(m_theme);
                m_chatsWidget->getChatsList()->recoverChatComponents(m_chatsWidget->getClientSide(), m_chatsWidget);
            }
        }
    }
    if (m_chatsWidget == nullptr) {
        m_chatsWidget = new ChatsWidget(this, m_client, m_theme);
        m_chatsWidget->setTheme(m_theme);
    }

    setCentralWidget(m_chatsWidget);
    m_client->setChatsWidget(m_chatsWidget);
}

std::string wideStringToString(const WCHAR* wideStr) {
    // Получаем необходимый размер для преобразования
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    // Преобразуем широкую строку в стандартную строку
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, &str[0], size_needed, nullptr, nullptr);
    return str;
}

QString getSaveDir() {
    WCHAR username[256];
    DWORD username_len = sizeof(username) / sizeof(username[0]);

    if (!GetUserNameW(username, &username_len)) {
        std::cout << "No User data" << std::endl;
        return QString(); // Возвращаем пустую строку или обработайте ошибку иначе
    }

    std::string usernameStr = wideStringToString(username);
    usernameStr.erase(std::remove(usernameStr.begin(), usernameStr.end(), '\0'), usernameStr.end());

    std::string saveDirectory = "C:\\Users\\" + usernameStr + "\\Documents\\Data_Air_Gram";

    
    if (!std::filesystem::exists(saveDirectory)) {
        std::filesystem::create_directories(saveDirectory); // C++17
    }

    return QString::fromStdString(saveDirectory);
}