#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QPainter>
#include <QScrollBar>

#include "chat.h"

class ButtonIcon;
enum Theme;

struct StyleHelloAreaComponent {
    QString labelStyleDark = R"(
    QLabel {
        background-color: rgba(59, 59, 59, 200); 
        color: rgb(240, 240, 240); 
        font-family: 'Segoe UI'; 
        font-size: 14px; 
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";

    QString labelStyleLight = R"(
    QLabel {
        background-color: rgba(148, 148, 148, 200); 
        color: rgb(240, 240, 240); 
        font-family: 'Segoe UI'; 
        font-size: 14px; 
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";
};

class HelloAreaComponent : public QWidget {
    Q_OBJECT

public:
    HelloAreaComponent(Theme theme);
    void setTheme(Theme theme);
    void setBackGround(Theme theme);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    StyleHelloAreaComponent* style;
    Theme                    m_theme;
    QPixmap					 m_background;
    

    QLabel*                  m_label;
    QVBoxLayout*             m_main_VLayout;
    QHBoxLayout*             m_main_HLayout;
};