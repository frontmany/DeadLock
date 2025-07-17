#include "notificationWidget.h"
#include "chatsWidget.h"
#include "buttons.h"
#include "photo.h"
#include "utility.h"
#include "client.h"
#include "chatsWidget.h"



NotificationWidget::NotificationWidget(ChatsWidget* chatsWidget, const QString& message,
    const Photo* photo,
    QWidget* parent)
    : QWidget(parent),
    m_chats_widget(chatsWidget),
    m_theme(Theme::LIGHT)
{
    setWindowFlags(Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint |
        Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowWithoutActivating);
    setFixedSize(300, 80);

    m_avatar = new AvatarIcon(this, 24, 48, false, m_theme);
    loadAvatar(photo); 
    m_avatar->setFixedSize(48, 48);

    m_messageLabel = new QLabel(message, this);
    m_messageLabel->setWordWrap(true);
    m_messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_messageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_closeButton = new QPushButton(this);
    m_closeButton->setFixedSize(20, 20);
    m_closeButton->setIcon(QIcon(":/resources/ChatsWidget/closeLight.png"));
    m_closeButton->setFlat(true);
    connect(m_closeButton, &QPushButton::clicked, this, &NotificationWidget::closeNotification);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(12, 12, 12, 12);
    mainLayout->setSpacing(12);

    mainLayout->addWidget(m_avatar);
    mainLayout->addWidget(m_messageLabel, 1);
    mainLayout->addWidget(m_closeButton, 0, Qt::AlignTop);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0, 2);
    setGraphicsEffect(shadow);

    QTimer::singleShot(5000, this, &QWidget::hide);
}

void NotificationWidget::loadAvatar(const Photo* photo)
{
    if (photo == nullptr) {
        m_avatar->setIcon(QIcon(":/resources/ChatsWidget/userFriend.png"));
        return;
    }

    try {
        std::string path = photo->getPhotoPath();
        if (path.empty()) {
            throw std::runtime_error("Empty photo path");
        }

        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open photo file");
        }

        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string fileData(fileSize, '\0');
        file.read(&fileData[0], fileSize);
        file.close();

        size_t delimiterPos = fileData.find('\n');
        if (delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid photo file format");
        }

        std::string encryptedKey = fileData.substr(0, delimiterPos);
        std::string encryptedData = fileData.substr(delimiterPos + 1);

        auto aesKey = utility::RSADecryptKey(m_chats_widget->getClient()->getPrivateKey(), encryptedKey);
        std::string decryptedData = utility::AESDecrypt(aesKey, encryptedData);

        QByteArray imageData(decryptedData.data(), decryptedData.size());
        QPixmap avatarPixmap;
        if (avatarPixmap.loadFromData(imageData)) {
            m_avatar->setIcon(QIcon(avatarPixmap));
        }
        else {
            throw std::runtime_error("Failed to load decrypted avatar");
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

    QString style;
    if (m_theme == Theme::DARK) {
        m_closeButton->setIcon(QIcon(":/resources/ChatsWidget/closeDark.png"));

        style =
            "NotificationWidget {"
            "  background-color: rgba(40, 40, 40, 220);"
            "  border-radius: 8px;"
            "}"
            "QLabel {"
            "  color: white;"
            "  font: 12px;"
            "}"
            "QPushButton {"
            "  background: transparent;"
            "  border: none;"
            "  padding: 2px;"
            "}"
            "QPushButton:hover {"
            "  background: rgba(255, 255, 255, 30);"
            "  border-radius: 4px;"
            "}";
    }
    else {
        m_closeButton->setIcon(QIcon(":/resources/ChatsWidget/closeLight.png"));

        style =
            "NotificationWidget {"
            "  background-color: rgba(255, 255, 255, 220);"
            "  border-radius: 8px;"
            "  border: 1px solid #e0e0e0;"
            "}"
            "QLabel {"
            "  color: black;"
            "  font: 12px;"
            "}"
            "QPushButton {"
            "  background: transparent;"
            "  border: none;"
            "  padding: 2px;"
            "}"
            "QPushButton:hover {"
            "  background: rgba(0, 0, 0, 10);"
            "  border-radius: 4px;"
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

void NotificationWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 8, 8);

    if (m_theme == Theme::DARK) {
        painter.fillPath(path, QColor(40, 40, 40, 220));
    }
    else {
        painter.fillPath(path, QColor(255, 255, 255, 220));
        painter.setPen(QColor(224, 224, 224));
        painter.drawPath(path);
    }
}

void NotificationWidget::closeNotification()
{
    hide();
    deleteLater();
}