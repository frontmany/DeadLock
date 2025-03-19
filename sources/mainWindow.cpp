#include <filesystem>
#include "mainWindow.h"
#include "utility.h"
#include "client.h"
#include "chatsListComponent.h"
#include "workerQt.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    if (isDarkMode()) {
        m_theme = DARK;
    }
    else {
        m_theme = LIGHT;
    }
     
    m_client = new Client;
    m_client->connectTo("192.168.1.49", 8080);
    m_client->run();
    m_client->setWorkerUI(m_worker);
    
    m_greetWidget = new GreetWidget(this, this, m_client, m_theme);
    m_greetWidget->hide();

    m_chatsWidget = new ChatsWidget(this, m_client, m_theme);
    m_chatsWidget->hide();

    m_worker = new WorkerQt(m_chatsWidget, m_client);
    m_client->setWorkerUI(m_worker);

    setupLoginWidget();
}

MainWindow::~MainWindow() {
    qDebug() << "window closing";
    if (m_client->isAuthorized() == true) {
        m_client->sendMyStatus(Utility::getCurrentDateTime());
        std::cout << "saving\n";
        m_client->save();
    }
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

void MainWindow::onLogin(bool isRegistration) {
    if (isRegistration) {
        setupGreetWidget();
    }
    else {
        setupChatsWidget();
    }
    
}

void MainWindow::setupGreetWidget() {
    m_greetWidget->show();
    m_greetWidget->startWelcomeAnimation();
    setCentralWidget(m_greetWidget);
}


void MainWindow::setupLoginWidget() {
    m_loginWidget = new LoginWidget(this, this, m_client);
    m_loginWidget->setTheme(m_theme);
    setCentralWidget(m_loginWidget);
}

void MainWindow::setupChatsWidget() {
    m_chatsWidget->show();
    m_chatsWidget->setTheme(m_theme);
    m_chatsWidget->setup();
    m_chatsWidget->setupChatComponents();
    setCentralWidget(m_chatsWidget);
    m_client->chatsWidgetState = true;
}