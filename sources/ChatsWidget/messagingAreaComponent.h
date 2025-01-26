#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainter>
#include <QScrollBar>

#include "request.h"
#include "chat.h"

struct StyleMessagingAreaComponent {
    QString darkSlider = R"(
    QScrollBar:vertical {
        border: 2px solid rgb(36, 36, 36);      
        background: rgb(36, 36, 36);        
        width: 10px;                 
        border-radius: 5px; 
    }

    QScrollBar::handle:vertical {
        background: rgb(56, 56, 56);   
        border: 2px solid rgb(56, 56, 56);      
        width: 10px;    
        border-radius: 5px;           
    }

    QScrollBar::add-line:vertical, 
    QScrollBar::sub-line:vertical { 
        background: none;             
    }
)";

    QString lightSlider = R"(
    QScrollBar:vertical {
        border: 2px solid rgb(250, 250, 250);      
        background: rgb(250, 250, 250);        
        width: 10px;                 
        border-radius: 5px; 
    }

    QScrollBar::handle:vertical {
        background: rgb(218, 219, 227);   
        border: 2px solid rgb(218, 219, 227);      
        width: 10px;    
        border-radius: 5px;           
    }

    QScrollBar::add-line:vertical, 
    QScrollBar::sub-line:vertical { 
        background: none;             
    }
)";

    QString DarkLineEditStyle = R"(
    QLineEdit {
        background-color: #333;    
        color: white;               
        border: none;     
        border-radius: 13px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid #888;     
    }
)";

    QString LightLineEditStyle = R"(
    QLineEdit {
        background-color: #ffffff;    
        color: black;                 
        border: none;       
        border-radius: 13px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: 2px solid rgb(237, 237, 237);        
    }
)";
};

class ButtonIcon;
class ChatHeaderComponent;
class Packet;
enum Theme;

class MessagingAreaComponent : public QWidget {
    Q_OBJECT

public:
    MessagingAreaComponent(QWidget* parent, QString friendName, Theme theme, Chat* chat);
    MessagingAreaComponent(Theme theme);
    void setTheme(Theme theme);

private slots:
    void onSendMessageClicked();
    void onTypeMessage();

protected:
    void paintEvent(QPaintEvent* event) override;


private:
    StyleMessagingAreaComponent* style;
    Theme                           m_theme;
    QColor                          m_backColor;
    

    QString                 m_friendName;
    QLineEdit*              m_messageInputEdit;
    ChatHeaderComponent*    m_header;
    QScrollArea*            m_scrollArea;  
    QWidget*                m_messagesWidget; 
    QVBoxLayout*            m_messagesLayout; 
    ButtonIcon*             m_sendMessageButton;

    Chat*                   m_chat;
};