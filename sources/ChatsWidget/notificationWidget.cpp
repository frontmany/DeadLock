#include "notificationWidget.h"
#include "chatsWidget.h"
#include "buttons.h"
#include "avatar.h"
#include "utility.h"
#include "chatsListComponent.h"
#include "client.h"
#include "chat.h"

NotificationWidget::NotificationWidget(ChatsWidget* chatsWidget, Chat* chat, Theme theme,
    QWidget* parent)
    : QWidget(parent),
    m_chat(chat),
    m_theme(theme),
    m_isCloseClicked(false)
{
    setWindowFlags(Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setFixedSize(330, 100);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(16);

    m_avatar = new AvatarIcon(this, 48, 64, false, m_theme);
    loadAvatar(chat->getFriendAvatar());
    m_avatar->setFixedSize(64, 64);
    installEventFilter(this);
    mainLayout->addWidget(m_avatar, 0, Qt::AlignLeft | Qt::AlignVCenter);
    mainLayout->addSpacing(-8);

    QVBoxLayout* textLayout = new QVBoxLayout(this);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->addSpacing(10);
    textLayout->setAlignment(Qt::AlignCenter);

    m_senderLabel = new QLabel(QString::fromStdString(chat->getFriendName()), this);
    m_senderLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    m_senderLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    textLayout->addWidget(m_senderLabel);
    textLayout->addSpacing(-10);

    m_messageLabel = new QLabel("New message", this);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    m_messageLabel->setFixedWidth(100);
    m_messageLabel->setStyleSheet("color: #666666; font-size: 13px;");
    m_messageLabel->installEventFilter(this);
    textLayout->addWidget(m_messageLabel);
    textLayout->addStretch();

    mainLayout->addLayout(textLayout);

    // Buttons container (hidden mode + close)
    QWidget* buttonsContainer = new QWidget(this);
    QHBoxLayout* buttonsLayout = new QHBoxLayout(buttonsContainer);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);

    // Hidden mode button
    m_hiddenModeButton = new QPushButton("Show hidden", this);
    m_hiddenModeButton->setFlat(true);
    m_hiddenModeButton->setAttribute(Qt::WA_NoMousePropagation);
    if (m_theme == DARK) {
        m_hiddenModeButton->setStyleSheet(
            "QPushButton {"
            "  color: rgb(100, 180, 255);"
            "  background-color: rgba(26, 133, 255, 0.15);"
            "  border-radius: 5px;"
            "  font-size: 12px;"
            "  padding: 6px 8px;"
            "  text-align: center;"
            "  border: none;"
            "}"
            "QPushButton:hover {"
            "  color: rgb(26, 133, 255);"
            "  background-color: rgba(26, 133, 255, 0.30);"
            "}"
            "QPushButton:pressed {"
            "  color: rgb(0, 102, 204);"
            "  background-color: rgba(0, 102, 204, 0.40);"
            "}"
            "QPushButton:disabled {"
            "  color: #555555;"
            "  background-color: rgba(80, 80, 80, 0.3);"
            "}");
    }
    else {
        m_hiddenModeButton->setStyleSheet(
            "QPushButton {"
            "  color: rgb(0, 102, 204);"
            "  background-color: rgba(0, 102, 204, 0.15);"
            "  border-radius: 5px;"
            "  font-size: 12px;"
            "  padding: 6px 8px;"
            "  text-align: center;"
            "  border: none;"
            "}"
            "QPushButton:hover {"
            "  color: rgb(26, 133, 255);"
            "  background-color: rgba(26, 133, 255, 0.30);"
            "}"
            "QPushButton:pressed {"
            "  color: rgb(0, 76, 153);"
            "  background-color: rgba(0, 76, 153, 0.40);"
            "}"
            "QPushButton:disabled {"
            "  color: #A0A0A0;"
            "  background-color: rgba(200, 200, 200, 0.3);"
            "}");
    }
    
    connect(m_hiddenModeButton, &QPushButton::clicked, [this, chatsWidget]() {
        closeNotification(false);
        if (!m_isCloseClicked) {
            if (!chatsWidget->getChatsList()->getIsHidden()) {
                chatsWidget->getChatsList()->onHideButtonToggled(true, true);
            }
            
            chatsWidget->onNotificationClicked(m_chat);
        }
    });
    buttonsLayout->addWidget(m_hiddenModeButton);


    // Close button
    m_closeButton = new ButtonIcon(this, 20, 20);
    QIcon icon1(":/resources/ChatsWidget/closeDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/closeHoverDark.png");
    m_closeButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/closeLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/closeHoverLight.png");
    m_closeButton->uploadIconsLight(icon2, iconHover2);
    m_closeButton->setTheme(m_theme);
    m_closeButton->setAttribute(Qt::WA_NoMousePropagation);
    connect(m_closeButton, &ButtonIcon::clicked, [this]() {
        m_isCloseClicked = true;
        closeNotification(true);
    });
    buttonsLayout->addWidget(m_closeButton);

    mainLayout->addWidget(buttonsContainer, 0, Qt::AlignRight | Qt::AlignTop);

    // Shadow effect
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setXOffset(0);
    shadow->setYOffset(4);
    shadow->setColor(QColor(0, 0, 0, 60));
    setGraphicsEffect(shadow);

    // Timer setup
    m_closeTimer = new QTimer(this);
    m_closeTimer->setSingleShot(true);
    m_closeTimer->setInterval(5000);
    connect(m_closeTimer, &QTimer::timeout, [this]() { closeNotification(true); });
    m_closeTimer->start();

    // Connect clicked signal
    connect(this, &NotificationWidget::clicked, [chatsWidget, this]() {
        closeNotification(false);
        if (!m_isCloseClicked) {
            chatsWidget->onNotificationClicked(m_chat);
        }
    });
}

