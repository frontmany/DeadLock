#include "MessagingAreaComponent.h"
#include "chatHeaderComponent.h"
#include "messageComponent.h"
#include "chatsWidget.h"
#include "mainWindow.h"
#include "buttons.h"
#include <random>
#include <limits>

int generateRandomNumber() {
    std::random_device rd; // Инициализация генератора случайных чисел
    std::mt19937 gen(rd()); // Используем Mersenne Twister
    std::uniform_int_distribution<int> distribution(0, 1000000); 
    return distribution(gen);
}

std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();

    std::time_t now_time_t = std::chrono::system_clock::to_time_t(now);

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time_t), "%H:%M");

    return oss.str(); 
}


MessagingAreaComponent::MessagingAreaComponent(QWidget* parent, QString friendName, Theme theme, Chat* chat, ChatsWidget* chatsWidget)
    : QWidget(parent), m_friendName(friendName), m_theme(theme), m_chat(chat), m_chatsWidget(chatsWidget) {

    setMinimumSize(300, 400);
    
    if (chat->getIsFriendHasPhoto() == true) {
        m_header = new ChatHeaderComponent(this, m_theme, QString::fromStdString(m_chat->getFriendName()), QString::fromStdString(m_chat->getFriendLastSeen()), QPixmap(QString::fromStdString(chat->getFriendPhoto().getPhotoPath())));
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

MessagingAreaComponent::MessagingAreaComponent(Theme theme) {
    style = new StyleMessagingAreaComponent;
    m_theme = theme;

    if (m_theme == DARK) {
        m_backColor = QColor(25, 25, 25);
        update();
    }
    else {
        m_backColor = QColor(240, 240, 240, 200);
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
    double id = generateRandomNumber();
    addMessageSent(m_messageInputEdit->toPlainText(), QString::fromStdString(getCurrentTime()), id);
    m_containerWidget->adjustSize();
    m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
    onTypeMessage();
    QString s = m_messageInputEdit->toPlainText();
    m_messageInputEdit->setText("");
    emit sendMessageData(s , QString::fromStdString(getCurrentTime()), m_chat, id);

}


void MessagingAreaComponent::onTypeMessage() {
    if (m_messageInputEdit->toPlainText() == "") {
        m_sendMessageButton->hide();
    }
    else {
        m_sendMessageButton->show();
    }
}


void MessagingAreaComponent::addMessageReceived(QString msg, QString timestamp, double id) {
    MessageComponent* message = new MessageComponent(this, timestamp, msg, m_theme, id, false);
    m_vec_messagesComponents.push_back(message);
    m_containerVLayout->addWidget(message);
}


void MessagingAreaComponent::addMessageSent(QString msg, QString timestamp, double id) {
    MessageComponent* message = new MessageComponent(this, timestamp, msg+'\n', m_theme, id, true);
    m_vec_messagesComponents.push_back(message);
    m_containerVLayout->addWidget(message);
}


void MessagingAreaComponent::addComponentToNotCurrentMessagingArea(Chat* foundChat, Msg* msg) {
    MessageComponent* message = new MessageComponent(m_chatsWidget, QString::fromStdString(msg->getTimestamp()), QString::fromStdString(msg->getMessage()), m_chatsWidget->getTheme(), msg->getId(), false);
    getMessagesComponentsVec().push_back(message);
    m_containerVLayout->addWidget(message);
}




QJsonObject MessagingAreaComponent::serialize() const {
    QJsonObject messagingAreaObject;
    messagingAreaObject["friendName"] = m_friendName;
    messagingAreaObject["theme"] = static_cast<int>(m_theme);
    messagingAreaObject["chat"] = m_chat->serialize();

    QJsonArray messagesArray;
    for (const auto& messageComponent : m_vec_messagesComponents) {
        messagesArray.append(messageComponent->serialize());
    }
    messagingAreaObject["messages"] = messagesArray;

    return messagingAreaObject;
}

MessagingAreaComponent* MessagingAreaComponent::deserialize(const QJsonObject& jsonObject, QWidget* parent, ChatsWidget* chatsWidget) {
    QString friendName = jsonObject["friendName"].toString();
    Theme theme = static_cast<Theme>(jsonObject["theme"].toInt());
    QJsonObject chatObject = jsonObject["chat"].toObject();
    Chat* chat = Chat::deserialize(chatObject);

    MessagingAreaComponent* component = new MessagingAreaComponent(parent, friendName, theme, chat, chatsWidget);
    component->hide();

    QJsonArray messagesArray = jsonObject["messages"].toArray();
    for (const auto& msgValue : messagesArray) {
        QJsonObject msgObject = msgValue.toObject();
        MessageComponent* msgComponent = MessageComponent::deserialize(msgObject);
        component->m_vec_messagesComponents.push_back(msgComponent);

        if (msgComponent->getIsSent()) {
            component->addMessageSent(msgComponent->getMessage(), msgComponent->getTimestamp(), msgComponent->getId());
        }
        else {
            component->addMessageReceived(msgComponent->getMessage(), msgComponent->getTimestamp(), msgComponent->getId());
        }
    }

    return component;
}