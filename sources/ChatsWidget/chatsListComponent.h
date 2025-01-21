#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QHoverEvent>
#include <QMouseEvent>
#include <QEvent>
#include "ChatComponent.h"



struct StyleChatsListComponent {
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

    QString DarkLineEditStyleRed = R"(
    QLineEdit {
        background-color: #333;    
        color: white;               
        border: 2px solid rgb(232, 44, 57);      
        border-radius: 13px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid rgb(232, 44, 57);       
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
        border: 2px solid #888;        
    }
)";

    QString addButtonStyle = R"(
    QPushButton {
        background-color: transparent;   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";

    QString DarkButtonStyleBlue = R"(
    QPushButton {
        background-color: rgb(21, 119, 232);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        font-family: "Segoe UI";  
        font-size: 14px;          
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";

    QString DarkButtonStyleRed = R"(
    QPushButton {
        background-color: rgb(232, 44, 44);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";

};

class RoundIconButton;
class ButtonIcon;
class ChatsWidget;
enum Theme;

class ChatsListComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget);
    ~ChatsListComponent();
    void setTheme(Theme theme);

    void addChatComponent(const QString& name, const QString& message, const QPixmap& avatar);
    void setRedBorderToChatAddDialog();
    void setAbleToCreateChatFlag(bool fl) { m_ableToCreateChat = fl; }

signals:
    void sendCreateChatData(QString login);

private slots:
    void slotToSendCreateChatData();
    void showChatAddDialog();
    void onTextChanged(const QString& text);


private:
    QColor          m_backgroundColor;
    StyleChatsListComponent* style;
    Theme           m_theme;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_profileHLayout;
    QHBoxLayout* m_contentsHLayout;


    ChatsWidget*        m_chatsWidget;
    QScrollArea*        m_scrollArea;
    QWidget*            m_containerWidget;
    QVBoxLayout*        m_containerVLayout;
    QLineEdit*          m_searchLineEdit;
    RoundIconButton*    m_profileButton;
    ButtonIcon*         m_newChatButton;


    QLineEdit*      m_friendLoginEdit;
    QPushButton*    m_createChatButton;
    ButtonIcon*     m_cancelButton;
    QWidget*        m_chatAddDialog;
    QVBoxLayout*    m_dialogLayout;
    QHBoxLayout*    m_buttonsLayout;
    QVBoxLayout*    m_aVLayout;

    bool m_isChatAddDialog = false;
    bool m_ableToCreateChat = true;
};