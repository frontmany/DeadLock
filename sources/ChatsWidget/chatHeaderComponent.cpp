#include "chatHeaderComponent.h"
#include "mainwindow.h"
#include "buttons.h"
#include "utility.h"




ChatHeaderComponent::ChatHeaderComponent(QWidget* parent, Theme theme, QString name, QString lastSeen, QPixmap avatar)
    : QWidget(parent), m_theme(theme) {

    if (m_theme == DARK) {
        m_backColor = QColor(36, 36, 36);
        update();
    }
    else {
        m_backColor = QColor(224, 224, 224);
        update();
    }

    //TODO QPIXMAP set avatar
    style = new StyleChatHeaderComponent;


    m_mainLayout = new QHBoxLayout(this);

    m_leftIcon = new AvatarIcon(this, 0, 0, 32, true);
    if (avatar.isNull()) {
        QIcon icon(":/resources/ChatsWidget/userFriend.png");
        m_leftIcon->setIcon(icon);
    }
    else {
        QIcon icon(avatar);
        m_leftIcon->setIcon(icon);
    }
    m_mainLayout->addWidget(m_leftIcon);

    m_rightLayout = new QVBoxLayout();

    m_nameLabel = new QLabel(name, this);
    if (m_theme == DARK) {
        m_nameLabel->setStyleSheet(style->lightLabelStyle);
    }
    else {
        m_nameLabel->setStyleSheet(style->darkLabelStyle);
    }

    m_lastSeenLabel = new QLabel(Utility::parseDate(lastSeen), this);
    if (m_theme == DARK) {

        if (m_lastSeenLabel->text() == "online") {
            m_lastSeenLabel->setStyleSheet(style->deepBlueLabelStyle);
        }
        else {
            m_lastSeenLabel->setStyleSheet(style->grayLabelStyle);
        }
    }
    else {
        if (m_lastSeenLabel->text() == "online") {
            m_lastSeenLabel->setStyleSheet(style->blueLabelStyle);
        }
        else {
            m_lastSeenLabel->setStyleSheet(style->grayLabelStyle);
        }
    }
    

    m_rightLayout->addWidget(m_nameLabel);
    m_rightLayout->addWidget(m_lastSeenLabel);

    m_mainLayout->addLayout(m_rightLayout);

    m_rightButton = new ButtonIcon(this, 50, 50);
    QIcon icon3(":/resources/ChatsWidget/menuDark.png");
    QIcon iconHover3(":/resources/ChatsWidget/menuHoverDark.png");
    m_rightButton->uploadIconsDark(icon3, iconHover3);

    QIcon icon4(":/resources/ChatsWidget/menuLight.png");
    QIcon iconHover4(":/resources/ChatsWidget/menuHoverLight.png");
    m_rightButton->uploadIconsLight(icon4, iconHover4);
    m_rightButton->setTheme(m_theme);
    m_rightButton->setIconSize(QSize(40, 30));
    m_mainLayout->addWidget(m_rightButton);
    m_mainLayout->addSpacing(5);

    setLayout(m_mainLayout);
}

void ChatHeaderComponent::setAvatar(const QPixmap& pixMap) {
    QIcon icon(pixMap);
    m_leftIcon->setIcon(icon);
    update();
}

void ChatHeaderComponent::setLastSeen(const QString& lastSeen) {
    m_lastSeenLabel->setText(lastSeen);
    if (m_lastSeenLabel->text() == "online") {
        if (m_theme == DARK) {
            m_lastSeenLabel->setStyleSheet(style->deepBlueLabelStyle);
        }
        else {
            m_lastSeenLabel->setStyleSheet(style->blueLabelStyle);
        }

    }
    else {
        m_lastSeenLabel->setStyleSheet(style->grayLabelStyle);
    }
}

void ChatHeaderComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (m_theme == DARK) {
        m_backColor = QColor(36, 36, 36);
        m_rightButton->setTheme(m_theme);
        m_nameLabel->setStyleSheet(style->lightLabelStyle);
        if (m_lastSeenLabel->text() == "online") {
            m_lastSeenLabel->setStyleSheet(style->deepBlueLabelStyle);
        }
        else {
            m_lastSeenLabel->setStyleSheet(style->grayLabelStyle);
        }
        update();
    }
    else {
        m_backColor = QColor(224, 224, 224);
        m_rightButton->setTheme(m_theme);
        m_nameLabel->setStyleSheet(style->darkLabelStyle);
        if (m_lastSeenLabel->text() == "online") {
            m_lastSeenLabel->setStyleSheet(style->blueLabelStyle);
        }
        else {
            m_lastSeenLabel->setStyleSheet(style->grayLabelStyle);
        }
        update();
    }
}


void ChatHeaderComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);


    painter.setBrush(QBrush(m_backColor));
    painter.setPen(Qt::NoPen); 


    QRect rect = this->rect();
    int radius = 15; 
    painter.drawRoundedRect(rect, radius, radius); 

    QWidget::paintEvent(event);
}