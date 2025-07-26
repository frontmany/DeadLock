#pragma once
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QGuiApplication>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QParallelAnimationGroup>
#include <QScreen>
#include <QString>
#include <QPushButton>
#include <QPainterPath>
#include <QTimer>
#include <QPropertyAnimation>

#include "theme.h"

class Photo;
class AvatarIcon;
class ChatsWidget;
class ButtonIcon;
class Chat;

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationWidget(ChatsWidget* chatsWidget, Chat* chat, Theme theme,
        QWidget* parent = nullptr);

    void setTheme(Theme theme);

signals:
    void clicked();
    void redirectToChat(Chat* chat);

protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void closeNotification(bool isShouldWithAnimation);

private:
    void loadAvatar(const Photo* photo);
    void setHoveredStyle(bool isHovered);

private:
    QTimer* m_closeTimer = nullptr;
    Chat* m_chat;
    Theme m_theme;
    AvatarIcon* m_avatar;
    QLabel* m_senderLabel; 
    QLabel* m_messageLabel;
    QPushButton* m_hiddenModeButton;
    ChatsWidget* m_chats_widget;
    ButtonIcon* m_closeButton;
    bool m_isCloseClicked = false;
};