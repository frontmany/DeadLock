#include <QApplication>
#include <QProxyStyle>
#include <QStyleOptionTitleBar>
#include <QPainter>
#include <QMainWindow>
#include <QStyleFactory>

#include "mainWindow.h"
#include <iostream>

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

    MainWindow* mainWindow = new MainWindow(nullptr);
    


    mainWindow->show();
    mainWindow->resize(960, 540);

    

    app.exec();
    delete mainWindow;
}