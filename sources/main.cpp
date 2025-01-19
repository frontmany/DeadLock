#include <QApplication>
#include <QProxyStyle>
#include <QStyleOptionTitleBar>
#include <QPainter>
#include <QMainWindow>
#include <QStyleFactory>

#include "mainWindow.h"
#include "clientSide.h"

class CustomStyle : public QProxyStyle {
public:
    CustomStyle(QStyle* style = nullptr) : QProxyStyle(style) {}
};

int main(int argc, char* argv[])
{
    ClientSide* client = new ClientSide;
    //client->init();
    //client->connectTo("192.168.1.49", 54000);

    QApplication app(argc, argv);
    CustomStyle* customStyle = new CustomStyle(QStyleFactory::create("Windows"));
    app.setStyle(customStyle);

    MainWindow* mainWindow = new MainWindow(nullptr, client);
    
    mainWindow->show();
    mainWindow->resize(960, 540);


    return app.exec();
}