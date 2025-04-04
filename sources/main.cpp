#include <QApplication>
#include <QProxyStyle>
#include <QStyleOptionTitleBar>
#include <QPainter>
#include <QMainWindow>
#include <QStyleFactory>

#include "mainWindow.h"
#include "windows.h"
#include <iostream>

class CustomStyle : public QProxyStyle {
public:
    CustomStyle(QStyle* style = nullptr) : QProxyStyle(style) {}
};

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru");

    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
    qputenv("QT_AUTO_SCREEN_SCALE_FACTOR", "0");
    qputenv("QT_SCALE_FACTOR", "1");

    QApplication app(argc, argv);
    CustomStyle* customStyle = new CustomStyle(QStyleFactory::create("Fusion"));
    app.setStyle(customStyle);

    MainWindow* mainWindow = new MainWindow(nullptr);

    
    mainWindow->resize(960, 540);
    mainWindow->showMaximized();

    app.exec();
    delete mainWindow;
}