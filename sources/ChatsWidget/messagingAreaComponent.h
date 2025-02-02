#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QDebug>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainter>
#include <string>
#include <QKeyEvent>
#include <vector>
#include <QScrollBar>
#include <random>

#include "request.h"
#include "chat.h"



std::string getCurrentTime();

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
    
    QString DarkTextEditStyle = R"(
    QTextEdit {
        background-color: rgb(36, 36, 36);    
        color: white;               
        border: none;     
        border-radius: 15px;         
        padding: 5px;               
    }
    QTextEdit:focus {
        border: 2px solid #888;     
    }
)";

QString LightTextEditStyle = R"(
    QTextEdit {
        background-color: #ffffff;    
        color: black;                 
        border: none;       
        border-radius: 15px;           
        padding: 5px;                 
    }
    QTextEdit:focus {
        border: 2px solid rgb(237, 237, 237);        
    }
)";
};

class ButtonIcon;
class ButtonCursor;
class ChatHeaderComponent;
class MessageComponent;
class ChatsWidget;
class Packet;
enum Theme;

class MyTextEdit : public QTextEdit
{
    Q_OBJECT
public:
    MyTextEdit(QWidget* parent) : QTextEdit(parent), m_max_length(100) {}
    void setMaxLength(int max_length) {
        m_max_length = max_length;
    }

protected:
    void keyPressEvent(QKeyEvent* event) override
    {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            emit enterPressed();
            return;
        }
        QTextEdit::keyPressEvent(event);
    }
    
private:
    int m_max_length = 0;
signals:
    void enterPressed();
};

class MessagingAreaComponent : public QWidget {
    Q_OBJECT

public:
    MessagingAreaComponent(QWidget* parent, QString friendName, Theme theme, Chat* chat, ChatsWidget* chatsWidget);
    MessagingAreaComponent(Theme theme);
    void setTheme(Theme theme);
    

    ChatHeaderComponent* getChatHeader() { return m_header; }
    std::vector<MessageComponent*>& getMessagesComponentsVec() { return m_vec_messagesComponents; }
    const Chat* getChatConst() const { return m_chat; }
    Chat* getChat() { return m_chat; }

    QJsonObject serialize() const;
    static MessagingAreaComponent* deserialize(const QJsonObject& jsonObject, QWidget* parent, ChatsWidget* chatsWidget);


signals:
    void sendMessageData(const QString& message, const QString& timeStamp, Chat* chat, double id);

public slots:
    void addMessageReceived(QString msg, QString timestamp, double id);
    void addMessageSent(QString msg, QString timestamp, double id);
    void addComponentToNotCurrentMessagingArea(Chat* foundChat, Msg* msg);

private slots:
    void adjustTextEditHeight();
    void onSendMessageClicked();
    void onTypeMessage();

protected:
    void paintEvent(QPaintEvent* event) override;


private:
    StyleMessagingAreaComponent*    style;
    Theme                           m_theme;
    QColor                          m_backColor;
    
    std::vector<MessageComponent*> m_vec_messagesComponents;

    QVBoxLayout* m_sendMessage_VLayout;
    QVBoxLayout* m_main_VLayout;
    QVBoxLayout* m_containerVLayout;
    ChatsWidget* m_chatsWidget;

    QString                 m_friendName;
    MyTextEdit*             m_messageInputEdit;
    ChatHeaderComponent*    m_header;
    QScrollArea*            m_scrollArea;  
    QWidget*                m_containerWidget;
    ButtonCursor*           m_sendMessageButton;

    Chat*                   m_chat;
};