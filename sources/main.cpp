#define ASIO_STANDALONE

#include <QApplication>
#include <QProxyStyle>
#include <QStyleOptionTitleBar>
#include <QPainter>
#include <QMainWindow>
#include <QStyleFactory>
#include <QTimer>
#include <iostream>

#include "client.h"
#include "configManager.h"
#include "mainWindow.h"
#include "utility.h"
#include "theme.h"



void tryAutoLoginOrShowLoginForm(MainWindow* mainWindow, Client* client, std::shared_ptr<ConfigManager> configManager) {
    if (configManager->checkIsAutoLogin()) {
        configManager->loadLoginHash();
        configManager->loadTheme();
        configManager->loadPasswordHash();

        mainWindow->setTheme(configManager->getIsDarkTheme() ? DARK : LIGHT);
        client->authorizeClient(configManager->getMyLoginHash(), configManager->getMyPasswordHash());
    }
    else {
        mainWindow->setupLoginWidget();
    }
}

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    
    MainWindow* mainWindow = new MainWindow(nullptr);
    if (utility::isApplicationAlreadyRunning()) {
        QTimer::singleShot(0, [&mainWindow]() {
            mainWindow->showMaximized();
            mainWindow->showAlreadyRunningDialog();
        });

        app.exec();
    }

    Client* client = new Client;

    std::shared_ptr<ConfigManager> configManager = client->getConfigManager();
    mainWindow->setConfigManager(configManager);
    mainWindow->setClient(client);
    mainWindow->setWorkerUIonClient();

    client->connectTo("192.168.1.45", 8080);
    client->startProcessingIncomingPackets();
    bool isConnected = client->waitForConnectionWithTimeout(1500);

    if (isConnected) {
        tryAutoLoginOrShowLoginForm(mainWindow, client, configManager);
    }
    
    mainWindow->showMaximized();
    app.exec();
}