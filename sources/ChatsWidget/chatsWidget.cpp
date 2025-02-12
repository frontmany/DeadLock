#include "chatsWidget.h"
#include "ChatsListComponent.h"
#include "messagingAreaComponent.h"
#include "addChatDialogComponent.h"
#include "helloAreaComponent.h"
#include "chatHeaderComponent.h"
#include "client.h"
#include "chat.h"
#include "chatComponent.h"
#include"mainWindow.h"


ChatsWidget::ChatsWidget(QWidget* parent, Client* client, Theme theme) 
    : QWidget(parent), m_client(client), m_theme(theme) {

    m_mainHLayout = new QHBoxLayout;
    m_mainHLayout->setAlignment(Qt::AlignLeft);

	m_leftVLayout = new QVBoxLayout;
    m_background.load(":/resources/LoginWidget/lightLoginBackground.jpg");
    m_current_messagingAreaComponent = nullptr;
    m_isFirstChatSet = true;
	m_chatsListComponent = new ChatsListComponent(this, this, m_theme);
    
    
    m_helloAreaComponent = new HelloAreaComponent(m_theme);
	m_leftVLayout->addWidget(m_chatsListComponent);
    m_mainHLayout->addLayout(m_leftVLayout);
    m_mainHLayout->addWidget(m_helloAreaComponent);
    this->setLayout(m_mainHLayout);
	
}



ChatsWidget::~ChatsWidget() {
    for (auto comp : m_vec_messagingComponents_cache) {
        delete comp;
    }
}


void ChatsWidget::onCreateChatButtonClicked(QString login) {
    OperationResult res = m_client->createChatWith(login.toStdString());
    if (res == OperationResult::FAIL) {
        m_chatsListComponent->getAddChatDialogComponent()->getEditComponent()->setRedBorderToChatAddDialog();
        return;
    }
    else if (res == OperationResult::REQUEST_TIMEOUT){
        m_chatsListComponent->getAddChatDialogComponent()->getEditComponent()->setRedBorderToChatAddDialog();
        //TODO REQUESTTIMEOUT DIALOG
        return;
    }
    
    if (m_isFirstChatSet == true) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
        m_isFirstChatSet = false;
    }
    else {
        m_mainHLayout->removeWidget(m_current_messagingAreaComponent);
        m_current_messagingAreaComponent->hide();
        m_vec_messagingComponents_cache.push_back(m_current_messagingAreaComponent);
    }

    Chat* chat = m_client->getMyChatsMap()[login.toStdString()];

    auto messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat, this);
    m_current_messagingAreaComponent = messagingAreaComponent;
    m_current_messagingAreaComponent->setTheme(m_theme);
    m_vec_messagingComponents_cache.push_back(m_current_messagingAreaComponent);
    m_mainHLayout->addWidget(m_current_messagingAreaComponent);

    m_chatsListComponent->addChatComponent(m_theme, chat);
    m_chatsListComponent->closeAddChatDialog();
}

void ChatsWidget::onSetChatMessagingArea(Chat* chat, ChatComponent* component) {
    if (m_isFirstChatSet == true) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
        m_isFirstChatSet = false;
    }
    else {
        m_mainHLayout->removeWidget(m_current_messagingAreaComponent);
        m_current_messagingAreaComponent->hide();
    }

    auto itMsgComp = std::find_if(m_vec_messagingComponents_cache.begin(), m_vec_messagingComponents_cache.end(), [chat](MessagingAreaComponent* msgComp) {
        return msgComp->getChatConst()->getFriendLogin() == chat->getFriendLogin();
        });

    if (itMsgComp == m_vec_messagingComponents_cache.end()) {
        std::cout << "error can not find mesaging Area Component";
    }
    else {
        m_current_messagingAreaComponent = *itMsgComp;
        m_current_messagingAreaComponent->setTheme(m_theme);
        m_current_messagingAreaComponent->show();
        m_mainHLayout->addWidget(m_current_messagingAreaComponent);

        auto unreadVec = chat->getUnreadReceiveMessagesVec();
        if (unreadVec.size() > 0) {
            m_client->sendMessageReadConfirmation(chat->getFriendLogin(), unreadVec);
        }

    }

    for (auto chatComp : m_chatsListComponent->getChatComponentsVec()) {
        chatComp->setSelected(false);
    }
    component->setSelected(true);
}

void ChatsWidget::onSendMessageData(Message* message, Chat* chat) {
    auto& chatsComponentsVec = m_chatsListComponent->getChatComponentsVec();
    auto itComponentsVec = std::find_if(chatsComponentsVec.begin(), chatsComponentsVec.end(), [chat](ChatComponent* chatComponent) {
        return chat->getFriendLogin() == chatComponent->getChatConst()->getFriendLogin();
        });

    ChatComponent* comp = *itComponentsVec;
    if (message->getMessage().length() > 15) {
        std::string s = message->getMessage().substr(0, 15) + "...";
        comp->setLastMessage(QString::fromStdString(s), false);
    }
    else {
        comp->setLastMessage(QString::fromStdString(message->getMessage()), false);
    }

    m_client->sendMessage(chat->getFriendLogin(), message->getMessage(), message->getTimestamp());
}

void ChatsWidget::onChangeThemeClicked() {
    if (m_theme == DARK) {
        m_theme = LIGHT;
        setTheme(LIGHT);
    }
    else {
        m_theme = DARK;
        setTheme(DARK);
    }
    
}

void ChatsWidget::setTheme(Theme theme) {
    m_theme = theme;
    setBackGround(theme);
    m_chatsListComponent->setTheme(theme);
    if (m_current_messagingAreaComponent != nullptr) {
        m_current_messagingAreaComponent->setTheme(m_theme);
    }
    else {
        m_helloAreaComponent->setTheme(m_theme);
    }
}


void ChatsWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawPixmap(this->rect(), m_background);
    QWidget::paintEvent(event);
}

/*
void ChatsWidget::createMessagingAreaFromClientSide(QString message, QString timeStamp, Chat* chat, double id) {
    MessagingAreaComponent* newComp = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat, this);
    newComp->setTheme(m_theme);
    Message* m = new Message(message.toStdString(), timeStamp.toStdString(), )
    newComp->addMessage(message, timeStamp, id);
    m_vec_messagingComponents_cache.push_back(newComp);
}
*/

void ChatsWidget::setBackGround(Theme theme) {
    if (theme == DARK) {
        if (m_background.load(":/resources/ChatsWidget/darkChatsBackground.jpg")) {
        }
    }
    else {
        if (m_background.load(":/resources/ChatsWidget/lightChatsBackground.jpg")) {
        }
    }
    update();
}


void ChatsWidget::setClient(Client* client) {
    m_client = client;
}

void ChatsWidget::load() {
    for (auto chatPair : m_client->getMyChatsMap()) {
        m_chatsListComponent->addChatComponent(m_theme, chatPair.second);
        MessagingAreaComponent* area = new MessagingAreaComponent(this, QString::fromStdString(chatPair.first), m_theme, chatPair.second, this);
        m_vec_messagingComponents_cache.push_back(area);
    }
}