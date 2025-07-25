#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QDebug>
#include <QPushButton>
#include <QTimer>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
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
#include <limits>

#include "chat.h"


struct StyleMessagingAreaComponent {
    StyleMessagingAreaComponent();
    QString darkSlider;
    QString lightSlider;
    QString DarkTextEditStyle;
    QString LightTextEditStyle;
    QString LightErrorLabelStyle;
    QString DarkErrorLabelStyle;
    QString buttonTransparentDark;
    QString buttonTransparentLight;
    QString DarkFileDialogButton;
    QString LightFileDialogButton;
    QString buttonTransparentFileDialogLight;
    QString buttonTransparentFileDialogDark;
    QString buttonTransparentFileDialogDarkAdd;
    QString buttonTransparentFileDialogLightAdd;
};

class ButtonIcon;
class ButtonCursor;
class ChatHeaderComponent;
class MessagingAreaComponent;
class DelimiterComponent;
class OverlayWidget;
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

    bool m_lastKeyIsBackspaceOrEnter = false;

protected:
    void keyPressEvent(QKeyEvent* event) override {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            if (event->modifiers() & Qt::ShiftModifier) {
                QTextEdit::keyPressEvent(event);
                m_lastKeyIsBackspaceOrEnter = false;
            }
            else {
                m_lastKeyIsBackspaceOrEnter = true;
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
            m_lastKeyIsBackspaceOrEnter = false;
            QTextEdit::paste();
            emit textLengthChanged(document()->characterCount());
            return;
        }

        if (event->key() == Qt::Key_Backspace ||
            event->key() == Qt::Key_Delete ||
            event->modifiers() & Qt::ControlModifier) 
        {
            m_lastKeyIsBackspaceOrEnter = true;
            QTextEdit::keyPressEvent(event);
            emit textLengthChanged(document()->characterCount());
            return;
        }

        if (document()->characterCount() < 8192) {
            m_lastKeyIsBackspaceOrEnter = false;
            QTextEdit::keyPressEvent(event);
            emit textLengthChanged(document()->characterCount());
        }
    }

signals:
    void enterPressed();
    void pasteExceeded(const QString& errorText);
    void textLengthChanged(int length);
};


class FriendProfileComponent : public QWidget
{
    Q_OBJECT
public:
    explicit FriendProfileComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Theme theme);
    void setUserData(const QString& login, const QString& name);
    void setTheme(Theme theme);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    void setupUI();
    void applyGlassEffect(QPainter& painter, const QPainterPath& path);

private:
    QLabel* m_login_label;
    QLabel* m_name_label;

    //ButtonIcon m_block_button; TODO
    ButtonIcon* m_close_button;
    MessagingAreaComponent* m_messagingAreaComponent;

    QVBoxLayout* m_mainLayout;
    QColor* m_color;

    Theme m_theme;
};



class ChatPropertiesComponent : public QWidget
{
    Q_OBJECT
public:
    explicit ChatPropertiesComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Theme theme);
    void setTheme(Theme theme);
    void disable(bool isDisabled);

protected:
    void paintEvent(QPaintEvent* event) override;

signals:
    void deleteRequested();

private:
    void setupUI();
    void applyGlassEffect(QPainter& painter, const QPainterPath& path);

private:
    StyleMessagingAreaComponent* m_style;
    MessagingAreaComponent* m_messagingAreaComponent;

    ButtonIcon* m_close_button;
    QPushButton* m_delete_chat_button;
    QVBoxLayout* m_mainLayout;

    QColor* m_color;
    Theme m_theme;
};


class MessagingAreaComponent : public QWidget {
    Q_OBJECT

public:
    MessagingAreaComponent(QWidget* parent, QString friendName, Theme theme, Chat* chat, ChatsWidget* chatsWidget);
    ~MessagingAreaComponent();

    bool isMessageVisible(MessageComponent* msgComp) const;
    std::vector<MessageComponent*>& getUreadMessageComponents();

    void setTheme(Theme theme);
    QScrollArea* getScrollArea() { return m_scrollArea; }

