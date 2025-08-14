#include "theme.h"
#include "helloAreaComponent.h"


StyleHelloAreaComponent::StyleHelloAreaComponent() {
    LabelStyleDark = R"(
    QLabel {
        background-color: rgba(59, 59, 59, 200); 
        color: rgb(240, 240, 240); 
        font-family: 'Segoe UI'; 
        font-size: 14px; 
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";

    LabelStyleLight = R"(
    QLabel {
        background-color: rgba(148, 148, 148, 200); 
        color: rgb(240, 240, 240); 
        font-family: 'Segoe UI'; 
        font-size: 14px; 
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";
}

HelloAreaComponent::HelloAreaComponent(Theme theme)
	: m_theme(theme){

	style = new StyleHelloAreaComponent();
	m_main_VLayout = new QVBoxLayout;
    m_main_VLayout->setAlignment(Qt::AlignCenter);

    m_label = new QLabel;
    if (m_theme == DARK) {
        m_label->setText("<img src=\":/resources/ChatsWidget/lockLight.png\" width=\"16\" height=\"16\" style=\"vertical-align:middle;\"> select a chat to start");
    }
    else {
        m_label->setText("<img src=\":/resources/ChatsWidget/lockDark.png\" width=\"16\" height=\"16\" style=\"vertical-align:middle;\"> select a chat to start");
    }
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_main_HLayout = new QHBoxLayout;
    m_main_HLayout->addWidget(m_label);
    m_main_HLayout->setAlignment(Qt::AlignHCenter);

    m_main_VLayout->addLayout(m_main_HLayout);
    
    this->setMinimumSize(300, 300);
    this->setMaximumSize(1600, 3000);

    setTheme(m_theme);
    setLayout(m_main_VLayout);
}

void HelloAreaComponent::setTheme(Theme theme) {
    m_theme = theme;

    if (m_theme == DARK) {
        m_label->setText("<img src=\":/resources/ChatsWidget/lockLight.png\" width=\"16\" height=\"16\" style=\"vertical-align:middle;\"> select a chat to start");
    }
    else {
        m_label->setText("<img src=\":/resources/ChatsWidget/lockDark.png\" width=\"16\" height=\"16\" style=\"vertical-align:middle;\"> select a chat to start");
    }

    if (theme == DARK) {
        m_label->setStyleSheet(style->LabelStyleDark);
        setBackGround(m_theme);
        update();
    }
    else {
        m_label->setStyleSheet(style->LabelStyleLight);
        setBackGround(m_theme);
        update();
    }
}

void HelloAreaComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = this->rect();
    int radius = 16;

    QPainterPath path;
    path.addRoundedRect(rect, radius, radius);

    QPainterPath rightRect;
    rightRect.addRect(rect.adjusted(radius, 0, 0, 0));
    path = path.united(rightRect);

    painter.setBrush(m_background);
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);
}


void HelloAreaComponent::setBackGround(Theme theme) {
    if (theme == DARK) {
        if (m_background.load(":/resources/ChatsWidget/helloDark.jpg")) {
        }
    }
    else {
        if (m_background.load(":/resources/ChatsWidget/helloLight.jpg")) {
        }
    }
}