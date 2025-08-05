#include "theme.h"
#include "chatComponent.h"
#include "chatsWidget.h"
#include "addChatDialogComponent.h"
#include "buttons.h"
#include "avatar.h"
#include "chat.h"
#include "utility.h"
#include "client.h"
#include <QRadialGradient>

ChatComponent::ChatComponent(QWidget* parent, ChatsWidget* chatsWidget, Chat* chat)
    : QWidget(parent), m_avatarSize(50), m_theme(DARK), m_chat(chat), m_isClicked(true), m_isSelected(false), m_isOnline(false) {


    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);

    setMinimumWidth(100);

    m_nameLabel = new QLabel(this);
    m_nameLabel->setText(QString::fromStdString(m_chat->getFriendName()));

    m_lastMessageLabel = new QLabel(this);
    m_nameLabel->setText(QString::fromStdString(m_chat->getLastReceivedOrSentMessage()));

    if (m_chat->getIsFriendHasAvatar() == true) {
        try {
            auto avatar = m_chat->getFriendAvatar();
            if (avatar != nullptr) {
                const std::string& data = avatar->getBinaryData();

                QByteArray imageData(data.data(), data.size());
                if (!m_avatar.loadFromData(imageData)) {
                    throw std::runtime_error("Failed to load avatar");
                }
            }
            else {
                m_avatar = QPixmap(":/resources/ChatsWidget/userFriend.png");
            }

            update();
        }
        catch (const std::exception& e) {
            m_avatar = QPixmap(":/resources/ChatsWidget/userFriend.png");
            qWarning() << "Avatar load error:" << e.what();
        }
    }
    else {
        if (m_theme == DARK) {
            m_avatar = QPixmap(":/resources/ChatsWidget/userFiend.png");
        }
        else {
            m_avatar = QPixmap(":/resources/ChatsWidget/userFiend.png");

        }
        update();
    }

    m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(122, 122, 122); font-family: 'Segoe UI'; ");

    m_contentsVLayout = new QVBoxLayout();
    m_contentsVLayout->addWidget(m_nameLabel);
    m_contentsVLayout->addWidget(m_lastMessageLabel);

    m_avatar_ico = new AvatarIcon(this, 50, 50, false, m_theme);
    if (chat->getIsFriendHasAvatar()) {
        QIcon avatarIcon(m_avatar);
        m_avatar_ico->setIcon(avatarIcon);
    }
    else {
        QIcon avatarIcon(":/resources/ChatsWidget/userFriend.png");
        m_avatar_ico->setIcon(avatarIcon);
    }
    
    m_UnreadDot = new ButtonIcon(this, 40, 40);
    QIcon icon1(":/resources/ChatsWidget/unreadDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/unreadDark.png");
    m_UnreadDot->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/unreadLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/unreadLight.png");
    m_UnreadDot->uploadIconsLight(icon2, iconHover2);
    m_UnreadDot->hide();
    if (m_chat->getUnreadReceiveMessagesVec().size() > 0){
        m_UnreadDot->show();
    }
    m_UnreadDot->setTheme(m_theme);

    m_statusVLayout = new  QVBoxLayout;
    m_statusVLayout->setAlignment(Qt::AlignBottom);
    m_statusVLayout->addWidget(m_UnreadDot);
    m_statusVLayout->addSpacing(-5);

    m_mainHLayout = new QHBoxLayout();
    m_mainHLayout->setContentsMargins(10, 10, 10, 10);
    m_mainHLayout->addWidget(m_avatar_ico);
    m_mainHLayout->addSpacing(5);
    m_mainHLayout->addLayout(m_contentsVLayout);
    m_mainHLayout->addSpacing(500);
    m_mainHLayout->addLayout(m_statusVLayout);

    m_hoverColorLight = QColor(214, 214, 214);
    m_hoverColorDark = QColor(56, 56, 56);

    connect(this, &ChatComponent::clicked, this, &ChatComponent::slotToSendChatData);
    connect(this, &ChatComponent::sendChatData, chatsWidget, &ChatsWidget::onSetChatMessagingArea);

    setLayout(m_mainHLayout);
}