void NotificationWidget::setTheme(Theme theme) {
    m_theme = theme;
    m_avatar->setTheme(theme);
    m_closeButton->setTheme(theme);
    // Устанавливаем стиль без hover-эффекта
    setHoveredStyle(false);
}


void NotificationWidget::loadAvatar(const Avatar* avatar)
{
    if (avatar == nullptr) {
        m_avatar->setIcon(QIcon(":/resources/ChatsWidget/userFriend.png"));
        return;
    }

    try {
        std::string binaryData = avatar->getBinaryData();
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

void NotificationWidget::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);

    QRect screenGeometry = QGuiApplication::primaryScreen()->availableGeometry();
    move(screenGeometry.right() - width() - 20,
        screenGeometry.bottom() - height() - 20);
}


void NotificationWidget::enterEvent(QEnterEvent* event)
{
    setHoveredStyle(true);
    if (m_closeTimer && m_closeTimer->isActive()) {
        m_closeTimer->stop();
    }
    QWidget::enterEvent(event);
}


void NotificationWidget::leaveEvent(QEvent* event)
{
    setHoveredStyle(false);
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

void NotificationWidget::setHoveredStyle(bool isHovered) {
    QString style;

    if (m_theme == Theme::DARK) {
        // В темной теме - светлее при наведении
        style = QString(
            "NotificationWidget {"
            "  background-color: %1;"
            "  border-radius: 12px;"
            "}"
        ).arg(isHovered ? "#383838" : "#2D2D2D");
    }
    else {
        // В светлой теме - темнее при наведении
        style = QString(
            "NotificationWidget {"
            "  background-color: %1;"
            "  border-radius: 12px;"
            "  border: 1px solid #E0E0E0;"
            "}"
        ).arg(isHovered ? "#F5F5F5" : "#FFFFFF");
    }

    // Сохраняем базовые стили для внутренних компонентов
    QString baseStyle;
    if (m_theme == Theme::DARK) {
        baseStyle =
            "QLabel {"
            "  color: #FFFFFF;"
            "}"
            "QPushButton {"
            "  color: #7EB6FF;"
            "}";
    }
    else {
        baseStyle =
            "QLabel {"
            "  color: #333333;"
            "}"
            "QPushButton {"
            "  color: #4A90E2;"
            "}";
    }

    setStyleSheet(style + baseStyle);
    update();
}

void NotificationWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 8, 8);

    if (m_theme == Theme::DARK) {
        QColor baseColor(31, 31, 31);
        painter.fillPath(path, baseColor);
    }
    else {
        QColor baseColor(255, 255, 255);
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
