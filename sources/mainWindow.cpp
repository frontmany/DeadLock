#include "mainWindow.h"


MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupLoginWidget();
}


MainWindow::~MainWindow() {
    delete loginWidget;
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


void MainWindow::setChatMenu() {

}


void MainWindow::setAuthorizationWidget() {

}


void MainWindow::setRegistrationWidget() {

}


void MainWindow::setupLoginWidget() {
    loginWidget = new LoginWidget(this);
    loginWidget->setTheme(LIGHT);
    setCentralWidget(loginWidget);
}