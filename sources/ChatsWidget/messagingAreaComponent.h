#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QSequentialAnimationGroup>
#include <QScrollArea>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <qclipboard.h>
#include <QPainter>
#include <QMimeData>
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

    QString LightErrorLabelStyle;
    QString DarkErrorLabelStyle;

};

class ButtonIcon;
class ButtonCursor;
class ChatHeaderComponent;
class MessageComponent;
class ChatsWidget;
class Packet;
enum Theme;

class MyTextEdit : public QTextEdit {
    Q_OBJECT
public:
    explicit MyTextEdit(QWidget* parent = nullptr) : QTextEdit(parent) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }

    void setLimitedPlainText(const QString& text) {
        if (text.length() <= 8192) {
            QTextEdit::setPlainText(text);
            emit textLengthChanged(document()->characterCount());
        }
        else {
            emit pasteExceeded("Maximum length exceeded (8192 characters limit).");
        }
    }

protected:
    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            if (event->modifiers() & Qt::ShiftModifier) {
                QTextEdit::keyPressEvent(event);
            }
            else {
                emit enterPressed();
                event->accept();
            }
            return;
        }

        if (event->key() == Qt::Key_V && event->modifiers() & Qt::ControlModifier) {
            QTextCursor cursor = textCursor();
            int selectedChars = cursor.selectedText().length();
            int currentLength = document()->characterCount();
            QString clipboardText = QGuiApplication::clipboard()->text();

            if (currentLength - selectedChars + clipboardText.length() > 8192) {
                emit pasteExceeded("Maximum length exceeded (8192 characters limit).");
                return;
            }
            QTextEdit::paste();
            emit textLengthChanged(document()->characterCount());
            return;
        }

        if (event->key() == Qt::Key_Backspace ||
            event->key() == Qt::Key_Delete ||
            event->modifiers() & Qt::ControlModifier) {
            QTextEdit::keyPressEvent(event);
            emit textLengthChanged(document()->characterCount());
            return;
        }

        if (document()->characterCount() < 8192) {
            QTextEdit::keyPressEvent(event);
            emit textLengthChanged(document()->characterCount());
        }
    }

signals:
    void enterPressed();
    void pasteExceeded(const QString& errorText);
    void textLengthChanged(int length);
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
    void setErrorLabelText(const QString& errorText);
    void markMessageAsChecked(Message* message);
    void moveSliderDown(bool isCalledFromWorker = false);

private slots:
    void adjustTextEditHeight();
    void onSendMessageClicked();
    void onTypeMessage();

protected:
    void paintEvent(QPaintEvent* event) override;
 

private:
    void updateRelatedChatComponentLastMessage();

private:
    StyleMessagingAreaComponent*    m_style;
    Theme                           m_theme;
    QColor                          m_backColor;
    
    std::vector<MessageComponent*> m_vec_messagesComponents;

    QVBoxLayout* m_sendMessage_VLayout;
    QVBoxLayout* m_main_VLayout;
    QVBoxLayout* m_containerVLayout;
    QHBoxLayout* m_button_sendHLayout;

    QString                 m_friendName;
    MyTextEdit*             m_messageInputEdit;
    ChatHeaderComponent*    m_header;
    QScrollArea*            m_scrollArea;  
    QWidget*                m_containerWidget;
    ButtonCursor*           m_sendMessageButton;

    QLabel*                 m_error_label;
    QHBoxLayout*            m_error_labelLayout;

    Chat*                   m_chat;
    ChatsWidget*            m_chatsWidget;
};