#pragma once

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainterPath>
#include <QHoverEvent>
#include <QMouseEvent>
#include <QEvent>

#include <chat.h>



enum Theme;
class ChatsWidget;
class ButtonIcon;
class AvatarIcon;

class ChatComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatComponent(QWidget* parent, ChatsWidget* chatsWidget, Chat* chat);

    void setTheme(Theme theme);
    void setSelected(bool isSelected);
    const Chat* getChatConst() const { return m_chat; }
    Chat* getChat() { return m_chat; }

   
   

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool event(QEvent* event) override;
    void hoverEnter(QHoverEvent* event);
    void hoverLeave(QHoverEvent* event);


private slots:
    void slotToSendChatData();


public slots:
    void setName(const QString& name);
    void setAvatar(const QPixmap& avatar);
    void setUnreadMessageDot(bool isUnreadMessages);
    void setLastMessage(const QString& message);

signals:
    void clicked();
    void sendChatData(Chat* chat, ChatComponent* component);


private:
    Chat*        m_chat;

    Theme        m_theme;
    QColor       m_backColor;

    QHBoxLayout* m_mainHLayout;
    QVBoxLayout* m_contentsVLayout;
    QVBoxLayout* m_statusVLayout;

    ButtonIcon*  m_UnreadDot;
    AvatarIcon*   m_avatar_ico;
    QLabel*      m_lastMessageLabel;
    QLabel*      m_nameLabel;
    QPixmap      m_avatar;

    QColor       m_hoverColorDark; 
    QColor       m_hoverColorLight; 
    QColor       m_currentColor; 

    bool         m_isClicked;
    bool         m_isSelected;
    int          m_avatarSize;

};