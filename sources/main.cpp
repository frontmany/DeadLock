#include <QApplication>
#include <QProxyStyle>
#include <QStyleOptionTitleBar>
#include <QPainter>
#include <QMainWindow>
#include <QStyleFactory>

#include "mainWindow.h"

class CustomStyle : public QProxyStyle {
public:
    CustomStyle(QStyle* style = nullptr) : QProxyStyle(style) {}
};

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    CustomStyle* customStyle = new CustomStyle(QStyleFactory::create("Windows"));
    app.setStyle(customStyle);

    MainWindow* mainWindow = new MainWindow();
    
    mainWindow->show();
    mainWindow->resize(960, 540);


    return app.exec();
}