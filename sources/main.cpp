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


class CustomStyle : public QProxyStyle {
public:
    CustomStyle(QStyle* style = nullptr) : QProxyStyle(style) {}
};

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru"); 
  
    QApplication app(argc, argv);
    CustomStyle* customStyle = new CustomStyle(QStyleFactory::create("Fusion"));
    app.setStyle(customStyle);
    
    std::shared_ptr<ConfigManager> configManager = std::make_shared<ConfigManager>();
    Client* client = new Client(configManager);

    configManager->setClient(client);

    client->connectTo("92.255.165.77", 8080);
    client->run();
    while (!client->isConnected()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    MainWindow* mainWindow = new MainWindow(nullptr, client, configManager);
    if (utility::isApplicationAlreadyRunning()) {
        QTimer::singleShot(0, [&mainWindow]() {
            mainWindow->showAlreadyRunningDialog();
        });
    }

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
    
    mainWindow->showMaximized();

    app.exec();
    delete mainWindow;
}