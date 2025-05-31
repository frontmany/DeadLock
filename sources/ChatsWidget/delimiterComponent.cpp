#include "theme.h"
#include "delimiterComponent.h"


StyleDelimiterComponent::StyleDelimiterComponent() {
    DarkLabelStyle = R"(
    QLabel {
<<<<<<< HEAD
        color: rgb(220, 220, 220);  
=======
        color: rgb(220, 220, 220);  /* Основной цвет текста */
>>>>>>> origin/master
        font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
        font-size: 12px;
        font-weight: 600;
        padding: 2px;
        margin: 0;
        background: transparent;
        border: none;
<<<<<<< HEAD
=======
    }
    
    QLabel:disabled {
        color: rgba(220, 220, 220, 60%);  /* Полупрозрачный для disabled */
    }
    
    QLabel[accessibleName="error"] {
        color: rgb(255, 100, 100);  /* Специальный цвет для ошибок */
>>>>>>> origin/master
    }
)";

    LightLabelStyle = R"(
    QLabel {
<<<<<<< HEAD
        color: rgb(80, 80, 80);  
=======
        color: rgb(80, 80, 80);  /* Основной цвет текста */
>>>>>>> origin/master
        font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
        font-size: 12px;
        font-weight: 600;
        padding: 2px;
        margin: 0;
        background: transparent;
        border: none;
<<<<<<< HEAD
    } 
=======
    }
    
    QLabel:disabled {
        color: rgba(80, 80, 80, 40%);  /* Полупрозрачный для disabled */
    }
    
    QLabel[accessibleName="error"] {
        color: rgb(200, 50, 50);  /* Специальный цвет для ошибок */
    }
>>>>>>> origin/master
)";
};

DelimiterComponent::DelimiterComponent(const QString& text, QWidget* parent, Theme theme)
    : QWidget(parent), m_theme(theme), m_text(text)
{
    m_style = new StyleDelimiterComponent;

    setAttribute(Qt::WA_TranslucentBackground);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setFixedHeight(40);

    m_label = new QLabel(text, this);
    m_label->setFixedWidth(380);
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
        m_bgColor = QColor(25, 25, 25, 180);
        m_textColor = QColor(220, 220, 220);
        m_label->setStyleSheet(m_style->DarkLabelStyle);
    }
    else {
        m_bgColor = QColor(240, 240, 240, 200);
        m_textColor = QColor(80, 80, 80);
        m_label->setStyleSheet(m_style->LightLabelStyle);
    }

    update();
}

void DelimiterComponent::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 20, 20);

    painter.fillPath(path, m_bgColor);

    painter.setPen(Qt::NoPen);
    painter.drawPath(path);

    QWidget::paintEvent(event);
}