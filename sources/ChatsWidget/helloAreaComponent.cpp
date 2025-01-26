#include "helloAreaComponent.h"
#include "mainwindow.h"

HelloAreaComponent::HelloAreaComponent(Theme theme)
	: m_theme(theme){

	style = new StyleHelloAreaComponent();
	m_main_VLayout = new QVBoxLayout;
    m_main_VLayout->setAlignment(Qt::AlignCenter);

    m_label = new QLabel("select a chat to start");
    m_label->setMinimumSize(100, 40);
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
        m_backColor = QColor(25, 25, 25);
        m_label->setStyleSheet(style->labelStyleDark);
        update();
    }
    else {
        m_backColor = QColor(204, 204, 204, 200);
        m_label->setStyleSheet(style->labelStyleLight);
        update();
    }
}
                                                            
void HelloAreaComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);

    QWidget::paintEvent(event);
}