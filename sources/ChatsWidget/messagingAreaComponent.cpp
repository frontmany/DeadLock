#include "MessagingAreaComponent.h"
#include "chatHeaderComponent.h"
#include "messageComponent.h"
#include "chatsListComponent.h"
#include "chatsWidget.h"
#include "utility.h"
#include "mainWindow.h"
#include "buttons.h"
#include "message.h"
#include "client.h"
#include "photo.h"
#include <random>
#include <limits>


StyleMessagingAreaComponent::StyleMessagingAreaComponent() {
    darkSlider = R"(
    QScrollBar:vertical {
        border: 2px solid rgb(36, 36, 36);      
        background: rgb(36, 36, 36);        
        width: 10px;                 
        border-radius: 5px; 
    }

    QScrollBar::handle:vertical {
        background: rgb(56, 56, 56);   
        border: 2px solid rgb(56, 56, 56);      
        width: 10px;    
        border-radius: 5px;           
    }

    QScrollBar::add-line:vertical, 
    QScrollBar::sub-line:vertical { 
        background: none;             
    }
)";

    lightSlider = R"(
    QScrollBar:vertical {
        border: 2px solid rgb(250, 250, 250);      
        background: rgb(250, 250, 250);        
        width: 10px;                 
        border-radius: 5px; 
    }

    QScrollBar::handle:vertical {
        background: rgb(218, 219, 227);   
        border: 2px solid rgb(218, 219, 227);      
        width: 10px;    
        border-radius: 5px;           
    }

    QScrollBar::add-line:vertical, 
    QScrollBar::sub-line:vertical { 
        background: none;             
    }
)";

    DarkTextEditStyle = R"(
    QTextEdit {
        background-color: rgb(36, 36, 36);    
        color: white;               
        border: none;     
        border-radius: 15px;         
        padding: 5px;               
    }
    QTextEdit:focus {
        border: 2px solid #888;     
    }
)";

    LightTextEditStyle = R"(
    QTextEdit {
        background-color: #ffffff;    
        color: black;                 
        border: none;       
        border-radius: 15px;           
        padding: 5px;                 
    }
    QTextEdit:focus {
        border: 2px solid rgb(237, 237, 237);        
    }
)";
};

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

MessagingAreaComponent::~MessagingAreaComponent() {
    for (auto* messageComponent : m_vec_messagesComponents) {
        delete messageComponent;
    }
    m_vec_messagesComponents.clear();

    delete m_messageInputEdit;
    delete m_header;
    delete m_sendMessageButton;
    delete m_containerWidget;

    delete m_scrollArea;
    delete m_main_VLayout;
    delete m_sendMessage_VLayout;
    delete style;
}

void MessagingAreaComponent::setAvatar(const QPixmap& pixMap) {
    m_header->setAvatar(pixMap);
    update();
}

void MessagingAreaComponent::setName(const QString& name) {
    m_header->setName(name);
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
    std::string msg = m_messageInputEdit->toPlainText().toStdString();
    if (msg.find_first_not_of(' ') == std::string::npos) {
        return;
    }

    msg.erase(0, msg.find_first_not_of(' '));
    msg.erase(msg.find_last_not_of(' ') + 1);

    size_t spaceCount = std::count(msg.begin(), msg.end(), ' ');
    if (spaceCount > 1 && msg.length() > 18) {
        msg += '\n';  
    }

    updateRelatedChatComponentLastMessage();

    Message* message = new Message(msg, utility::getTimeStamp(), utility::generateId(), true, false);
    addMessage(message);
    m_containerWidget->adjustSize();
    onTypeMessage();
    QString s = m_messageInputEdit->toPlainText();
    m_messageInputEdit->setText("");
    emit sendMessageData(message, m_chat);

    moveSliderDown();
}

void MessagingAreaComponent::updateRelatedChatComponentLastMessage() {
    ChatsListComponent* comp = m_chatsWidget->getChatsList();
    std::vector<ChatComponent*> chatCompVec = comp->getChatComponentsVec();
    auto it = std::find_if(chatCompVec.begin(), chatCompVec.end(), [this](ChatComponent* chatComp) {
        return chatComp->getChat()->getFriendLogin() == m_chat->getFriendLogin();
        });

    if (it != chatCompVec.end()) {
        ChatComponent* relatedComp = *it;
        relatedComp->setLastMessage(m_messageInputEdit->toPlainText());
    }

    m_chat->setLastReceivedOrSentMessage((m_messageInputEdit->toPlainText().toStdString()));
}



void MessagingAreaComponent::onTypeMessage() {
    QString text = m_messageInputEdit->toPlainText();
    if (text == "") {
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

void MessagingAreaComponent::moveSliderDown(bool isCalledFromWorker) {

    if (isCalledFromWorker) {
        QCoreApplication::processEvents();
    }

    QTimer::singleShot(0, this, [this]() {
        if (m_containerWidget->layout()->count() > 0) {
            QWidget *lastMessage = m_containerWidget->layout()->itemAt(m_containerWidget->layout()->count() - 1)->widget();
            m_scrollArea->ensureWidgetVisible(lastMessage);
        }
    });
}

void MessagingAreaComponent::markMessageAsChecked(Message* message) {
    Client* client = m_chatsWidget->getClient();
    client->sendMessageReadConfirmation(m_chat->getFriendLogin(), { message });

    m_chat->getMessagesVec().back()->setIsRead(true);
}
