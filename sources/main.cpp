#include <QApplication>
#include <QProxyStyle>
#include <QStyleOptionTitleBar>
#include <QPainter>
#include <QMainWindow>
#include <QStyleFactory>
#include <iostream>


#include "client.h"
#include "mainWindow.h"


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
    client->connectTo("192.168.56.1", 8080);
    client->run();

    MainWindow* mainWindow = new MainWindow(nullptr, client);
    mainWindow->setupLoginWidget();
    
    mainWindow->showMaximized();

    app.exec();
    delete mainWindow;
}