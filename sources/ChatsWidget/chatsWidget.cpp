#include "chatsWidget.h"
#include "ChatsListComponent.h"
#include "messagingAreaComponent.h"
#include "addChatDialogComponent.h"
#include "helloAreaComponent.h"
#include "chatHeaderComponent.h"
#include "clientSide.h"
#include "request.h"
#include "chat.h"
#include "chatComponent.h"
#include"mainWindow.h"


ChatsWidget::ChatsWidget(QWidget* parent, ClientSide* client, Theme theme) 
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

}


void ChatsWidget::onCreateChatButtonClicked(QString login) {
    QString login2 = login;
    Chat* chat = nullptr;
    try {
        chat = m_client->createChatWith(login2.toStdString());
    }
    catch (...) {
        auto ChatsVec = m_client->getMyChatsVec();
        ChatsVec.erase(ChatsVec.end() - 1);
        m_chatsListComponent->getAddChatDialogComponent()->getEditComponent()->setRedBorderToChatAddDialog();
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
        m_current_messagingAreaComponent->show();
        m_mainHLayout->addWidget(m_current_messagingAreaComponent);
    }

    for (auto chatComp : m_chatsListComponent->getChatComponentsVec()) {
        chatComp->setSelected(false);
    }
    component->setSelected(true);
    
    

    //TODO direct Notification about messages was read
}

void ChatsWidget::onSendMessageData(const QString& message, const QString& timeStamp, Chat* chat, double id) {
    Msg* msg = new Msg;
    msg->setId(id);
    msg->setIsSend(true);
    msg->setMessage(message.toStdString());
    msg->setTimestamp(timeStamp.toStdString());
    m_client->sendMessage(chat, message.toStdString(), timeStamp.toStdString(), id);
}

void ChatsWidget::setTheme(Theme theme) {
    m_theme = theme;
    setBackGround(theme);
    m_chatsListComponent->setTheme(theme);
}


void ChatsWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawPixmap(this->rect(), m_background);
    QWidget::paintEvent(event);
}


void ChatsWidget::setBackGround(Theme theme) {
    if (theme == DARK) {
        if (m_background.load(":/resources/ChatsWidget/darkChatsBackground.jpg")) {
        }
    }
    else {
        if (m_background.load(":/resources/ChatsWidget/lightChatsBackground.jpg")) {
        }
    }
}