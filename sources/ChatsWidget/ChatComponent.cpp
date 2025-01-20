#include "chatComponent.h"

ChatComponent::ChatComponent(QWidget* parent)
    : QWidget(parent), m_avatarSize(50) {
    setMinimumSize(100, 60);
    setMaximumSize(1000, 60);


    m_nameLabel = new QLabel(this);
    m_lastMessageLabel = new QLabel(this);


    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_lastMessageLabel->setStyleSheet("font-size: 12px; color: gray;");

    m_contentsVLayout = new QVBoxLayout();
    m_contentsVLayout->addWidget(m_nameLabel);
    m_contentsVLayout->addWidget(m_lastMessageLabel);

    m_mainHLayout = new QHBoxLayout(this);
    m_mainHLayout->setContentsMargins(70, 10, 10, 10); // Отступы
    m_mainHLayout->addLayout(m_contentsVLayout);


    setLayout(m_mainHLayout);
}

void ChatComponent::setName(const QString& name) {
    m_nameLabel->setText(name);
}

void ChatComponent::setMessage(const QString& message) {
    m_lastMessageLabel->setText(message);
}

void ChatComponent::setAvatar(const QPixmap& avatar) {
    m_avatar = avatar.scaled(m_avatarSize, m_avatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
}


void ChatComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    // Рисуем закругленный прямоугольник (фон)
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(25, 25, 25));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 5, 5);

    // Рисуем круглый аватар
    if (!m_avatar.isNull()) {
        QPainterPath path;
        QRectF avatarRect(10, (height() - m_avatarSize) / 2, m_avatarSize, m_avatarSize);
        path.addEllipse(avatarRect);
        painter.setClipPath(path);
        painter.drawPixmap(avatarRect.toRect(), m_avatar);
        painter.setClipPath(QPainterPath());
    }
}