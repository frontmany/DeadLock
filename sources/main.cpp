#include <QApplication>
#include <QProxyStyle>
#include <QStyleOptionTitleBar>
#include <QPainter>
#include <QMainWindow>
#include <QStyleFactory>
#include <QTimer>
#include <iostream>

#include "client.h"
#include "mainWindow.h"
#include "utility.h"


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
    
    Client* client = new Client;
    bool isAutoLogin = client->autoLoginAndLoad();
    client->connectTo("192.168.56.1", 8080);
    client->run();

    MainWindow* mainWindow = new MainWindow(nullptr, client);
    if (utility::isApplicationAlreadyRunning()) {
        QTimer::singleShot(0, [&mainWindow]() {
            mainWindow->showAlreadyRunningDialog();
        });
    }


    if (isAutoLogin == true) {
        client->initDatabase(client->getMyLogin());
        client->authorizeClient(client->getMyLogin(), client->getMyPasswordHash());
    }
    else {
        mainWindow->setupLoginWidget();
    }
    
    mainWindow->showMaximized();

    app.exec();
    delete mainWindow;
}