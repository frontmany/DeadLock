#pragma once

#include <QWidget>
#include <vector>
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
#include "mainwindow.h"



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
        border-radius: 15px;         
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
        border-radius: 15px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: 2px solid rgb(237, 237, 237);        
    }
)";

    QString transparentButtonStyle = R"(
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

};

class AvatarIcon;
class AddChatDialogComponent;
class MessagingAreaComponent;
class ButtonIcon;
class ToggleSwitch;
class Client;
class ChatsWidget;

class ChatsListComponent : public QWidget {
    Q_OBJECT

public:
    ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme);
    ~ChatsListComponent();
    void setTheme(Theme theme);
    void setAbleToCreateChatFlag(bool fl) { m_ableToCreateChat = fl; }
    void SetAvatar(const Photo& photo);

    std::vector<ChatComponent*>& getChatComponentsVec() { return m_vec_chatComponents; }
    AddChatDialogComponent* getAddChatDialogComponent() { return m_chatAddDialog; }
    QLineEdit* getSearchLineEdit() { return m_searchLineEdit; }

signals:
    void sendCreateChatData(QString login);
    void sendChangeTheme();


public slots:
    void addChatComponent(Theme theme, Chat* chat, bool isSelected);
    void openAddChatDialog();
    void closeAddChatDialog();
    void receiveCreateChatData(QString login);
    void popUpComponent(ChatComponent* comp);
    void loadAvatarFromPC(const std::string& login);

private slots:
    void toSendChangeTheme(bool fl);

private:
    QColor                      m_backgroundColor;
    StyleChatsListComponent*    style;
    Theme                       m_theme;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_profileHLayout;
    QHBoxLayout* m_contentsHLayout;

    ButtonIcon*         m_moon_icon;
    ToggleSwitch*       m_darkModeSwitch;
    ChatsWidget*        m_chatsWidget;
    QScrollArea*        m_scrollArea;
    QWidget*            m_containerWidget;
    QVBoxLayout*        m_containerVLayout;
    QLineEdit*          m_searchLineEdit;
    AvatarIcon*         m_profileButton;
    ButtonIcon*         m_newChatButton;
    AddChatDialogComponent* m_chatAddDialog;
    ChatsWidget*        m_chats_widget;

    std::vector<ChatComponent*> m_vec_chatComponents;
    bool m_isChatAddDialog = false;
    bool m_ableToCreateChat = true;
};