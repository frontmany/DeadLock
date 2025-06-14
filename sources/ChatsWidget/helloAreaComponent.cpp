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

    m_label = new QLabel("select a chat to start");
    m_label->setMinimumSize(150, 40);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_main_HLayout = new QHBoxLayout;
    m_main_HLayout->addWidget(m_label);
    m_main_HLayout->setAlignment(Qt::AlignHCenter);

    m_main_VLayout->addLayout(m_main_HLayout);
    
    setTheme(m_theme);
    setLayout(m_main_VLayout);
}

void HelloAreaComponent::setTheme(Theme theme) {
    m_theme = theme;
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
    painter.setPen(Qt::NoPen);


    QPainterPath path;
    QRect rect = this->rect();
    int radius = 20; 
    path.addRoundedRect(rect, radius, radius);


    painter.setClipPath(path); 
    painter.drawPixmap(rect, m_background);
    painter.setClipping(false);

    QWidget::paintEvent(event);
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