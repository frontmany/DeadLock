#include "MessagingAreaComponent.h"
#include "chatHeaderComponent.h"
#include "mainWindow.h"
#include "buttons.h"

MessagingAreaComponent::MessagingAreaComponent(QWidget* parent, QString friendName, Theme theme, Chat* chat)
    : QWidget(parent), m_friendName(friendName), m_theme(theme), m_chat(chat) {

    setMinimumSize(300, 400);
    
    if (chat->getIsFriendHasPhoto() == true) {
        m_header = new ChatHeaderComponent(this, m_theme, QString::fromStdString(m_chat->getFriendName()), QString::fromStdString(m_chat->getFriendLastSeen()), QPixmap(QString::fromStdString(chat->getFriendPhoto().getPhotoPath())));
    }
    else {
        m_header = new ChatHeaderComponent(this, m_theme, QString::fromStdString(m_chat->getFriendName()), QString::fromStdString(m_chat->getFriendLastSeen()), QPixmap());
    }

    style = new StyleMessagingAreaComponent;

    if (m_theme == DARK) {
        m_backColor = QColor(38, 38, 38);
    }
    else {
        m_backColor = QColor(240, 240, 240);
    }

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_messagesWidget = new QWidget(); 
    m_messagesLayout = new QVBoxLayout(m_messagesWidget); 
    m_messagesWidget->setLayout(m_messagesLayout);
    m_scrollArea->setWidget(m_messagesWidget);

    QHBoxLayout* hla = new QHBoxLayout;

    m_sendMessageButton = new ButtonIcon(this);
    QIcon icon1(":/resources/ChatsWidget/sendDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/sendHoverDark.png");
    m_sendMessageButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/sendLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/sendHoverLight.png");
    m_sendMessageButton->uploadIconsLight(icon2, iconHover2);
    m_sendMessageButton->setTheme(m_theme);
    m_sendMessageButton->hide();

    m_messageInputEdit = new QLineEdit(this);
    m_messageInputEdit->setMinimumHeight(30);
    m_messageInputEdit->setPlaceholderText("Type your message...");

    hla->addWidget(m_messageInputEdit);
    hla->addWidget(m_sendMessageButton);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(m_header);
    layout->addWidget(m_scrollArea);
    layout->addLayout(hla);
    layout->setContentsMargins(10, 10, 10, 10); 
    layout->setSpacing(5);

    connect(m_messageInputEdit, &QLineEdit::textChanged, this, &MessagingAreaComponent::onTypeMessage);

    setLayout(layout);
}

void MessagingAreaComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (m_theme == DARK) {
        m_backColor = QColor(38, 38, 38);
        m_messageInputEdit->setStyleSheet(style->DarkLineEditStyle);
        update();

    }
    else {
        m_backColor = QColor(240, 240, 240);
        m_messageInputEdit->setStyleSheet(style->LightLineEditStyle);

        update();
    }
}

MessagingAreaComponent::MessagingAreaComponent(Theme theme) {
    style = new StyleMessagingAreaComponent;
    m_theme = theme;

    if (m_theme == DARK) {
        m_backColor = QColor(38, 38, 38);
        update();
    }
    else {
        m_backColor = QColor(240, 240, 240);
        update();
    }


    //TODO if it's no chat but "hello" component
}

void MessagingAreaComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backColor); 
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 15, 15); 
}


void MessagingAreaComponent::onSendMessageClicked() {
    QLabel* messageLabel = new QLabel(m_messageInputEdit->text(), this);
    messageLabel->setWordWrap(true);
    m_messagesLayout->addWidget(messageLabel);
    m_messagesWidget->adjustSize();
    m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
}


void MessagingAreaComponent::onTypeMessage() {
    if (m_messageInputEdit->text() == "") {
        m_sendMessageButton->hide();
    }
    else {
        m_sendMessageButton->show();
    }
}