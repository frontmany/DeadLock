#pragma once
#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QGuiApplication>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QScreen>
#include <QString>
#include <QPushButton>
#include <QPainterPath>
#include <QTimer>

#include "theme.h"

class Photo;
class AvatarIcon;
class ChatsWidget;

class NotificationWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NotificationWidget(ChatsWidget* chatsWidget,
        const QString& message,
        const Photo* photo,
        QWidget* parent = nullptr);

    void setTheme(Theme theme);

protected:
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

private slots:
    void closeNotification();

private:
    void loadAvatar(const Photo* photo);

    Theme m_theme;
    AvatarIcon* m_avatar;
    QLabel* m_messageLabel;
    ChatsWidget* m_chats_widget;
    QPushButton* m_closeButton;
};