void ChatComponent::setOnlineIndicator(bool isOnline) {
    m_isOnline = isOnline;
    m_avatar_ico->setOnlineIndicator(isOnline);
}

void ChatComponent::setSelected(bool isSelected) {
    m_isSelected = isSelected;
    if (isSelected == true) {
        if (m_theme == DARK) {
            m_backColor = QColor(135, 135, 135);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(240, 240, 240); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(250, 250, 250);");
            m_currentColor = m_backColor;
        }
        else {
            m_backColor = QColor(26, 133, 255);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(240, 240, 240); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(250, 250, 250);");
            m_currentColor = m_backColor;
        }
        update();
    }
    else {
        if (m_theme == DARK) {
            m_backColor = QColor(25, 25, 25);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(122, 122, 122); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(201, 201, 201);");
            m_currentColor = m_backColor;
        }
        else {
            m_backColor = QColor(229, 228, 226);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(122, 122, 122); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(20, 20, 20);");
            m_currentColor = m_backColor;
        }
        update();
    }
}

void ChatComponent::setTheme(Theme theme) {
    m_theme = theme;
    m_UnreadDot->setTheme(m_theme);
    m_avatar_ico->setTheme(m_theme);
    if (m_theme == DARK) {
        m_backColor = QColor(25, 25, 25);
        m_currentColor = m_backColor;
        if (m_isSelected) {
            setSelected(true);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(240, 240, 240); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(250, 250, 250);");
        }
        else {
            setSelected(false);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(161, 161, 161); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(201, 201, 201);");
        }
        update();
    }
    else {
        m_backColor = QColor(255, 255, 255);
        m_currentColor = m_backColor;
        if (m_isSelected) {
            setSelected(true);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(240, 240, 240); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(250, 250, 250);");
        }
        else {
            setSelected(false);
            m_lastMessageLabel->setStyleSheet("font-size: 12px; color: rgb(122, 122, 122); font-family: 'Segoe UI'; ");
            m_nameLabel->setStyleSheet("font-weight: bold; font-size: 14px; color: rgb(20, 20, 20);");
        }
        update();
    }
}

void ChatComponent::setUnreadMessageDot(bool isUnreadMessages) {
    if (isUnreadMessages == true) {
        m_UnreadDot->show();
    }
    else {
        m_UnreadDot->hide();
    }
}

void ChatComponent::setName(const QString& name) {
    m_nameLabel->setText(name);
    update();
}

void ChatComponent::setLastMessage(const QString& message) {
    QString cleanedMessage = message;
    cleanedMessage.replace('\n', ' ');

    if (cleanedMessage.length() > 10) {
        cleanedMessage = cleanedMessage.left(10) + "...";
    }

    m_lastMessageLabel->setText(cleanedMessage);
}

void ChatComponent::setLastMessageAsIncomingFilesIndicator() {
    m_lastMessageLabel->setText("Files arriving securely... ");
}

void ChatComponent::setAvatar(const QPixmap& avatar) {
    m_avatar = avatar.scaled(m_avatarSize, m_avatarSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QIcon avatarIcon(m_avatar);
    m_avatar_ico->setIcon(avatarIcon);
    update();
}

void ChatComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(m_currentColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 5, 5);
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
        if (m_isSelected == true) {
            m_currentColor = QColor(26, 133, 255);
        }
        else {
            m_currentColor = m_hoverColorLight;
        }

    }
    else {
        if (m_isSelected == true) {
            m_currentColor = QColor(135, 135, 135);
        }
        else {
            m_currentColor = m_hoverColorDark;
        }
    }
    update();
}

void ChatComponent::hoverLeave(QHoverEvent* event)
{
    m_currentColor = m_backColor;
    update();
}



void ChatComponent::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_isSelected) {
            return;
        }

        m_isSelected = true;
        setSelected(true);
        emit clicked();
    }
}

void ChatComponent::slotToSendChatData() {
    setUnreadMessageDot(false);
    emit sendChatData(m_chat, this);

}