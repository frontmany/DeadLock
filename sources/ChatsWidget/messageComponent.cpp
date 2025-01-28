#include "MessageComponent.h"
#include "mainwindow.h"
#include "buttons.h"

InnerComponent::InnerComponent(QWidget* parent, const QString& timestamp, const QString& text, Theme theme) {
    style = new StyleInnerComponent;

    m_main_HLayout = new QHBoxLayout(this);
    m_main_HLayout->setAlignment(Qt::AlignRight); 
    m_main_HLayout->setSpacing(5);
    m_main_HLayout->setContentsMargins(5, 5, 5, 5);


    m_textLabel = new QLabel(this);
    m_textLabel->setStyleSheet(style->labelStyleDarkMessage);
    m_textLabel->setText(text);
    m_textLabel->setWordWrap(true); 
    m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_textLabel->setMaximumWidth(250); 
    m_textLabel->adjustSize();

    m_time_VLayout = new QVBoxLayout;
    m_time_VLayout->setAlignment(Qt::AlignBottom);
    m_timestampLabel = new QLabel(this);
    m_timestampLabel->setStyleSheet(style->labelStyleDarkTime);
    m_timestampLabel->setText(timestamp);
    m_timestampLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_time_VLayout->addWidget(m_timestampLabel);
    m_time_VLayout->addSpacing(-6);

    m_isRead_VLayout = new QVBoxLayout;
    m_isRead_VLayout->setAlignment(Qt::AlignBottom);
    m_readStatusBtn = new ButtonIcon(this, 30, 30);
    m_readStatusBtn->uploadIconsLight(QIcon(":/resources/ChatsWidget/check.png"), QIcon(":/resources/ChatsWidget/check.png"));
    m_readStatusBtn->uploadIconsDark(QIcon(":/resources/ChatsWidget/notCheck.png"), QIcon(":/resources/ChatsWidget/notCheck.png"));
    m_isRead_VLayout->addWidget(m_readStatusBtn);
    m_isRead_VLayout->addSpacing(-2);

    m_main_HLayout->addWidget(m_textLabel);
    m_main_HLayout->addLayout(m_time_VLayout);
    m_main_HLayout->addSpacing(-8);
    m_main_HLayout->addLayout(m_isRead_VLayout);


    this->setLayout(m_main_HLayout);
    this->setMinimumSize(10, 40);
    
    this->setMaximumSize(600, 800);
    setTheme(m_theme);
}

void InnerComponent::setReadStatus(bool read) {
    if (read) {
        m_readStatusBtn->setTheme(LIGHT);
    }
    else {
        m_readStatusBtn->setTheme(DARK);
    }
}

void InnerComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    QRect rect = this->rect();
    int radius = 20; // Радиус закругления
    path.addRoundedRect(rect, radius, radius);
    painter.fillPath(path, m_backColor); // Заливаем фон

    QWidget::paintEvent(event); // Вызываем базовый метод
}


void InnerComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (theme == DARK) {
        m_backColor = QColor(102, 102, 102);
        m_textLabel->setStyleSheet(style->labelStyleDarkMessage);
        m_timestampLabel->setStyleSheet(style->labelStyleDarkTime);
    }
    else {
        m_backColor = QColor(212, 229, 255);
        m_textLabel->setStyleSheet(style->labelStyleLightMessage);
        m_timestampLabel->setStyleSheet(style->labelStyleLightTime);

    }
    update(); 
}


MessageComponent::MessageComponent(QWidget* parent, const QString& timestamp, const QString& text, Theme theme, int id)
    : QWidget(parent), m_theme(theme), m_id(id) {
    m_innerWidget = new InnerComponent(this, timestamp, text, m_theme);
    this->setStyleSheet("background-color: transparent;");
  
    m_main_HLayout = new QHBoxLayout(this);
    m_main_HLayout->setAlignment(Qt::AlignRight); 
    m_main_HLayout->addWidget(m_innerWidget);
    m_main_HLayout->addSpacing(5);
    m_main_HLayout->setContentsMargins(5, 5, 5, 5);

   
    setLayout(m_main_HLayout);
    setReadStatus(false);
    setTheme(theme);
}
