#include "chatsWidget.h"
#include "ChatsListComponent.h"
#include "messagingAreaComponent.h"
#include "addChatDialogComponent.h"
#include "helloAreaComponent.h"
#include "chatHeaderComponent.h"
#include "client.h"
#include "utility.h"
#include "message.h"
#include "chat.h"
#include "chatComponent.h"
#include "mainWindow.h"


ChatsWidget::ChatsWidget(QWidget* parent, MainWindow* mainWindow, Client* client, Theme theme) 
    : QWidget(parent), m_client(client), m_theme(theme), m_main_window(mainWindow) {

    m_mainHLayout = new QHBoxLayout;
    m_mainHLayout->setAlignment(Qt::AlignLeft);


    m_background.load(":/resources/LoginWidget/lightLoginBackground.jpg");
    m_current_messagingAreaComponent = nullptr;
	m_chatsListComponent = new ChatsListComponent(this, this, m_theme);

    if (m_client->getPhoto() != nullptr) {
        m_chatsListComponent->SetAvatar(*m_client->getPhoto());
    }

    m_leftVLayout = new QVBoxLayout;
    m_leftVLayout->addWidget(m_chatsListComponent);

    m_helloAreaComponent = new HelloAreaComponent(m_theme);
    m_mainHLayout->addLayout(m_leftVLayout);
    m_mainHLayout->addWidget(m_helloAreaComponent);
    this->setLayout(m_mainHLayout);
}

ChatsWidget::~ChatsWidget() {
    for (auto comp :  m_vec_messaging_components) {
        delete comp;
    }
}

MainWindow* ChatsWidget::getMainWindow() {
    return m_main_window;
}

void ChatsWidget::onCreateChatButtonClicked(QString login) {
    bool isValidCreation = isValidChatCreation(login.toStdString());
    if (isValidCreation) {
        m_client->createChatWith(login.toStdString());
    }
}

void ChatsWidget::onSetChatMessagingArea(Chat* chat, ChatComponent* component) {
    removeRightComponent();

    auto itMsgComp = std::find_if( m_vec_messaging_components.begin(),  m_vec_messaging_components.end(), [chat](MessagingAreaComponent* msgComp) {
        return msgComp->getChatConst()->getFriendLogin() == chat->getFriendLogin();
        });

    if (itMsgComp ==  m_vec_messaging_components.end()) {
        std::cout << "error can not find messaging Area Component";
    }

    else {
        m_current_messagingAreaComponent = *itMsgComp;
        m_current_messagingAreaComponent->show();
        m_current_messagingAreaComponent->setTheme(m_theme);
        m_mainHLayout->addWidget(m_current_messagingAreaComponent);
        m_current_messagingAreaComponent->getScrollArea()->verticalScrollBar()->setValue(m_current_messagingAreaComponent->getScrollArea()->verticalScrollBar()->maximum());

        auto unreadVec = chat->getUnreadReceiveMessagesVec();
        if (unreadVec.size() > 0) {
            for (auto msg : unreadVec) {
                m_client->sendMessageReadConfirmation(chat->getFriendLogin(), msg);
                msg->setIsRead(true);
            }
        }

    }
    selectChatComponent(component);
}

void ChatsWidget::onSendMessageData(Message* message, Chat* chat) {
    chat->getMessagesVec().push_back(message);
    chat->setLastReceivedOrSentMessage(message->getMessage());
    m_client->sendMessage(chat->getFriendLogin(), message);
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

void ChatsWidget::createMessagingComponent(std::string friendName, Chat* chat) {
    std::lock_guard<std::mutex> guard(m_mtx);
     m_vec_messaging_components.emplace_back(new MessagingAreaComponent(nullptr, QString::fromStdString(friendName), m_theme, chat, this));
     m_vec_messaging_components.back()->hide();
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

void ChatsWidget::restoreMessagingAreaComponents() {
    std::lock_guard<std::mutex> guard(m_mtx);

    for (auto& chatPair : m_client->getMyChatsMap()) {
        MessagingAreaComponent* area = new MessagingAreaComponent(this, QString::fromStdString(chatPair.first), m_theme, chatPair.second, this);
        area->hide();
         m_vec_messaging_components.push_back(area);
    }
}

void ChatsWidget::restoreChatComponents() {
    std::lock_guard<std::mutex> guard(m_mtx);

    auto& map = m_client->getMyChatsMap();
    for (int i = 0; i < m_client->getMyChatsMap().size(); i++) {
        auto it = std::find_if(map.begin(), map.end(), [i](std::pair<std::string, Chat*> chatPair) {
            return chatPair.second->getLayoutIndex() == i;
            });
        
        std::pair<std::string, Chat*> chatPair = *it;

        m_chatsListComponent->addChatComponent(m_theme, chatPair.second, false);
        ChatComponent* comp = m_chatsListComponent->getChatComponentsVec().back();
        auto& vec = chatPair.second->getMessagesVec();
        if (vec.size() == 0) {
            comp->setUnreadMessageDot(false);
            comp->setLastMessage("no messages yet");
        }
        else {
            comp->setUnreadMessageDot(!vec.back()->getIsRead() && !vec.back()->getIsSend());
            comp->setLastMessage(QString::fromStdString(vec.back()->getMessage()));
        }
    }
}

bool ChatsWidget::isValidChatCreation(const std::string& loginToCheck) {
    auto& chatsMap = m_client->getMyChatsMap();
    auto it = std::find_if(chatsMap.begin(), chatsMap.end(), [&loginToCheck](std::pair<std::string, Chat*> pair) {
        return pair.first == loginToCheck;
        });

    if (it != m_client->getMyChatsMap().end() || loginToCheck == m_client->getMyLogin()) {
        m_chatsListComponent->getAddChatDialogComponent()->getEditComponent()->setRedBorderToChatAddDialog();
        return false;
    }

    return true;
}

void ChatsWidget::removeRightComponent() {
    if (m_is_hello_component) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
        setIsHelloAreaComponent(false);
    }
    else {
        m_mainHLayout->removeWidget(m_current_messagingAreaComponent);
        m_current_messagingAreaComponent->hide();
    }
}

void ChatsWidget::selectChatComponent(ChatComponent* component) {
    for (auto chatComp : m_chatsListComponent->getChatComponentsVec()) {
        chatComp->setSelected(false);
    }
    component->setSelected(true);
}



void ChatsWidget::createAndSetMessagingAreaComponent(Chat* chat) {
    Theme theme = getTheme();
    QHBoxLayout* mainHLayout = getMainHLayout();

    MessagingAreaComponent* messagingAreaComponent = new MessagingAreaComponent(
        this,
        QString::fromStdString(chat->getFriendName()),
        theme,
        chat,
        this
    );

    setCurrentMessagingAreaComponent(messagingAreaComponent);
    messagingAreaComponent->setTheme(theme);

    mainHLayout->addWidget(messagingAreaComponent);

    getMessagingAreasVec().push_back(messagingAreaComponent);
}

void ChatsWidget::createAndAddChatComponentToList(Chat* chat) {
    Theme theme = getTheme();
    ChatsListComponent* chatsListComponent = getChatsList();

    chatsListComponent->addChatComponent(theme, chat, true);
}

void ChatsWidget::closeAddChatDialog() {
    ChatsListComponent* chatsListComponent = getChatsList();
    chatsListComponent->closeAddChatDialog();
}