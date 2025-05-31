#include "innerComponent.h"
#include "buttons.h"

StyleInnerComponent::StyleInnerComponent() {
    labelStyleDarkMessage = R"(
        QLabel {
            background-color: transparent; 
            color: rgb(240, 240, 240); 
            font-family: 'Segoe UI'; 
            font-size: 14px; 
            font-weight: normal;
            padding: 10px; 
            border: none; 
            border-radius: 20px; 
        }
        )";

    labelStyleLightMessage = R"(
        QLabel {
            background-color: transparent; 
            color: rgb(52, 52, 52); 
            font-family: 'Segoe UI'; 
            font-size: 14px; 
            font-weight: normal;
            padding: 10px; 
            border: none; 
            border-radius: 20px; 
        }
        )";

    labelStyleDarkTime = R"(
        QLabel {
            background-color: transparent; 
            color: rgb(219, 219, 219); 
            font-family: 'Segoe UI'; 
            font-size: 13px; 
            padding: 10px; 
            border: none; 
            border-radius: 20px; 
        }
        )";

    labelStyleLightTime = R"(
        QLabel {
            background-color: transparent; 
            color: rgb(153, 153, 153); 
            font-family: 'Segoe UI'; 
            font-size: 13px; 
            padding: 10px; 
            border: none; 
            border-radius: 20px; 
        }
        )";
};



InnerComponent::InnerComponent(QWidget* parent, const QString& timestamp, const QString& text, Theme theme, bool isSent) {
    style = new StyleInnerComponent;
    m_isSent = isSent;

    m_main_HLayout = new QHBoxLayout(this);
    m_main_HLayout->setAlignment(Qt::AlignRight);
    m_main_HLayout->setSpacing(5);
    m_main_HLayout->setContentsMargins(5, 5, 5, 5);

    m_textLabel = new QLabel(this);
    m_textLabel->adjustSize();
    m_textLabel->setWordWrap(true);
    m_textLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_textLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_textLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_textLabel->setStyleSheet(style->labelStyleDarkMessage);
    m_textLabel->setText(text);





    m_text_VLayout = new QVBoxLayout;
    m_text_VLayout->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_text_VLayout->addSpacing(-3);
    m_text_VLayout->addWidget(m_textLabel);


    m_timestampLabel = new QLabel(this);
    m_timestampLabel->setStyleSheet(style->labelStyleDarkTime);
    m_timestampLabel->setText(timestamp);
    m_timestampLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);


    m_time_VLayout = new QVBoxLayout;
    m_time_VLayout->setAlignment(Qt::AlignBottom);
    m_time_VLayout->addWidget(m_timestampLabel);
    m_time_VLayout->addSpacing(-6);


    m_readStatusBtn = new ButtonIcon(this, 30, 30);
    m_readStatusBtn->uploadIconsLight(QIcon(":/resources/ChatsWidget/check.png"), QIcon(":/resources/ChatsWidget/check.png"));
    m_readStatusBtn->uploadIconsDark(QIcon(":/resources/ChatsWidget/notCheck.png"), QIcon(":/resources/ChatsWidget/notCheck.png"));

    m_isRead_VLayout = new QVBoxLayout;
    m_isRead_VLayout->setAlignment(Qt::AlignBottom);
    m_isRead_VLayout->addWidget(m_readStatusBtn);

    m_isRead_VLayout->addSpacing(-12);


    m_main_HLayout->addLayout(m_text_VLayout);
    m_main_HLayout->addLayout(m_time_VLayout);
    m_main_HLayout->addSpacing(-4);

    if (m_isSent == true) {
        m_main_HLayout->addLayout(m_isRead_VLayout);
    }
    else {
        m_readStatusBtn->hide();
    }

    this->setLayout(m_main_HLayout);
    this->setMinimumSize(30, 35);
    this->setMaximumSize(600, 8000);
    setTheme(m_theme);
}

InnerComponent::~InnerComponent() {
    delete m_textLabel;
    delete m_timestampLabel;
    delete m_readStatusBtn;
    delete style;
}

void InnerComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    QRect rect = this->rect();
    int radius = 20;
    path.addRoundedRect(rect, radius, radius);
    painter.fillPath(path, m_backColor);

    QWidget::paintEvent(event);
}

void InnerComponent::setReadStatus(bool read) {

    if (read) {
        m_is_read = true;
        m_readStatusBtn->setTheme(LIGHT);
    }
    else {
        m_is_read = false;
        m_readStatusBtn->setTheme(DARK);
    }
}

void InnerComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (theme == DARK) {
        if (m_isSent) {
            m_backColor = QColor(102, 102, 102);
            m_textLabel->setStyleSheet(style->labelStyleDarkMessage);
            m_timestampLabel->setStyleSheet(style->labelStyleDarkTime);
        }
        else {
            m_backColor = QColor(71, 71, 71);
            m_textLabel->setStyleSheet(style->labelStyleDarkMessage);
            m_timestampLabel->setStyleSheet(style->labelStyleDarkTime);
        }

    }
    else {
        if (m_isSent) {
            m_backColor = QColor(212, 229, 255);
            m_textLabel->setStyleSheet(style->labelStyleLightMessage);
            m_timestampLabel->setStyleSheet(style->labelStyleLightTime);
        }
        else {
            m_backColor = QColor(224, 224, 224);
            m_textLabel->setStyleSheet(style->labelStyleLightMessage);
            m_timestampLabel->setStyleSheet(style->labelStyleLightTime);
        }
    }
    update();
}