    ChatHeaderComponent* getChatHeader() { return m_header; }
    ButtonIcon* getAttachFileButton() { return m_attachFileButton; }
    std::vector<MessageComponent*>& getMessagesComponentsVec() { return m_vec_messagesComponents; }
    const Chat* getChatConst() const { return m_chat; }
    Chat* getChat() { return m_chat; }
    ChatsWidget* getChatsWidget() { return m_chatsWidget; }
    DelimiterComponent* getDelimiterComponentUnread() { return m_delimiter_component_unread; }
    MyTextEdit* getTextEdit() { return m_messageInputEdit; }
    ChatPropertiesComponent* getChatPropertiesComponent() { return m_chat_properties_component; }

    void removeDelimiterComponentUnread();
    void moveDelimiterComponentUnreadDown();
    bool isDelimiterComponentUnread() const { return isDelimiterUnread; }

    void markVisibleMessagesAsChecked();
    void hideSendMessageButton();

signals:
    void sendMessageData(Message*, Chat* chat);
    void sendFilesData(Message*, Chat* chat, size_t filesCount);

public slots:
    void addDelimiterComponentIncomingFilesLoading();
    

    void onRetryClicked(Message* messageToRetry);
    void openFriendProfile();
    void closeFriendProfile();

    void openChatPropertiesDialog();
    void closeChatPropertiesDialog();

    void addMessage(Message* message, bool isRecoveringMessages);
    void setAvatar(const QPixmap& pixMap);
    void setName(const QString& name);
    void setErrorLabelText(const QString& errorText);
    void moveSliderDown(bool isCalledFromWorker = false);
    void onChatDelete();

private slots:
    void adjustTextEditHeight();
    void onSendMessageClicked();
    void onAttachFileClicked();
    void onTypeMessage();
    void onSendFiles();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void removeDelimiterComponentIncomingFilesLoading();
    void updateRelatedChatComponentLastMessage();
    void handleScroll(int value);
    void updateSliderButtonPosition();
    void onTypingTimeout();
    void addFileToDisplayList(QDialog* filesDialog, QVBoxLayout* filesLayout, const QStringList& newFiles);
   

private:
    StyleMessagingAreaComponent*    m_style;
    Theme                           m_theme;
    QColor                          m_backColor;
    
    std::vector<MessageComponent*> m_vec_unread_messagesComponents;
    std::vector<MessageComponent*> m_vec_messagesComponents;

    QVBoxLayout* m_sendMessage_VLayout;
    QVBoxLayout* m_attachFile_VLayout;
    QVBoxLayout* m_main_VLayout;
    QVBoxLayout* m_containerVLayout;
    QHBoxLayout* m_button_sendHLayout;

    DelimiterComponent* m_delimiter_component_unread = nullptr;
    bool isDelimiterUnread = false;

    DelimiterComponent* m_delimiter_component_is_incoming_files = nullptr;
    bool isDelimiterIncoming = false;


    QDialog* m_friendProfileDialog = nullptr;
    OverlayWidget* m_friendProfileOverlay = nullptr;
    QDialog* m_chatPropertiesDialog = nullptr;
    OverlayWidget* m_chatPropertiesOverlay = nullptr;
    FriendProfileComponent* m_friend_profile_component;
    ChatPropertiesComponent* m_chat_properties_component;

    QTimer* m_typingTimer;
    bool m_isTypingActive = false;

    QString                 m_friendName;
    ChatHeaderComponent*    m_header;
    QScrollArea*            m_scrollArea;  
    QWidget*                m_containerWidget;
    MyTextEdit*             m_messageInputEdit;
    ButtonCursor*           m_sendMessageButton;
    ButtonIcon*             m_attachFileButton;
    ButtonIcon*              m_move_slider_down_button;
    MyTextEdit*             m_files_caption_edit = nullptr;
    QStringList             m_vec_selected_files;
    QLabel*                 m_error_label;
    QHBoxLayout*            m_error_labelLayout;
    Chat*                   m_chat;
    ChatsWidget*            m_chatsWidget;
};