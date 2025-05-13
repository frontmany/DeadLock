#include "delimiterComponent.h"
#include "mainwindow.h"

DelimiterComponent::DelimiterComponent(const QString& text, QWidget* parent, Theme theme)
    : QWidget(parent), m_theme(theme), m_text(text)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(40);
    setFixedWidth(180);

    m_label = new QLabel(text, this);
    m_label->setAlignment(Qt::AlignCenter);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setAlignment(Qt::AlignCenter);
    layout->addWidget(m_label);
    layout->setContentsMargins(15, 5, 15, 5);

    setTheme(theme);
}

void DelimiterComponent::setTheme(Theme theme)
{
    m_theme = theme;

    if (m_theme == Theme::DARK) {
        m_bgColor = QColor(50, 50, 50, 180); 
        m_textColor = QColor(220, 220, 220);
    }
    else {
        m_bgColor = QColor(240, 240, 240, 200);
        m_textColor = QColor(80, 80, 80);
    }

    m_label->setStyleSheet(QString("color: %1; font-size: 11px;").arg(m_textColor.name()));
    update();
}

void DelimiterComponent::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 25, 25);

    painter.fillPath(path, m_bgColor);

    painter.setPen(Qt::NoPen);
    painter.drawPath(path);

    QWidget::paintEvent(event);
}