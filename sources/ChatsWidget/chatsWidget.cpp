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
#include "avatar.h"
#include "utility.h"
#include "chat.h"
#include "chatComponent.h"
#include "messageComponent.h"
#include "mainWindow.h"
#include "filewrapper.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <shellapi.h>
#endif



ChatsWidget::ChatsWidget(QWidget* parent, MainWindow* mainWindow, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme)
    : QWidget(parent), m_client(client), m_theme(theme), m_main_window(mainWindow), m_config_manager(configManager) {

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_current_messagingAreaComponent = nullptr;
    m_chatsListComponent = new ChatsListComponent(this, this, m_theme, m_client->getIsHidden());
    if (m_config_manager->getIsNeedToUpdate()) {
        m_chatsListComponent->showUpdateButton();
    }

    if (m_config_manager->getIsHasAvatar()) {
        m_chatsListComponent->setAvatar(m_config_manager->getAvatar());
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

void ChatsWidget::updateAndRestart() {
    m_config_manager->setIsNeedToUpdate(false);

#ifdef Q_OS_WIN
    DWORD pid = GetCurrentProcessId();
    QString updaterPath = "deadlock_updater.exe";
    QStringList args;
    args << QString::number(pid);

    wchar_t updaterPathW[1024];
    updaterPath.toWCharArray(updaterPathW);
    updaterPathW[updaterPath.length()] = '\0';

    QString argsStr = args.join(" ");
    wchar_t argsW[1024];
    argsStr.toWCharArray(argsW);
    argsW[argsStr.length()] = '\0';

    SHELLEXECUTEINFO sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = updaterPathW;
    sei.lpParameters = argsW;
    sei.nShow = SW_SHOWNORMAL;

    if (!ShellExecuteEx(&sei)) {
        DWORD err = GetLastError();
        if (err == ERROR_CANCELLED) {
            qWarning("User declined UAC prompt");
        }
        else {
            qWarning("Failed to start updater as admin. Error code: %lu", err);
        }
        return;
    }
#else
    QString program = "./deadlock_updater"; 
    QStringList args;
    args << QString::number(QCoreApplication::applicationPid());

    if (!QProcess::startDetached(program, args)) {
        qWarning("Failed to start updater");
        return;
    }
#endif

    QApplication::quit();
}

void ChatsWidget::showNotification(Chat* chat) {
    if (isActiveWindow() && !isMinimized()) {
        return;
    }

    NotificationWidget* notification = new NotificationWidget(
        this,
        chat,
        m_theme,
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
    auto itMsgAreaComp = std::find_if(m_vec_messaging_components.begin(), m_vec_messaging_components.end(), [chat](MessagingAreaComponent* msgComp) {
        return msgComp->getChatConst()->getFriendLogin() == chat->getFriendLogin();
        });

    if (itMsgAreaComp == m_vec_messaging_components.end()) {
        std::cout << "error can not find messaging Area Component";
        return;
    }

    MessagingAreaComponent* messagingArea = *itMsgAreaComp;
    messagingArea->setTheme(m_theme);

    QList<int> sizes = m_splitter->sizes();
    setRightComponent(messagingArea);
    m_splitter->setSizes(sizes);

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
        m_config_manager->setTheme(false);
        m_main_window->setTheme(LIGHT);
    }
    else {
        m_theme = DARK;
        setTheme(DARK);
        m_config_manager->setTheme(true);
        m_main_window->setTheme(DARK);
    }
} 

void ChatsWidget::onChatDelete(const QString& loginOfRemovedChat) {
    m_chatsListComponent->removeComponent(loginOfRemovedChat);

    std::string loginStd = loginOfRemovedChat.toStdString();
    m_client->deleteFriendMessagesInDatabase(loginStd);
    m_config_manager->deleteFriendChatInConfig(loginStd);
    m_client->deleteFriendFromChatsMap(utility::calculateHash(loginStd));

    HelloAreaComponent* helloComp = new HelloAreaComponent(m_theme);

    auto sizes = m_splitter->sizes();
    setRightComponent(helloComp);
    m_splitter->setSizes(sizes);

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
        area->setTheme(m_theme);
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
    if (m_splitter->count() > 1) {
        QWidget* oldWidget = m_splitter->widget(1);
        oldWidget->hide();

        if (m_is_hello_component) {
            oldWidget->deleteLater();
            m_is_hello_component = false;
            m_helloAreaComponent = nullptr;
        }
    }
}

void ChatsWidget::setRightComponent(std::variant<MessagingAreaComponent*, HelloAreaComponent*> rightComponentVariant) {
    removeRightComponent();

    if (std::holds_alternative<MessagingAreaComponent*>(rightComponentVariant)) {
        m_current_messagingAreaComponent = std::get<MessagingAreaComponent*>(rightComponentVariant);
        m_splitter->insertWidget(1, m_current_messagingAreaComponent);
        m_current_messagingAreaComponent->show();
    }
    else if (std::holds_alternative<HelloAreaComponent*>(rightComponentVariant)) {
        m_helloAreaComponent = std::get<HelloAreaComponent*>(rightComponentVariant);
        m_current_messagingAreaComponent = nullptr;
        m_is_hello_component = true;
        m_splitter->insertWidget(1, m_helloAreaComponent);
        m_helloAreaComponent->show();
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

    MessagingAreaComponent* messagingAreaComponent = new MessagingAreaComponent(
        this,
        QString::fromStdString(chat->getFriendName()),
        theme,
        chat,
        this
    );
    messagingAreaComponent->setTheme(m_theme);

    auto sizes = m_splitter->sizes();
    setRightComponent(messagingAreaComponent);
    m_splitter->setSizes(sizes);

    getMessagingAreasVec().push_back(messagingAreaComponent);
}

void ChatsWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);

    const int minWindowWidth = 600;

    const int leftWidgetMinimumWidth = 250;

    const int rightWidgetMinimumWidth = 350;


    QList<int> currentSizes = m_splitter->sizes();

    if (width() < minWindowWidth) {

        if (currentSizes[0] > 0) {

            QList<int> newSizes;
            newSizes << 0 << width(); 
            m_splitter->setSizes(newSizes);
        }
    }
    else {
        if (currentSizes[0] == 0) {
            QList<int> newSizes;
            int leftWidth = qMax(leftWidgetMinimumWidth, width() - rightWidgetMinimumWidth);
            newSizes << leftWidth << width() - leftWidth;
            m_splitter->setSizes(newSizes);
        }
        else {
            int totalWidth = currentSizes[0] + currentSizes[1];
            if (totalWidth > 0) {
                double ratio = static_cast<double>(currentSizes[0]) / totalWidth;
                int newLeftWidth = qMax(leftWidgetMinimumWidth, static_cast<int>(width() * ratio));
                QList<int> newSizes;
                newSizes << newLeftWidth << width() - newLeftWidth;
                m_splitter->setSizes(newSizes);
            }
        }
    }
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