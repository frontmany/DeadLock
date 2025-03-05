#include "MessagingAreaComponent.h"
#include "chatHeaderComponent.h"
#include "messageComponent.h"
#include "chatsWidget.h"
#include "utility.h"
#include "mainWindow.h"
#include "buttons.h"
#include "message.h"
#include "client.h"
#include <random>
#include <limits>

MessagingAreaComponent::MessagingAreaComponent(QWidget* parent, QString friendName, Theme theme, Chat* chat, ChatsWidget* chatsWidget)
    : QWidget(parent), m_friendName(friendName), m_theme(theme), m_chat(chat), m_chatsWidget(chatsWidget) {

    setMinimumSize(300, 400);
    
    if (chat->getIsFriendHasPhoto() == true) {
        m_header = new ChatHeaderComponent(this, m_theme, QString::fromStdString(m_chat->getFriendName()), QString::fromStdString(m_chat->getFriendLastSeen()), QPixmap(QString::fromStdString(chat->getFriendPhoto()->getPhotoPath())));
    }
    else {
        m_header = new ChatHeaderComponent(this, m_theme, QString::fromStdString(m_chat->getFriendName()), QString::fromStdString(m_chat->getFriendLastSeen()), QPixmap());
    }

    style = new StyleMessagingAreaComponent;

    if (m_theme == DARK) {
        m_backColor = QColor(25, 25, 25);
    }
    else {
        m_backColor = QColor(240, 240, 240, 300);
    }

    m_containerWidget = new QWidget();
    m_containerVLayout = new QVBoxLayout(m_containerWidget);
    m_containerVLayout->setAlignment(Qt::AlignBottom);
    m_containerWidget->setLayout(m_containerVLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_containerWidget);
    m_scrollArea->setStyleSheet("background: transparent;");

    QHBoxLayout* hla = new QHBoxLayout;

    m_sendMessageButton = new ButtonCursor(this, m_theme);
    QIcon icon1(":/resources/ChatsWidget/sendDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/sendHoverDark.png");
    m_sendMessageButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/sendLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/sendHoverLight.png");
    m_sendMessageButton->uploadIconsLight(icon2, iconHover2);
    m_sendMessageButton->setTheme(m_theme);
    m_sendMessageButton->hide();

    m_sendMessage_VLayout = new QVBoxLayout;
    m_sendMessage_VLayout->setAlignment(Qt::AlignBottom);
    m_sendMessage_VLayout->addWidget(m_sendMessageButton);


    m_messageInputEdit = new MyTextEdit(this);
    m_messageInputEdit->setMaxLength(100);
    m_messageInputEdit->setMinimumHeight(30);
    m_messageInputEdit->setPlaceholderText("Type your message...");
    m_messageInputEdit->setAcceptRichText(false);
    m_messageInputEdit->setFixedHeight(m_messageInputEdit->document()->size().height());

    hla->addWidget(m_messageInputEdit);
    hla->addLayout(m_sendMessage_VLayout);

    m_main_VLayout = new QVBoxLayout();
    m_main_VLayout->addWidget(m_header);
    m_main_VLayout->addWidget(m_scrollArea);
    m_main_VLayout->addLayout(hla);

    m_main_VLayout->setContentsMargins(10, 10, 10, 10);
    m_main_VLayout->setSpacing(5);

    connect(m_messageInputEdit, &MyTextEdit::textChanged, this, &MessagingAreaComponent::adjustTextEditHeight);
    connect(m_messageInputEdit, &MyTextEdit::textChanged, this, &MessagingAreaComponent::onTypeMessage);
    connect(m_messageInputEdit, &MyTextEdit::enterPressed, this, &MessagingAreaComponent::onSendMessageClicked);

    connect(m_sendMessageButton, &ButtonCursor::clicked, this, &MessagingAreaComponent::onSendMessageClicked);
    connect(this, &MessagingAreaComponent::sendMessageData, m_chatsWidget, &ChatsWidget::onSendMessageData);

    setLayout(m_main_VLayout);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    adjustTextEditHeight();

    for (auto message : m_chat->getMessagesVec()) {
        addMessage(message);
    }
}

void MessagingAreaComponent::adjustTextEditHeight() {
    m_messageInputEdit->setFixedHeight(m_messageInputEdit->document()->size().height() + 12); 
}

void MessagingAreaComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (m_theme == DARK) {
        m_backColor = QColor(25, 25, 25);
        m_messageInputEdit->setStyleSheet(style->DarkTextEditStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->darkSlider);
        m_header->setTheme(DARK);
        for (auto msgComp : m_vec_messagesComponents) {
            msgComp->setTheme(DARK);
        }
        update();

    }
    else {
        m_backColor = QColor(240, 240, 240, 200);
        m_messageInputEdit->setStyleSheet(style->LightTextEditStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->lightSlider);
        m_header->setTheme(LIGHT);
        for (auto msgComp : m_vec_messagesComponents) {
            msgComp->setTheme(LIGHT);
        }
        update();
    }
}

void MessagingAreaComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backColor); 
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 15, 15); 
}


void MessagingAreaComponent::onSendMessageClicked() {
    Message* message = new Message(m_messageInputEdit->toPlainText().toStdString(), Utility::getCurrentTime(), Utility::generateId(), true, false);
    addMessage(message);
    m_containerWidget->adjustSize();
    m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
    onTypeMessage();
    QString s = m_messageInputEdit->toPlainText();
    m_messageInputEdit->setText("");
    emit sendMessageData(message, m_chat);

}


void MessagingAreaComponent::onTypeMessage() {
    if (m_messageInputEdit->toPlainText() == "") {
        m_sendMessageButton->hide();
    }
    else {
        m_sendMessageButton->show();
    }
}


void MessagingAreaComponent::addMessage(Message* message) {
    MessageComponent* messageComp = new MessageComponent(this, message, m_theme);
    if (message->getIsSend()) {
        if (message->getIsRead()) {
            messageComp->setIsRead(true);
        }
        else {
            messageComp->setIsRead(false);
        }
    }
    
    m_vec_messagesComponents.push_back(messageComp);
    m_containerVLayout->addWidget(messageComp);
}

void MessagingAreaComponent::markMessageAsChecked(Message* message) {
    std::thread th([this, message]() {
        Client* client = m_chatsWidget->getClientSide();
        client->sendMessageReadConfirmation(m_chat->getFriendLogin(), { message });
        });
    th.join();
}
