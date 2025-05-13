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
    StyleHelloAreaComponent();
    QString LabelStyleDark;
    QString LabelStyleLight;
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