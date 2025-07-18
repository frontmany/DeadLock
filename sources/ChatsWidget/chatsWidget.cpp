#include "theme.h"
#include "chatsWidget.h"
#include "ChatsListComponent.h"
#include "messagingAreaComponent.h"
#include "addChatDialogComponent.h"
#include "helloAreaComponent.h"
#include "chatHeaderComponent.h"
#include "configManager.h"
#include "notificationWidget.h"
#include "client.h"
#include "message.h"
#include "photo.h"
#include "utility.h"
#include "chat.h"
#include "chatComponent.h"
#include "messageComponent.h"
#include "mainWindow.h"
#include "filewrapper.h"



ChatsWidget::ChatsWidget(QWidget* parent, MainWindow* mainWindow, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme)
    : QWidget(parent), m_client(client), m_theme(theme), m_main_window(mainWindow), m_config_manager(configManager) {

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_current_messagingAreaComponent = nullptr;
    m_chatsListComponent = new ChatsListComponent(this, this, m_theme, m_client->getIsHidden());

    if (m_config_manager->getPhoto() != nullptr) {
        m_chatsListComponent->setAvatar(*m_config_manager->getPhoto());
    }

    m_leftWidget = new QWidget(this);
    m_leftVLayout = new QVBoxLayout(m_leftWidget);
    m_leftVLayout->setContentsMargins(0, 0, 0, 0);
    m_leftVLayout->addWidget(m_chatsListComponent);

    m_helloAreaComponent = new HelloAreaComponent(m_theme);


    m_splitter->addWidget(m_leftWidget);
    m_splitter->addWidget(m_helloAreaComponent);

    QList<int> sizes;
    sizes << 250 << 750;
    m_splitter->setSizes(sizes);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_splitter);
    setLayout(mainLayout);

    m_is_hello_component = true;
}

ChatsWidget::~ChatsWidget() {
    for (auto comp :  m_vec_messaging_components) {
        delete comp;
    }
}


void ChatsWidget::showNotification(Chat* chat) {
    if (isActiveWindow() && !isMinimized()) {
        return;
    }

    NotificationWidget* notification = new NotificationWidget(
        this,
        chat,
        this
    );

    notification->setTheme(m_theme);
    notification->show();
}

void ChatsWidget::onNotificationClicked(Chat* chat) {
    auto& vec = m_chatsListComponent->getChatComponentsVec();

    auto it = std::find_if(vec.begin(), vec.end(), [chat](ChatComponent* comp) {
        return comp->getChat()->getFriendLogin() == chat->getFriendLogin();
    });

    if (it == vec.end())
        return;

    ChatComponent* comp = *it;
    comp->setUnreadMessageDot(false);
    onSetChatMessagingArea(chat, comp);
    
    m_main_window->showMaximized();
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
    QList<int> sizes = m_splitter->sizes();
    
    removeRightComponent();

    auto itMsgAreaComp = std::find_if(m_vec_messaging_components.begin(), m_vec_messaging_components.end(), [chat](MessagingAreaComponent* msgComp) {
        return msgComp->getChatConst()->getFriendLogin() == chat->getFriendLogin();
        });

    if (itMsgAreaComp == m_vec_messaging_components.end()) {
        std::cout << "error can not find messaging Area Component";
        return;
    }

    m_current_messagingAreaComponent = *itMsgAreaComp;

    if (m_splitter->count() > 1) {
        QWidget* oldRight = m_splitter->widget(1);
        if (oldRight != nullptr) {
            oldRight->hide();
            oldRight->setParent(nullptr);
        }
    }

    m_splitter->addWidget(m_current_messagingAreaComponent);
    m_current_messagingAreaComponent->show();
    m_splitter->setSizes(sizes);
    m_current_messagingAreaComponent->setTheme(m_theme);

    auto scrollArea = m_current_messagingAreaComponent->getScrollArea();
    auto& unreadMessageComponentsVec = m_current_messagingAreaComponent->getUreadMessageComponents();

    bool isAtMinimum = scrollArea->verticalScrollBar()->value() == scrollArea->verticalScrollBar()->minimum();
    bool isHidden = m_client->getIsHidden();

    if (isAtMinimum) {
        scrollArea->verticalScrollBar()->setValue(scrollArea->verticalScrollBar()->maximum());

        if (!isHidden) {
            for (auto msgComp : unreadMessageComponentsVec) {
                auto message = msgComp->getMessage();
                message->setIsRead(true);
                m_client->sendMessageReadConfirmation(chat->getFriendLogin(), message);
            }
        }
    }

    if (!isHidden) {
        m_current_messagingAreaComponent->markVisibleMessagesAsChecked();
    }
    selectChatComponent(component);
}

