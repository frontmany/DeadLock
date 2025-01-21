#include "chatsWidget.h"
#include "ChatsListComponent.h"
#include "clientSide.h"
#include"mainWindow.h"


ChatsWidget::ChatsWidget(QWidget* parent, ClientSide* client) 
    : QWidget(parent), m_client(client) {

	m_mainVLayout = new QVBoxLayout;
    m_background.load(":/resources/LoginWidget/lightLoginBackground.jpg");
	m_chatsListComponent = new ChatsListComponent(this, this);
    m_chatsListComponent->addChatComponent("qq", "zxc", m_background);


	m_mainVLayout->addWidget(m_chatsListComponent);
	this->setLayout(m_mainVLayout);
}


ChatsWidget::~ChatsWidget() {

}

void ChatsWidget::onCreateChatButtonClicked(QString login) {
    if (m_client->createChatWith(login.toStdString())) {
        req::Packet p = m_client->getUserDataByLogin(login.toStdString());
        m_chatsListComponent->addChatComponent(QString::fromStdString(p.sender.name), "", QPixmap());
        m_chatsListComponent->setAbleToCreateChatFlag(true);
    }
    else {
        m_chatsListComponent->setAbleToCreateChatFlag(false);
        m_chatsListComponent->setRedBorderToChatAddDialog();
    }
}


void ChatsWidget::setTheme(Theme theme) {
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