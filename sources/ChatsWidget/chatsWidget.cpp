#include "chatsWidget.h"
#include "ChatsListComponent.h"
#include "messagingAreaComponent.h"
#include "helloAreaComponent.h"
#include "clientSide.h"
#include "request.h"
#include "chat.h"
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
    //m_chatsListComponent->addChatComponent("qq", "zxc", m_background, m_theme);
    
    
    m_helloAreaComponent = new HelloAreaComponent(m_theme);
	m_leftVLayout->addWidget(m_chatsListComponent);
    m_mainHLayout->addLayout(m_leftVLayout);
    m_mainHLayout->addWidget(m_helloAreaComponent);
    this->setLayout(m_mainHLayout);
	

}


ChatsWidget::~ChatsWidget() {

}


void ChatsWidget::onCreateChatButtonClicked(QString login) {
    Chat* chat = nullptr;
    try {
        Chat* chatTry = m_client->createChatWith(login.toStdString());
        chat = chatTry;
    }
    catch (...) {
        auto ChatsVec = m_client->getMyChatsVec();
        ChatsVec.erase(ChatsVec.end() - 1);

        m_chatsListComponent->setAbleToCreateChatFlag(false);
        m_chatsListComponent->setRedBorderToChatAddDialog();
        return;
    }
    
    if (m_isFirstChatSet == true) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
    }
    else {
        m_mainHLayout->removeWidget(m_messagingAreaComponent);
        delete m_messagingAreaComponent;
    }
    
    m_messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat);
    m_messagingAreaComponent->setTheme(m_theme);
    m_mainHLayout->addWidget(m_messagingAreaComponent);

    if (chat->getIsFriendHasPhoto() == false) {
        m_chatsListComponent->addChatComponent(QString::fromStdString(chat->getFriendName()), "", QPixmap(), m_theme);
    }
    else {
        m_chatsListComponent->addChatComponent(QString::fromStdString(chat->getFriendName()), "", QPixmap(QString::fromStdString(chat->getFriendPhoto().getPhotoPath())), m_theme);
    }
    m_chatsListComponent->setAbleToCreateChatFlag(true);
}

void ChatsWidget::onSetChatMessagingArea(Chat* chat) {
    if (m_isFirstChatSet == true) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
        m_messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat);
        m_messagingAreaComponent->setTheme(m_theme);
        m_mainHLayout->addWidget(m_messagingAreaComponent);
    }
    else {
        m_mainHLayout->removeWidget(m_messagingAreaComponent);
        delete m_messagingAreaComponent;
        m_messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat);
        m_messagingAreaComponent->setTheme(m_theme);
        m_mainHLayout->addWidget(m_messagingAreaComponent);
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