void ChatsWidget::onSendMessageData(Message* message, Chat* chat) {
    chat->getMessagesVec().push_back(message);
    m_client->sendMessage(chat->getPublicKey(), chat->getFriendLogin(), message);
}

void ChatsWidget::onFilesData(Message* message, Chat* chat, size_t filesCount) {
    chat->getMessagesVec().push_back(message);
    m_client->sendFilesMessage(*message);
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

void ChatsWidget::onChatDelete(const QString& loginOfRemovedChat) {
    removeRightComponent();

    m_helloAreaComponent = new HelloAreaComponent(m_theme);
    m_splitter->addWidget(m_helloAreaComponent);
    m_is_hello_component = true;

    m_chatsListComponent->removeComponent(loginOfRemovedChat);

    auto it = std::find_if(m_vec_messaging_components.begin(), m_vec_messaging_components.end(), [loginOfRemovedChat](MessagingAreaComponent* msgComp) {
        return loginOfRemovedChat.toStdString() == msgComp->getChat()->getFriendLogin();
    });

    if (it != m_vec_messaging_components.end()) {
        MessagingAreaComponent* area = *it;
        area->setParent(nullptr);
        m_vec_messaging_components.erase(it);
        area->deleteLater();
        m_current_messagingAreaComponent = nullptr;
    }

    m_config_manager->deleteFriendChatInConfig(loginOfRemovedChat.toStdString());
    m_client->deleteFriendMessagesInDatabase(loginOfRemovedChat.toStdString());
    m_client->deleteFriendFromChatsMap(utility::calculateHash(loginOfRemovedChat.toStdString()));

}

void ChatsWidget::createMessagingComponent(std::string friendName, Chat* chat) {
    std::lock_guard<std::mutex> guard(m_mtx);
     m_vec_messaging_components.emplace_back(new MessagingAreaComponent(this, QString::fromStdString(friendName), m_theme, chat, this));
     m_vec_messaging_components.back()->hide();
}

void ChatsWidget::setTheme(Theme theme) {
    m_theme = theme;
    m_chatsListComponent->setTheme(theme);
    if (m_current_messagingAreaComponent != nullptr) {
        m_current_messagingAreaComponent->setTheme(m_theme);
    }
    else {
        m_helloAreaComponent->setTheme(m_theme);
    }

    update();
}

void ChatsWidget::paintEvent(QPaintEvent* event) {
    if (m_theme == DARK) {
        QPixmap background(":/resources/ChatsWidget/darkChatsBackground.jpg");
        QPainter painter(this);
        painter.fillRect(this->rect(), background);
        QWidget::paintEvent(event);
    }
    else {
        QColor backgroundColor = QColor(240, 248, 255);
        QPainter painter(this);
        painter.fillRect(this->rect(), backgroundColor);
        QWidget::paintEvent(event);
    }
}

void ChatsWidget::setClient(Client* client) {
    m_client = client;
}

void ChatsWidget::restoreMessagingAreaComponents() {
    std::lock_guard<std::mutex> guard(m_mtx);

    for (auto& chatPair : m_client->getMyHashChatsMap()) {
        MessagingAreaComponent* area = new MessagingAreaComponent(this, QString::fromStdString(chatPair.first), m_theme, chatPair.second, this);
        area->hide();
        m_vec_messaging_components.push_back(area);
    }
}

void ChatsWidget::restoreChatComponents() {
    std::lock_guard<std::mutex> guard(m_mtx);

    auto& map = m_client->getMyHashChatsMap();
    for (int i = 0; i < m_client->getMyHashChatsMap().size(); i++) {
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
        }
    }
}

bool ChatsWidget::isValidChatCreation(const std::string& loginToCheck) {
    std::string loginToCheckHash = utility::calculateHash(loginToCheckHash);
    auto& chatsMap = m_client->getMyHashChatsMap();
    auto it = std::find_if(chatsMap.begin(), chatsMap.end(), [&loginToCheckHash](std::pair<std::string, Chat*> pair) {
        return pair.first == loginToCheckHash;
    });

    if (it != m_client->getMyHashChatsMap().end() || loginToCheck == m_config_manager->getMyLogin()) {
        m_chatsListComponent->getAddChatDialogComponent()->getEditComponent()->setRedBorderToChatAddDialog();
        return false;
    }

    return true;
}

void ChatsWidget::removeRightComponent() {
    if (m_is_hello_component) {
        m_splitter->widget(1)->hide();
        m_helloAreaComponent->deleteLater();
        m_helloAreaComponent = nullptr;
        setIsHelloAreaComponent(false);
    }
    else if (m_current_messagingAreaComponent != nullptr) {
        m_splitter->widget(1)->hide();
        m_current_messagingAreaComponent->hide();
        m_current_messagingAreaComponent->setParent(nullptr);
        m_current_messagingAreaComponent = nullptr;
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