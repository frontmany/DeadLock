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

class ChatComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatComponent(QWidget* parent, ChatsWidget* chatsWidget, Chat* chat);
    explicit ChatComponent(QWidget* parent, ChatsWidget* chatsWidget);

    void setName(const QString& name);
    void setMessage(const QString& message);
    void setAvatar(const QPixmap& avatar);
    void setTheme(Theme theme);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    bool event(QEvent* event) override;
    void hoverEnter(QHoverEvent* event);
    void hoverLeave(QHoverEvent* event);

private slots:
    void slotToSendChatData();

signals:
    void clicked();
    void sendChatData(Chat* chat);


private:
    Chat*        m_chat;

    Theme        m_theme;
    QColor       m_backColor;
    QHBoxLayout* m_mainHLayout;
    QVBoxLayout* m_contentsVLayout;
    QLabel*      m_lastMessageLabel;
    QLabel*      m_nameLabel;
    QPixmap      m_avatar;

    QColor       m_hoverColorDark; 
    QColor       m_hoverColorLight; 
    QColor       m_currentColor; 

    bool         m_isClicked;
    int          m_avatarSize;
};