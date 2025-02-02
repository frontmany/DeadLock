#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "request.h"


class ButtonIcon;
enum Theme;

struct StyleChatHeaderComponent {


    QString purpleLabelStyle = R"(
    QLabel {
        font-family: "Segoe UI";
        background-color: transparent;  
        font-weight: normal;
        border: none;   
        font-size: 14px;
        color: rgb(85, 90, 250);            
    }
)";

    QString blueLabelStyle = R"(
    QLabel {
        font-family: "Segoe UI";
        background-color: transparent;  
        font-weight: normal;
        border: none;   
        font-size: 14px;
        color: rgb(104, 163, 252);            
    }
)";

    QString lightLabelStyle = R"(
    QLabel {
        font-family: "Segoe UI";
        background-color: transparent;  
        font-weight: bold;
        border: none;   
        font-size: 14px;
        color: rgb(219, 219, 219);            
    }
)";

    QString darkLabelStyle = R"(
    QLabel {
        font-family: "Segoe UI";
        background-color: transparent;  
        font-weight: bold;
        border: none;   
        font-size: 14px;
        color: rgb(47, 47, 48);            
    }
)";

    QString grayLabelStyle = R"(
    QLabel {
        font-family: "Segoe UI";
        background-color: transparent;  
        font-weight: normal;
        border: none;   
        font-size: 14px;
        color: rgb(120, 120, 120);            
    }
)";

};

class ChatHeaderComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatHeaderComponent(QWidget* parent, Theme theme, QString name, QString lastSeen, QPixmap avatar);
  
    const QString& getLastSeen() const { return m_lastSeenLabel->text(); }

public slots:
    void setLastSeen(const QString& lastSeen);

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    void setTheme(Theme theme);


private:
    StyleChatHeaderComponent* style;
    Theme m_theme;
    QColor m_backColor;
    
    ButtonIcon*     m_leftIcon;
    ButtonIcon*     m_rightButton;

    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_rightLayout;

    QLabel*     m_nameLabel;
    QLabel*     m_lastSeenLabel;
};