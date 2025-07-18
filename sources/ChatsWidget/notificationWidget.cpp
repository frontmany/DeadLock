#include "notificationWidget.h"
#include "chatsWidget.h"
#include "buttons.h"
#include "photo.h"
#include "utility.h"
#include "chatsWidget.h"
#include "client.h"
#include "chat.h"



NotificationWidget::NotificationWidget(ChatsWidget* chatsWidget, Chat* chat,
    QWidget* parent)
    : QWidget(parent),
    m_chat(chat),
    m_theme(Theme::LIGHT)
{
    setWindowFlags(Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(300, 84);

    m_avatar = new AvatarIcon(this, 24, 48, false, m_theme);
    loadAvatar(chat->getFriendPhoto()); 
    m_avatar->setFixedSize(60, 60);
    m_avatar->installEventFilter(this);

    std::string message = "New message from " + chat->getFriendName();
    m_messageLabel = new QLabel(QString::fromStdString(message), this);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_messageLabel->installEventFilter(this);

    m_closeButton = new ButtonIcon(this, 25, 25);
    QIcon icon1(":/resources/ChatsWidget/closeDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/closeHoverDark.png");
    m_closeButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/closeLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/closeHoverLight.png");
    m_closeButton->uploadIconsLight(icon2, iconHover2);
    m_closeButton->setTheme(m_theme);
    connect(m_closeButton, &ButtonIcon::clicked, [this]() {closeNotification(true); });
    connect(this, &NotificationWidget::clicked, [chatsWidget, this]() {
        closeNotification(false);
        chatsWidget->onNotificationClicked(m_chat); 
    });

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    mainLayout->addWidget(m_avatar);
    mainLayout->addWidget(m_messageLabel, 1);
    mainLayout->addWidget(m_closeButton, 0, Qt::AlignTop);

    m_closeTimer = new QTimer(this);
    m_closeTimer->setSingleShot(true);
    m_closeTimer->setInterval(5000); 

    

    connect(m_closeTimer, &QTimer::timeout, [this]() {closeNotification(true); });
    m_closeTimer->start();
}

void NotificationWidget::loadAvatar(const Photo* photo)
{
    if (photo == nullptr) {
        m_avatar->setIcon(QIcon(":/resources/ChatsWidget/userFriend.png"));
        return;
    }

    try {
        std::string binaryData = photo->getBinaryData();
        QByteArray imageData(binaryData.data(), static_cast<int>(binaryData.size()));

        QPixmap avatarPixmap;
        if (avatarPixmap.loadFromData(imageData)) {
            m_avatar->setIcon(QIcon(avatarPixmap));
        }
        else {
            throw std::runtime_error("Failed to load avatar from binary data");
        }
    }
    catch (const std::exception& e) {
        qWarning() << "Avatar load error:" << e.what();
        m_avatar->setIcon(QIcon(":/resources/ChatsWidget/userFriend.png"));
    }
}

void NotificationWidget::setTheme(Theme theme)
{
    m_theme = theme;
    m_avatar->setTheme(theme);
    m_closeButton->setTheme(theme);

    QString style;
    if (m_theme == Theme::DARK) {
        style =
            "NotificationWidget {"
            "  background-color: rgba(51, 51, 51);"
            "  border-radius: 8px;"
            "}"
            "QLabel {"
            "  color: white;"
            "  font: 12px;"
            "}";
    }
    else {
        style =
            "NotificationWidget {"
            "  background-color: rgba(255, 255, 255);"
            "  border-radius: 8px;"
            "  border: 1px solid #e0e0e0;"
            "}"
            "QLabel {"
            "  color: black;"
            "  font: 12px;"
            "}";
    }

    setStyleSheet(style);

}

void NotificationWidget::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);

    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    move(screenGeometry.right() - width() - 20,
        screenGeometry.bottom() - height() - 20);
}


void NotificationWidget::enterEvent(QEnterEvent* event)
{
    m_isHovered = true;
    update();
    if (m_closeTimer && m_closeTimer->isActive()) {
        m_closeTimer->stop();
    }
    QWidget::enterEvent(event);
}


void NotificationWidget::leaveEvent(QEvent* event)
{
    m_isHovered = false;
    update();
    if (m_closeTimer) {
        m_closeTimer->start();
    }
    QWidget::leaveEvent(event);
}

bool NotificationWidget::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        emit clicked();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

void NotificationWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 8, 8);

    if (m_theme == Theme::DARK) {
        QColor baseColor(51, 51, 51);
        if (m_isHovered) {
            baseColor = baseColor.lighter(110);
        }
        painter.fillPath(path, baseColor);
    }
    else {
        QColor baseColor(255, 255, 255);
        if (m_isHovered) {
            baseColor = baseColor.darker(110);
        }
        painter.fillPath(path, baseColor);
        painter.setPen(QColor(224, 224, 224));
        painter.drawPath(path);
    }
}



void NotificationWidget::closeNotification(bool isShouldWithAnimation)
{
    if (!isShouldWithAnimation) {
        hide();
    }

    if (!graphicsEffect()) {
        auto* opacityEffect = new QGraphicsOpacityEffect(this);
        this->setGraphicsEffect(opacityEffect);
    }
    auto* opacityEffect = static_cast<QGraphicsOpacityEffect*>(graphicsEffect());

    QPropertyAnimation* posAnimation = new QPropertyAnimation(this, "pos");
    posAnimation->setDuration(300);
    posAnimation->setStartValue(pos());
    posAnimation->setEndValue(pos() + QPoint(width(), 0));
    posAnimation->setEasingCurve(QEasingCurve::InQuad);

    QPropertyAnimation* opacityAnimation = new QPropertyAnimation(opacityEffect, "opacity");
    opacityAnimation->setDuration(150);
    opacityAnimation->setStartValue(1.0);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->setEasingCurve(QEasingCurve::InQuad);


    QParallelAnimationGroup* group = new QParallelAnimationGroup(this);
    group->addAnimation(posAnimation);
    group->addAnimation(opacityAnimation);

    connect(group, &QParallelAnimationGroup::finished, this, [this]() {
        this->hide();
        this->deleteLater();
        });

    group->start(QAbstractAnimation::DeleteWhenStopped);
}
