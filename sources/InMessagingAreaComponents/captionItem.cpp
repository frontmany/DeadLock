#include "captionItem.h"

StyleCaptionItem::StyleCaptionItem() {
    darkSentWidget = R"(
        QWidget {
            background-color: #2D5278;
            border-radius: 8px;
            padding: 10px;
        }
    )";

    lightSentWidget = R"(
        QWidget {
            background-color: #D4E6FF;
            border-radius: 8px;
            padding: 10px;
        }
    )";



    darkSentMessage = R"(
        QLabel {
            color: #FFFFFF;
            font-family: "Segoe UI";
            font-size: 14px;
            background-color: transparent;
        }
    )";

    lightSentMessage = R"(
        QLabel {
            color: #000000;
            font-family: "Segoe UI";
            font-size: 14px;
            background-color: transparent;
        }
    )";



    darkReceivedWidget = R"(
        QWidget {
            border-radius: 8px;
            padding: 10px;
        }
    )";

    lightReceivedWidget = R"(
        QWidget {
            border-radius: 8px;
            padding: 10px;
        }
    )";



    darkReceivedMessage = R"(
        QLabel {
            color: #FFFFFF;
            font-family: "Segoe UI";
            font-size: 14px;
            background-color: transparent;
        }
    )";

    lightReceivedMessage = R"(
        QLabel {
            color: #000000;
            font-family: "Segoe UI";
            font-size: 14px;
            background-color: transparent;
        }
    )";



    darkTimestamp = R"(
        QLabel {
            color: #AAAAAA;
            font-size: 11px;
            background-color: transparent;
            font-family: "Segoe UI";
        }
    )";

    lightTimestamp = R"(
        QLabel {
            color: #666666;
            font-size: 11px;
            background-color: transparent;
            font-family: "Segoe UI";
        }
    )";
}


CaptionItem::CaptionItem(QWidget* parent, bool isSent, Theme theme)
    : QWidget(parent), m_isRead(false), m_isSent(isSent), m_theme(theme) {
    m_messageText = new QLabel(this);
    m_messageText->setWordWrap(true);
    m_messageText->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_messageText->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_timestampLabel = new QLabel(this);
    m_timestampLabel->setContentsMargins(0, 0, 0, 0);
    m_timestampLabel->setAlignment(Qt::AlignRight | Qt::AlignBottom);
    m_timestampLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);

    m_readStatusIcon = new QPushButton(this);
    m_readStatusIcon->setStyleSheet("background: transparent; border: none;");
    m_readStatusIcon->setIcon(QIcon("C:/prj/test_project_qt/notCheck.png"));
    m_readStatusIcon->setIconSize(QSize(16, 16));
    if (!isSent) {
        m_readStatusIcon->setVisible(false);
    }

    updateLayout();
    updateStyles();
}

CaptionItem::~CaptionItem() {
    delete m_messageText;
    delete m_timestampLabel;
    delete m_readStatusIcon;
}

void CaptionItem::setMessage(const QString& message) {
    m_messageText->setText(message);
}

void CaptionItem::setTimestamp(const QString& timestamp) {
    m_timestampLabel->setText(timestamp);
}

void CaptionItem::setTheme(Theme theme) {
    if (m_theme != theme) {
        m_theme = theme;
        updateStyles();
    }
}

void CaptionItem::updateStyles() {
    if (m_theme == Theme::DARK) {
        m_timestampLabel->setStyleSheet(m_style.darkTimestamp);

        if (m_isSent) {
            m_messageText->setStyleSheet(m_style.darkSentMessage);
            m_backgroundColor = QColor(132, 132, 130);
            setStyleSheet(m_style.darkSentWidget);
        }
        else {
            m_messageText->setStyleSheet(m_style.darkReceivedMessage);
            m_backgroundColor = QColor(85, 85, 85);
            setStyleSheet(m_style.darkReceivedWidget);
        }
    }
    else {
        m_timestampLabel->setStyleSheet(m_style.lightTimestamp);
        

        if (m_isSent) {
            m_messageText->setStyleSheet(m_style.lightSentMessage);
            m_backgroundColor = QColor(240, 248, 255);
            setStyleSheet(m_style.lightSentWidget);
        }
        else {
            m_messageText->setStyleSheet(m_style.lightReceivedMessage);
            m_backgroundColor = QColor(245, 245, 245);
            setStyleSheet(m_style.lightReceivedWidget);
        }
    }
    update();
}

void CaptionItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    QRectF rect = this->rect();

    path.moveTo(rect.topLeft());
    path.lineTo(rect.topRight());
    path.lineTo(rect.right(), rect.bottom() - 8);
    path.quadTo(rect.right(), rect.bottom(), rect.right() - 8, rect.bottom());
    path.lineTo(rect.left() + 8, rect.bottom());
    path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - 8);
    path.closeSubpath();

    painter.fillPath(path, m_backgroundColor);
    QWidget::paintEvent(event);
}

void CaptionItem::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    updateLayout();
}

void CaptionItem::updateLayout() {
    if (layout()) {
        delete layout();
    }

    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->setContentsMargins(0, 0, 0, 0);
    bottomLayout->addWidget(m_timestampLabel);

    if (m_isSent) {
        bottomLayout->addSpacing(-20);
        bottomLayout->addWidget(m_readStatusIcon);
        bottomLayout->addSpacing(-5);
    }
    bottomLayout->setAlignment(Qt::AlignRight);

    QVBoxLayout* textLayout = new QVBoxLayout();
    textLayout->setContentsMargins(6, 0, 6, 0);
    textLayout->addWidget(m_messageText);
    textLayout->addSpacing(-15);
    textLayout->addLayout(bottomLayout);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addLayout(textLayout, 1);

    setLayout(mainLayout);
}

void CaptionItem::setIsRead(bool isRead) {
    m_isRead = isRead;
    if (isRead) {
        m_readStatusIcon->setIcon(QIcon("C:/prj/test_project_qt/check.png"));
    }
    else {
        m_readStatusIcon->setIcon(QIcon("C:/prj/test_project_qt/notCheck.png"));
    }
    m_readStatusIcon->setIconSize(QSize(16, 16));
}