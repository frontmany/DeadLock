#include "chatHeaderComponent.h"
#include "mainwindow.h"
#include "buttons.h"




ChatHeaderComponent::ChatHeaderComponent(QWidget* parent, Theme theme, QString name, QString lastSeen, QPixmap avatar)
    : QWidget(parent), m_theme(theme) {

    if (m_theme == DARK) {
        m_backcolor = QColor(46, 46, 46);
        update();
    }
    else {
        m_backcolor = QColor(224, 224, 224);
        update();
    }

    //TODO QPIXMAP set avatar

    m_mainLayout = new QHBoxLayout(this);
    m_leftIcon = new ButtonIcon(this);

    QIcon icon1(":/resources/ChatsWidget/userFriendDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/userFriendHoverDark.png");
    m_leftIcon->uploadIconsDark(icon1, iconHover1);

    QIcon icon2(":/resources/ChatsWidget/userFriendLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/userFriendHoverLight.png");
    m_leftIcon->uploadIconsLight(icon2, iconHover2);
    m_leftIcon->setTheme(m_theme);

    m_mainLayout->addWidget(m_leftIcon);


    m_rightLayout = new QVBoxLayout();

    m_nameLabel = new QLabel(name, this);
    m_lastSeenLabel = new QLabel(lastSeen, this);

    m_rightLayout->addWidget(m_nameLabel);
    m_rightLayout->addWidget(m_lastSeenLabel);

    m_mainLayout->addLayout(m_rightLayout);

    m_rightButton = new ButtonIcon( this);
    QIcon icon3(":/resources/ChatsWidget/menuDark.png");
    QIcon iconHover3(":/resources/ChatsWidget/menuHoverDark.png");
    m_rightButton->uploadIconsDark(icon3, iconHover3);

    QIcon icon4(":/resources/ChatsWidget/menuLight.png");
    QIcon iconHover4(":/resources/ChatsWidget/menuHoverLight.png");
    m_rightButton->uploadIconsLight(icon4, iconHover4);
    m_rightButton->setTheme(m_theme);
    m_rightButton->setIconSize(QSize(40, 30));
    m_mainLayout->addWidget(m_rightButton);

    setLayout(m_mainLayout);
}


void ChatHeaderComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);


    painter.setBrush(QBrush(m_backcolor));
    painter.setPen(Qt::NoPen); 


    QRect rect = this->rect();
    int radius = 10; 
    painter.drawRoundedRect(rect, radius, radius); 

    QWidget::paintEvent(event);
}