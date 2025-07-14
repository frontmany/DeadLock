#include "theme.h"
#include "chatHeaderComponent.h"
#include "messagingAreaComponent.h"
#include "buttons.h"
#include "utility.h"

StyleChatHeaderComponent::StyleChatHeaderComponent(){
    deepBlueLabelStyle = R"(
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;  
            font-weight: normal;
            border: none;   
            font-size: 14px;
            color: rgb(64, 140, 255);            
        }
        )";

    blueLabelStyle = R"(
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;  
            font-weight: normal;
            border: none;   
            font-size: 14px;
            color: rgb(104, 163, 252);            
        }
        )";

    lightLabelStyle = R"(
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;  
            font-weight: bold;
            border: none;   
            font-size: 14px;
            color: rgb(219, 219, 219);            
        }
        )";

    darkLabelStyle = R"(
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;  
            font-weight: bold;
            border: none;   
            font-size: 14px;
            color: rgb(47, 47, 48);            
        }
        )";

    grayLabelStyle = R"(
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;  
            font-weight: normal;
            border: none;   
            font-size: 14px;
            color: rgb(120, 120, 120);            
        }
        )";
};


ChatHeaderComponent::ChatHeaderComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Theme theme, QString name, QString lastSeen, QPixmap avatar)
    : QWidget(parent), m_theme(theme), m_messaging_area_component(messagingAreaComponent) 
{
    style = new StyleChatHeaderComponent;

    // typing component
    m_typingContainer = new QWidget(this);
    QHBoxLayout* typingLayout = new QHBoxLayout(m_typingContainer);
    typingLayout->setContentsMargins(0, 0, 0, 0);
    typingLayout->setSpacing(4);

    QLabel* typingText = new QLabel("typing", m_typingContainer);
    typingText->setStyleSheet("color: #5E5E5E; font-size: 12px;");

    QMovie* typingMovie = new QMovie(":/resources/ChatsWidget/typing.gif");
    QLabel* typingGif = new QLabel(this);
    typingGif->setMovie(typingMovie);
    typingGif->setFixedSize(25, 25);
    typingGif->setAlignment(Qt::AlignCenter);
    typingGif->setScaledContents(true);
    typingMovie->start();

    typingLayout->addWidget(typingText, 0, Qt::AlignVCenter);
    typingLayout->addWidget(typingGif, 0, Qt::AlignVCenter);
    typingLayout->addStretch();
    m_typingContainer->hide();

    m_leftIcon = new AvatarIcon(this, 32, 50, true, m_theme);
    if (avatar.isNull()) {
        QIcon icon(":/resources/ChatsWidget/userFriend.png");
        m_leftIcon->setIcon(icon);
    }
    else {
        QIcon icon(avatar);
        m_leftIcon->setIcon(icon);
    }
    connect(m_leftIcon, &AvatarIcon::clicked, [this]() {
        m_messaging_area_component->openFriendProfile();
    });

    m_nameLabel = new QLabel(name, this);
    m_lastSeenLabel = new QLabel(QString::fromStdString(utility::parseDate(lastSeen.toStdString())), this);
    
    m_rightLayout = new QVBoxLayout();
    m_rightLayout->addWidget(m_nameLabel);
    m_rightLayout->addWidget(m_lastSeenLabel);

    m_mainLayout = new QHBoxLayout(this);
    m_mainLayout->addWidget(m_leftIcon);
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
    connect(m_rightButton, &ButtonIcon::clicked, [this]() {
        m_messaging_area_component->openChatPropertiesDialog();
    });

    m_mainLayout->addWidget(m_rightButton);
    m_mainLayout->addSpacing(5);

    setLayout(m_mainLayout);
    setTheme(m_theme);
}

void ChatHeaderComponent::swapLastSeenLabel(bool isTyping) {
    if (isTyping) {
        if (m_lastSeenLabel != nullptr) {
            m_rightLayout->removeWidget(m_lastSeenLabel);
            m_lastSeenLabel->hide();
            m_rightLayout->insertWidget(1, m_typingContainer);
            m_typingContainer->show();
        }
    }
    else {
        if (m_typingContainer != nullptr) {
            m_rightLayout->removeWidget(m_typingContainer);
            m_typingContainer->hide();
            m_rightLayout->insertWidget(1, m_lastSeenLabel);
            m_lastSeenLabel->show();
        }
    }
}

void ChatHeaderComponent::setAvatar(const QPixmap& pixMap) {
    QIcon icon(pixMap);
    m_leftIcon->setIcon(icon);
    update();
}

void ChatHeaderComponent::setName(const QString& name) {
    m_nameLabel->setText(name);
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
    m_leftIcon->setTheme(m_theme);
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
        m_backColor = QColor(229, 228, 226);
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