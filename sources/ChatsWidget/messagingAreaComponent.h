#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QCoreApplication>
#include <QPainter>
#include <string>
#include <QKeyEvent>
#include <vector>
#include <QScrollBar>
#include <random>

#include "chat.h"


struct StyleMessagingAreaComponent {
    StyleMessagingAreaComponent();
    QString darkSlider;
    QString lightSlider;
    QString DarkTextEditStyle;
    QString LightTextEditStyle;
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
    ~MessagingAreaComponent();

    void setTheme(Theme theme);
    QScrollArea* getScrollArea() { return m_scrollArea; }

    ChatHeaderComponent* getChatHeader() { return m_header; }
    std::vector<MessageComponent*>& getMessagesComponentsVec() { return m_vec_messagesComponents; }
    const Chat* getChatConst() const { return m_chat; }
    Chat* getChat() { return m_chat; }


signals:
    void sendMessageData(Message*, Chat* chat);

public slots:
    void addMessage(Message* message);
    void setAvatar(const QPixmap& pixMap);
    void setName(const QString& name);
    void markMessageAsChecked(Message* message);
    void moveSliderDown(bool isCalledFromWorker = false);

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
    

    QString                 m_friendName;
    MyTextEdit*             m_messageInputEdit;
    ChatHeaderComponent*    m_header;
    QScrollArea*            m_scrollArea;  
    QWidget*                m_containerWidget;
    ButtonCursor*           m_sendMessageButton;

    Chat*                   m_chat;
    ChatsWidget*            m_chatsWidget;
};