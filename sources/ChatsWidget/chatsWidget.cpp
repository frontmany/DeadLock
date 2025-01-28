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
    m_messagingAreaComponent = nullptr;
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
        m_mainHLayout->removeWidget(m_messagingAreaComponent);
        delete m_messagingAreaComponent;

    }

    m_messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat);
    m_messagingAreaComponent->setTheme(m_theme);
    m_mainHLayout->addWidget(m_messagingAreaComponent);

    m_chatsListComponent->addChatComponent(m_theme, chat);
    m_chatsListComponent->closeAddChatDialog();
}

void ChatsWidget::onSetChatMessagingArea(Chat* chat, ChatComponent* component) {
    if (m_isFirstChatSet == true) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
        m_messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat);
        m_messagingAreaComponent->setTheme(m_theme);
        m_mainHLayout->addWidget(m_messagingAreaComponent);

        for (auto chatComp : m_chatsListComponent->getChatComponentsVec()) {
            chatComp->setSelected(false);
        }

        component->setSelected(true);
        m_isFirstChatSet = false;
    }
    else {
        m_mainHLayout->removeWidget(m_messagingAreaComponent);
        delete m_messagingAreaComponent;
        m_messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat);
        m_messagingAreaComponent->setTheme(m_theme);
        m_mainHLayout->addWidget(m_messagingAreaComponent);

        for (auto chatComp : m_chatsListComponent->getChatComponentsVec()) {
            chatComp->setSelected(false);
        }
        component->setSelected(true);
    }
    
   
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