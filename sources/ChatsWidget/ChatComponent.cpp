#include "chatComponent.h"
#include "chatsWidget.h"
#include "buttons.h"
#include "mainwindow.h"

ChatComponent::ChatComponent(QWidget* parent, ChatsWidget* chatsWidget, Chat* chat)
    : QWidget(parent), m_avatarSize(50), m_theme(DARK), m_chat(chat), m_isSelected(false), m_isClicked(true) {
    setMinimumSize(100, 70);
    setMaximumSize(1000, 70);

    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);

    m_nameLabel = new QLabel(this);
    m_lastMessageLabel = new QLabel(this);

    if (m_chat->getIsFriendHasPhoto() == true) {
        const Photo& photo = m_chat->getFriendPhoto();
        m_avatar = QPixmap(QString::fromStdString(photo.getPhotoPath()));
        update();
    }
    else {
        if (m_theme == DARK) {
            m_avatar = QPixmap(":/resources/LoginWidget/lightLoginBackground.jpg");
        }
        else {
            m_avatar = QPixmap(":/resources/LoginWidget/lightLoginBackground.jpg");

        }
        update();
    }

    m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; font-family: 'Segoe UI'; ");
    m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(122, 122, 122); font-family: 'Segoe UI'; ");
    m_lastMessageLabel->setText("no messages yet");

    m_contentsVLayout = new QVBoxLayout();
    m_contentsVLayout->addWidget(m_nameLabel);
    m_contentsVLayout->addWidget(m_lastMessageLabel);

    

    m_statusOnlineDot = new ButtonIcon(this, 50, 50);
    QIcon icon1(":/resources/ChatsWidget/online.png");
    QIcon iconHover1(":/resources/ChatsWidget/online.png");
    m_statusOnlineDot->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/online.png");
    QIcon iconHover2(":/resources/ChatsWidget/online.png");
    m_statusOnlineDot->uploadIconsLight(icon2, iconHover2);
    m_statusOnlineDot->hide();
    m_statusOnlineDot->setTheme(m_theme);

    m_statusVLayout = new  QVBoxLayout;
    m_statusVLayout->setAlignment(Qt::AlignBottom);
    m_statusVLayout->addWidget(m_statusOnlineDot);
    m_statusVLayout->addSpacing(-10);

    m_mainHLayout = new QHBoxLayout();
    m_mainHLayout->setContentsMargins(70, 10, 10, 10);
    m_mainHLayout->addLayout(m_contentsVLayout);
    m_mainHLayout->addSpacing(500);
    m_mainHLayout->addLayout(m_statusVLayout);

    m_hoverColorLight = QColor(240, 240, 240);
    m_hoverColorDark = QColor(56, 56, 56);

    connect(this, &ChatComponent::clicked, this, &ChatComponent::slotToSendChatData);
    connect(this, &ChatComponent::sendChatData, chatsWidget, &ChatsWidget::onSetChatMessagingArea);

    setLayout(m_mainHLayout);
}
void ChatComponent::setSelected(bool isSelected) {
    m_isSelected = isSelected;
    if (isSelected == true) {
        m_backColor = QColor(56, 56, 56);
        update();
    }
    else {
        m_backColor = QColor(25, 25, 25);
        update();
    }
}

void ChatComponent::setOnlineDot(bool isOnline) {
    if (isOnline == true) {
        m_statusOnlineDot->show();
    }
    else {
        m_statusOnlineDot->hide();
    }
}

void ChatComponent::setName(const QString& name) {
    m_nameLabel->setText(name);
}

void ChatComponent::setLastMessage(const QString& message) {
    m_lastMessageLabel->setText(message);
}

void ChatComponent::setAvatar(const QPixmap& avatar) {
    m_avatar = avatar.scaled(m_avatarSize, m_avatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    update();
}

void ChatComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (m_theme == DARK) {
        m_backColor = QColor(25, 25, 25);
        m_currentColor = m_backColor;
        update();
    }
    else {
        m_backColor = QColor(255, 255, 255);
        m_currentColor = m_backColor;
        m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(47, 47, 48);");
        update();
    }
}

void ChatComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);


    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(m_currentColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 5, 5);


    if (!m_avatar.isNull()) {
        QPainterPath path;
        QRectF avatarRect(10, (height() - m_avatarSize) / 2, m_avatarSize, m_avatarSize);
        path.addEllipse(avatarRect);
        painter.setClipPath(path);
        painter.drawPixmap(avatarRect.toRect(), m_avatar);
        painter.setClipPath(QPainterPath());
    }
}

bool ChatComponent::event(QEvent* event)
{
    switch (event->type())
    {
    case QEvent::HoverEnter:
        hoverEnter(static_cast<QHoverEvent*>(event));
        return true;
    case QEvent::HoverLeave:
        hoverLeave(static_cast<QHoverEvent*>(event));
        return true;
    default:
        return QWidget::event(event);
    }
}


void ChatComponent::hoverEnter(QHoverEvent* event)
{
    if (m_theme == LIGHT) {
        m_currentColor = m_hoverColorLight;
        update();
    }
    else {
        m_currentColor = m_hoverColorDark;
        update();
    }
}

void ChatComponent::hoverLeave(QHoverEvent* event)
{
    m_currentColor = m_backColor;
    update();
}



void ChatComponent::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

void ChatComponent::slotToSendChatData() {
    if (m_isSelected == false) {
        emit sendChatData(m_chat);
    }
}