#include "mainWindow.h"
#include "clientSide.h"
//#include<windows.h>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {\
    m_client = new ClientSide(m_chatsWidget);
    m_client->init();
    m_client->connectTo("192.168.1.49", 54000);

    setupLoginWidget();
    //setupChatsWidget();

}


MainWindow::~MainWindow() {
    delete m_loginWidget;
}

/*
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
*/

void MainWindow::onLogin(bool isLoggedIn) {
    if (isLoggedIn) {
        setupChatsWidget();
    }
}


void MainWindow::setupLoginWidget() {
    m_loginWidget = new LoginWidget(this, this, m_client);
    m_loginWidget->setTheme(DARK);
    setCentralWidget(m_loginWidget);
}


void MainWindow::setupChatsWidget() {
    m_chatsWidget = new ChatsWidget(this, m_client, DARK);
    m_chatsWidget->setTheme(DARK);
    setCentralWidget(m_chatsWidget);
    m_client->setChatsWidget(m_chatsWidget);
}