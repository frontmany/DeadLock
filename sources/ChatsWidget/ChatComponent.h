#pragma once

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QLabel>
#include <QVBoxLayout>
#include <QPainterPath>

class ChatComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatComponent(QWidget* parent);

    void setName(const QString& name);
    void setMessage(const QString& message);
    void setAvatar(const QPixmap& avatar);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QHBoxLayout* m_mainHLayout;
    QVBoxLayout* m_contentsVLayout;
    QLabel*      m_lastMessageLabel;
    QLabel*      m_nameLabel;
    QPixmap      m_avatar;
    int          m_avatarSize;
};