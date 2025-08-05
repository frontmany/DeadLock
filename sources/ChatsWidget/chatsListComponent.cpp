#include "theme.h"
#include "chatsListComponent.h"
#include "addChatDialogComponent.h"
#include "fieldsEditComponent.h"
#include "chatsWidget.h"
#include "messagingAreaComponent.h"
#include "friendSearchDialogComponent.h"
#include "messageComponent.h"
#include "overlayWidget.h"
#include "mainwindow.h"
#include "chatHeaderComponent.h"
#include "buttons.h"
#include "avatar.h"
#include "utility.h"
#include "client.h"
#include "chat.h"
#include "chatComponent.h"
#include "profileEditorWidget.h"

#include <QPainter>
#include <QPaintEvent>

StyleChatsListComponent::StyleChatsListComponent() {
    DarkSlider = R"(
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

    LightSlider = R"(
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

    DarkLineEditStyle = R"(
    QLineEdit {
        background-color: #333;    
        color: white;               
        border: none;     
        border-radius: 14px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid #888;     
    }
)";

    LightLineEditStyle = R"(
    QLineEdit {
        background-color: #ffffff;    
        color: black;                 
        border: none;       
        border-radius: 14px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: 2px solid rgb(237, 237, 237);        
    }
)";

    TransparentButtonStyle = R"(
    QPushButton {
        background-color: transparent;   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";

    DialogStyle = R"(
    QLineEdit {
        background-color: #ffffff;                    
        border: none;       
        border-radius: 15px;           
        padding: 5px;                 
    }
)";

    DarkHideButton = R"(
QPushButton {
    background-color: rgb(25, 25, 25);
    border-radius: 15px;
    padding: 8px 16px;
    color: #E0E0E0;
    font-family: 'Segoe UI', sans-serif;
    font-size: 14px;
    font-weight: 800;
}

QPushButton:hover {
    background-color: rgb(35, 35, 35);
}

QPushButton:pressed {
    color: rgb(100, 180, 255);
    background-color: rgba(26, 133, 255, 0.15);
}

QPushButton:checked {
    background-color: rgba(26, 133, 255, 0.15);
    color: rgb(100, 180, 255);
}

QPushButton:checked:hover {
    background-color: rgba(0, 138, 255, 0.18);
}

QPushButton:disabled {
    background-color: rgba(80, 80, 80, 0.3);
    color: #555555;
}
)";

    LightHideButton = R"(
QPushButton {
    background-color: rgb(240, 238, 237);
    border-radius: 15px;
    padding: 8px 16px;
    color: #100C08;
    font-family: 'Segoe UI', sans-serif;
    font-size: 14px;
    font-weight: 800;
}

QPushButton:hover {
    background-color: rgb(245, 245, 245);
}

QPushButton:pressed {
    background-color: #D1E3F6;
    color: #2A5885;
}

QPushButton:checked {
    background-color: #D1E3F6;
    color: #2A5885;
}

QPushButton:checked:hover {
    background-color: #C5DDF4;
}

QPushButton:disabled {
    background-color: #F0F0F0;
    color: #A0A0A0;
}
)";

    DarkNoConnectionLabelStyle = R"(
QLabel {
    background-color: #D94A4A;        
    color: #FFF0F0;                   
    font-size: 14px;
    font-weight: 600;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    padding: 10px 16px;
    border-radius: 15px;
    qproperty-alignment: 'AlignCenter';
}
)";

    LightNoConnectionLabelStyle = R"(
QLabel {
    background-color: #FFD6D6;       
    color: #B22222;                    
    font-size: 14px;
    font-weight: 600;
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    padding: 10px 16px;
    border-radius: 15px;
    qproperty-alignment: 'AlignCenter';
}
)";

    updateButtonDark = R"(
QPushButton {
    background-color: rgba(26, 133, 255, 0.15); 
    color: rgb(100, 180, 255);
    border: none;
    font-weight: 600;
    font-size: 14px;
    padding: 5px 12px;
    border-radius: 5px;
}

QPushButton:hover {
    background-color: rgba(26, 133, 255, 0.30); 
    color: rgb(26, 133, 255);
}

QPushButton:pressed {
    background-color: rgba(0, 102, 204, 0.40);   
    color: rgb(0, 102, 204);
}

QPushButton:disabled {
    background-color: rgba(80, 80, 80, 0.3);
    color: #555555;
}
)";


    updateButtonLight = R"(
QPushButton {
    background-color: rgba(0, 102, 204, 0.15);  
    color: rgb(0, 102, 204);
    border: none;
    font-weight: 600;
    font-size: 14px;
    padding: 5px 12px;
    border-radius: 5px;
}

QPushButton:hover {
    background-color: rgba(26, 133, 255, 0.30);
    color: rgb(26, 133, 255);
}

QPushButton:pressed {
    background-color: rgba(0, 76, 153, 0.40);
    color: rgb(0, 76, 153);
}

QPushButton:disabled {
    background-color: rgba(200, 200, 200, 0.3);
    color: #A0A0A0;
}
)";

    updateButtonInProgressDark = R"(
QPushButton {
    background-color: rgba(26, 133, 255, 0.10); 
    color: rgba(100, 180, 255, 0.7);
    border: none;
    font-weight: 600;
    font-size: 14px;
    padding: 5px 12px;
    border-radius: 5px;
}

QPushButton:hover {
    background-color: rgba(26, 133, 255, 0.10);
    color: rgba(100, 180, 255, 0.7);
}

QPushButton:pressed {
    background-color: rgba(26, 133, 255, 0.10);
    color: rgba(100, 180, 255, 0.7);
}
)";

    updateButtonInProgressLight = R"(
QPushButton {
    background-color: rgba(0, 102, 204, 0.10);  
    color: rgba(0, 102, 204, 0.7);
    border: none;
    font-weight: 600;
    font-size: 14px;
    padding: 5px 12px;
    border-radius: 5px;
}

QPushButton:hover {
    background-color: rgba(0, 102, 204, 0.10);
    color: rgba(0, 102, 204, 0.7);
}

QPushButton:pressed {
    background-color: rgba(0, 102, 204, 0.10);
    color: rgba(0, 102, 204, 0.7);
}
)";
}

ChatsListComponent::ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme, bool isHidden)
    : QWidget(parent),
    m_chatsWidget(chatsWidget), m_chatAddDialog(nullptr), m_chats_widget(chatsWidget),
    m_profile_editor_widget(nullptr), m_is_hidden(isHidden)
{
    style = new StyleChatsListComponent;
    m_theme = theme;

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);

    setMinimumWidth(300);

    m_profileHLayout = new QHBoxLayout();
    m_profileHLayout->addSpacing(20);
    m_profileHLayout->setAlignment(Qt::AlignLeft);

    m_profileButton = new AvatarIcon(this, 32, 50, true, m_theme);
    QIcon avatarIcon(":/resources/ChatsWidget/userFriend.png");
    m_profileButton->setIcon(avatarIcon);
    m_profileHLayout->addWidget(m_profileButton);

    m_newChatButton = new ButtonIcon(this, 50, 50);
    QIcon icon3(":/resources/ChatsWidget/startChatDark.png");
    QIcon iconHover3(":/resources/ChatsWidget/startChatHoverDark.png");
    m_newChatButton->uploadIconsDark(icon3, iconHover3);
    QIcon icon4(":/resources/ChatsWidget/startChatLight.png");
    QIcon iconHover4(":/resources/ChatsWidget/startChatHoverLight.png");
    m_newChatButton->uploadIconsLight(icon4, iconHover4);
    m_profileHLayout->addWidget(m_newChatButton);

    m_hideButton = new QPushButton(this);
    m_hideButton->setObjectName("hideButton");
    m_hideButton->setCursor(Qt::PointingHandCursor);
    m_hideButton->setCheckable(true);
    m_hideButton->setFixedSize(125, 32);
    m_hideButton->setIconSize(QSize(32, 32));
    m_hideButton->installEventFilter(this);

    m_updateButton = new QPushButton(this);
    m_updateButton->setFixedSize(245, 32);
    m_updateButton->hide();
    m_profileHLayout->addWidget(m_updateButton);
    connect(m_updateButton, &QPushButton::clicked, [this]() {
        m_chatsWidget->getClient()->requestUpdate();
        m_updateButton->setText("Quantum Core Loading...");
        m_isUpdateLoading = true;
        if (m_theme == DARK) {
            m_updateButton->setStyleSheet(style->updateButtonInProgressDark);
        }
        else {
            m_updateButton->setStyleSheet(style->updateButtonInProgressLight);
        }
    });

    if (m_is_hidden) {
        m_hideButton->click();
    }
    updateHideButton();

    connect(m_hideButton, &QPushButton::toggled, [this](bool checked) {onHideButtonToggled(checked, false); });
    m_profileHLayout->addWidget(m_hideButton);

    m_connectionDownLabel = new QLabel;
    m_connectionDownLabel->hide();
    m_connectionDownLabel->setFixedSize(255, 36);
    m_profileHLayout->addWidget(m_connectionDownLabel);


    m_reconnectButton = new ButtonIcon(this, 32, 32);
    QIcon reconnectIconDark(":/resources/ChatsWidget/reloadDark.png");
    QIcon reconnectIconDarkHover(":/resources/ChatsWidget/reloadDarkHover.png");
    m_reconnectButton->uploadIconsDark(reconnectIconDark, reconnectIconDarkHover);
    QIcon reconnectIconLight(":/resources/ChatsWidget/reloadLight.png");
    QIcon reconnectIconLightHover(":/resources/ChatsWidget/reloadLightHover.png");
    m_reconnectButton->uploadIconsLight(reconnectIconLight, reconnectIconLightHover);
    m_reconnectButton->setTheme(m_theme);
    connect(m_reconnectButton, &ButtonIcon::clicked, [this]() {
        m_reconnectButton->hide();
        m_reconnectionGif->show();
        m_chatsWidget->getClient()->tryReconnect();
    });
    m_reconnectButton->setIconSize(QSize(24, 24));
    m_reconnectButton->hide();

    m_profileHLayout->addWidget(m_reconnectButton);


    m_reconnectionGif = new QLabel(this);
    QMovie* movie = new QMovie(":/resources/ChatsWidget/process.gif"); 
    m_reconnectionGif->setMovie(movie);
    movie->start();
    m_reconnectionGif->hide();

    m_profileHLayout->addWidget(m_reconnectionGif);

    m_darkModeSwitch = new ToggleSwitch(this, m_theme);
    m_darkModeSwitch->setTheme(m_theme);
    QObject::connect(m_darkModeSwitch, &ToggleSwitch::toggled, this, &ChatsListComponent::toSendChangeTheme);
    m_profileHLayout->addSpacing(1500);
    m_profileHLayout->addWidget(m_darkModeSwitch);
    m_profileHLayout->addSpacing(15);

    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("  Search for friends...");

    m_search_timer = new QTimer(this);
    m_search_timer->setInterval(500);
    m_search_timer->setSingleShot(true);

    connect(m_searchLineEdit, &QLineEdit::textChanged, [this]() {
        std::string text = m_searchLineEdit->text().trimmed().toStdString();
        if (text == "") {
            m_friend_search_dialog->closeDialog();
        }
        else {
            m_search_timer->start();
        }
    });

    connect(m_search_timer, &QTimer::timeout, [this]() {
        if (m_friend_search_dialog) {
            std::string text = m_searchLineEdit->text().trimmed().toStdString();
            if (text != "" && text.find_first_not_of(' ') != std::string::npos) {
                m_chatsWidget->getClient()->findUser(text);
            }
            else if (text == ""){
                m_friend_search_dialog->hide();
            }
        }
    });

    m_friend_search_dialog = new FriendSearchDialogComponent(this, this, m_theme);
    m_friend_search_dialog->hide();

    m_contentsHLayout = new QHBoxLayout();
    m_contentsHLayout->addSpacing(10);
    m_contentsHLayout->setAlignment(Qt::AlignLeft);
    m_contentsHLayout->addWidget(m_searchLineEdit);
    m_contentsHLayout->addSpacing(15);

    m_mainVLayout->addLayout(m_profileHLayout);
    m_mainVLayout->addSpacing(10);
    m_mainVLayout->addLayout(m_contentsHLayout);
    m_mainVLayout->addWidget(m_friend_search_dialog);
    m_mainVLayout->addSpacing(4);

    m_containerWidget = new QWidget();
    m_containerVLayout = new QVBoxLayout(m_containerWidget);
    m_containerVLayout->setAlignment(Qt::AlignTop);
    m_containerWidget->setLayout(m_containerVLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setStyleSheet("background: transparent;");
    m_scrollArea->setWidget(m_containerWidget);

    m_mainVLayout->addWidget(m_scrollArea);

    m_profile_editor_widget = new ProfileEditorWidget(this, this, m_chatsWidget->getClient(), m_chatsWidget->getConfigManager(), m_theme);
    m_profile_editor_widget->hide();

    connect(m_profileButton, &AvatarIcon::clicked, this, &ChatsListComponent::openEditUserDialogWidnow);
    connect(m_newChatButton, &ButtonIcon::clicked, this, &ChatsListComponent::openAddChatDialog);
    connect(this, &ChatsListComponent::sendCreateChatData, m_chatsWidget, &ChatsWidget::onCreateChatButtonClicked);
    connect(this, &ChatsListComponent::sendChangeTheme, m_chatsWidget, &ChatsWidget::onChangeThemeClicked);
}

void ChatsListComponent::onHideButtonToggled(bool checked, bool isNeedToToggleButton)
{
    if (isNeedToToggleButton) {
        m_hideButton->click();
        return;
    }

    m_is_hidden = checked;
    auto client = m_chatsWidget->getClient();
    client->setIsHidden(checked);
    updateHideButton();

    auto currentMessagingAreaComponent = m_chatsWidget->getCurrentMessagingAreaComponent();
    if (currentMessagingAreaComponent != nullptr) {
        currentMessagingAreaComponent->markVisibleMessagesAsChecked();
    }

    if (m_is_hidden) {
        enableHiddenMode(client);
    }
    else {
        disableHiddenMode(client);
    }
}

void ChatsListComponent::enableHiddenMode(Client* client)
{
    auto messagingAreasVec = m_chatsWidget->getMessagingAreasVec();
    for (auto comp : messagingAreasVec) {
        comp->getTextEdit()->setDisabled(true);
        comp->getAttachFileButton()->setDisabled(true);
        comp->getChatPropertiesComponent()->disable(true);
        comp->hideSendMessageButton();
    }

    m_profileButton->setDisabled(true);
    client->broadcastMyStatus(utility::getCurrentFullDateAndTime());
}

void ChatsListComponent::disableHiddenMode(Client* client)
{
    auto messagingAreasVec = m_chatsWidget->getMessagingAreasVec();
    for (auto comp : messagingAreasVec) {
        comp->getTextEdit()->setDisabled(false);
        comp->getAttachFileButton()->setDisabled(false);
        comp->getChatPropertiesComponent()->disable(false);
    }
    m_profileButton->setDisabled(false);
    client->broadcastMyStatus("online");
}

bool ChatsListComponent::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_hideButton) {
        if (event->type() == QEvent::Enter) {
            if (m_theme == DARK) {
                if (m_is_hidden) {
                    m_hideButton->setIcon(m_hiddenIconDarkHover);
                }
                else {
                    m_hideButton->setIcon(m_visibleIconDarkHover);
                }
            }
            else {
                if (m_is_hidden) {
                    m_hideButton->setIcon(m_hiddenIconLightHover);
                }
                else {
                    m_hideButton->setIcon(m_visibleIconLightHover);
                }
            }

            return true;
        }
        else if (event->type() == QEvent::Leave) {
            if (m_theme == DARK) {
                if (m_is_hidden) {
                    m_hideButton->setIcon(m_hiddenIconDark);
                }
                else {
                    m_hideButton->setIcon(m_visibleIconDark);
                }
            }
            else {
                if (m_is_hidden) {
                    m_hideButton->setIcon(m_hiddenIconLight);
                }
                else {
                    m_hideButton->setIcon(m_visibleIconLight);
                }
            }

            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ChatsListComponent::disableProfileButton() {
    m_profileButton->setDisabled(true);
}

void ChatsListComponent::activateProfileButton() {
    m_profileButton->setDisabled(false);
}

void ChatsListComponent::updateHideButton() {
    bool isDarkTheme = m_theme == DARK;
    if (m_theme == DARK) {
        if (m_is_hidden) {
            m_hideButton->setIcon(m_hiddenIconDark);
            m_hideButton->setText("  Hidden");
        }
        else {
            m_hideButton->setIcon(m_visibleIconDark);
            m_hideButton->setText("  Visible");
        }
    }
    else {
        if (m_is_hidden) {
            m_hideButton->setIcon(m_hiddenIconLight);
            m_hideButton->setText("  Hidden");
        }
        else {
            m_hideButton->setIcon(m_visibleIconLight);
            m_hideButton->setText("  Visible");
        }
    }
    m_hideButton->setStyleSheet(isDarkTheme ? style->DarkHideButton : style->LightHideButton);
}

void ChatsListComponent::removeComponent(const QString& loginOfRemovedComponent) {
    auto it = std::find_if(m_vec_chatComponents.begin(), m_vec_chatComponents.end(), [loginOfRemovedComponent](ChatComponent* comp) {
        return loginOfRemovedComponent.toStdString() == comp->getChat()->getFriendLogin();
    });

    if (it != m_vec_chatComponents.end()) {
        ChatComponent* foundComp = *it;
        m_containerVLayout->removeWidget(foundComp);
        utility::decreaseFollowingChatIndexes(m_chats_widget->getClient()->getMyHashChatsMap(), foundComp->getChat());

        delete foundComp;
        m_vec_chatComponents.erase(it);
    }
}

void ChatsListComponent::openEditUserDialogWidnow() {
    m_profile_editor_widget->show();
    showProfileDialog();
}

void ChatsListComponent::closeEditUserDialogWidnow() {
    m_profile_editor_widget->hide();
}

void ChatsListComponent::toSendChangeTheme(bool fl) {
    emit sendChangeTheme();
}


void ChatsListComponent::receiveCreateChatData(QString login) {
    QString login2 = login;
    emit sendCreateChatData(login2);
}

void ChatsListComponent::addChatComponent(Theme theme, Chat* chat, bool isSelected) {
    if (isSelected) {
        for (auto chatComp : m_vec_chatComponents) {
            chatComp->setSelected(false);
        }
    }

    auto componentsVecIt = std::find_if(m_vec_chatComponents.begin(), m_vec_chatComponents.end(), [chat](ChatComponent* comp) {
        return chat->getFriendLogin() == comp->getChat()->getFriendLogin();
        });
    
    if (componentsVecIt != m_vec_chatComponents.end()) {
        return;
    }

    ChatComponent* chatComponent = new ChatComponent(this, m_chatsWidget, chat);
    chatComponent->setName(QString::fromStdString(chat->getFriendName()));
    chatComponent->setTheme(theme);
    chatComponent->setSelected(isSelected);
    
    // Устанавливаем онлайн индикатор на основе статуса пользователя
    std::string lastSeen = chat->getFriendLastSeen();
    bool isOnline = (lastSeen == "online");
    chatComponent->setOnlineIndicator(isOnline);
    
    m_containerVLayout->insertWidget(chatComponent->getChat()->getLayoutIndex(), chatComponent);
    m_vec_chatComponents.push_back(chatComponent);

    auto& messagesVec = chat->getMessagesVec();
    if (messagesVec.size() == 0) {
        chatComponent->setLastMessage("no messages yet");
    }
    else {
        auto lastMessage = messagesVec.back();
        QString msg = "";
        if (lastMessage->getRelatedFilesCount() > 0) {
            msg = "File";
        }
        else{
            msg = QString::fromStdString(lastMessage->getMessage());
        }

        chatComponent->setLastMessage(msg);
    }

}

void ChatsListComponent::openAddChatDialog() {
    if (m_isChatAddDialog) {
        return;
    }
    m_chatAddDialog = new AddChatDialogComponent(this, this, m_theme);
    m_chatAddDialog->showDialog();
    m_chatAddDialog->setTheme(m_theme);
    m_searchLineEdit->hide();
    m_mainVLayout->insertWidget(m_mainVLayout->count() - 2, m_chatAddDialog);
    m_isChatAddDialog = true;
}

void ChatsListComponent::closeAddChatDialog() {
    m_chatAddDialog->closeDialog();
    m_isChatAddDialog = false;
}

ChatsWidget* ChatsListComponent::getChatsWidget() const {
    return m_chats_widget;
}

void ChatsListComponent::setTheme(Theme theme) {
    m_theme = theme;
    m_darkModeSwitch->setTheme(m_theme);
    m_profileButton->setTheme(m_theme);
    m_friend_search_dialog->setTheme(m_theme);
    m_newChatButton->setTheme(m_theme);
    m_reconnectButton->setTheme(m_theme);

    updateHideButton();

    if (m_profile_editor_widget != nullptr) {
        m_profile_editor_widget->setTheme(theme);
    }


    if (theme == DARK) {
        if (m_isUpdateLoading) {
            m_updateButton->setStyleSheet(style->updateButtonInProgressDark);
        }
        else {
            m_updateButton->setStyleSheet(style->updateButtonDark);
        }

        m_connectionDownLabel->setStyleSheet(style->DarkNoConnectionLabelStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->DarkSlider);
        m_searchLineEdit->setStyleSheet(style->DarkLineEditStyle);


        for (auto comp : m_vec_chatComponents) {
            comp->setTheme(DARK);
        }

        if (m_chatAddDialog != nullptr) {
            if (m_chatAddDialog->isHidden() == false) {
                m_chatAddDialog->setTheme(DARK);
            }
        }
    }
    else {
        if (m_isUpdateLoading) {
            m_updateButton->setStyleSheet(style->updateButtonInProgressLight);
        }
        else {
            m_updateButton->setStyleSheet(style->updateButtonLight);
        }

        m_updateButton->setStyleSheet(style->updateButtonLight);
        m_connectionDownLabel->setStyleSheet(style->LightNoConnectionLabelStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->LightSlider);
        m_searchLineEdit->setStyleSheet(style->LightLineEditStyle);

        for (auto comp : m_vec_chatComponents) {
            comp->setTheme(LIGHT);
        }
        if (m_chatAddDialog != nullptr) {
            if (m_chatAddDialog->isHidden() == false) {
                m_chatAddDialog->setTheme(LIGHT);
            }
        }
    }
}

void ChatsListComponent::popUpComponent(ChatComponent* comp) {
    std::cout << "call popUpComponent";
    m_containerVLayout->removeWidget(comp);
    m_containerVLayout->insertWidget(0, comp);
}

void ChatsListComponent::setNameFieldInProfileEditorWidget(const std::string& name) {
    m_profile_editor_widget->setName(name);
}

void ChatsListComponent::setAvatar(Avatar* avatar) {
    try {
        const std::string& data = avatar->getBinaryData();

        QByteArray imageData(data.data(), data.size());
        QPixmap avatarPixmap;
        if (!avatarPixmap.loadFromData(imageData)) {
            throw std::runtime_error("Failed to load decrypted avatar");
        }

        QIcon avatarIcon(avatarPixmap);
        m_profileButton->setIcon(avatarIcon);
    }
    catch (const std::exception& e) {
        qWarning() << "Avatar load error:" << e.what();
    }
}

void ChatsListComponent::setAvatarInProfileEditorWidget(Avatar* avatar) {
    m_profile_editor_widget->updateAvatar(avatar);
}

void ChatsListComponent::showConnectionDownLabel() {
    m_chatsWidget->getClient()->stopClient();
    m_hideButton->hide();
    m_updateButton->hide();
    m_reconnectionGif->hide();
    m_hideButton->setChecked(true);
    m_connectionDownLabel->setText("Galactic Silence (Connection Lost)");
    m_connectionDownLabel->show();
    QTimer::singleShot(5000, this, [this]() {
        m_reconnectButton->show();
    });
}

void ChatsListComponent::removeConnectionDownLabel() {
    m_connectionDownLabel->hide();
    m_reconnectionGif->hide();
    m_reconnectButton->hide();
    m_hideButton->show();
}

void ChatsListComponent::showUpdateButton() {
    m_hideButton->hide();
    m_hideButton->setChecked(false);
    m_updateButton->setText("Cosmic Pause (Update Needed)");
    m_updateButton->show();
}


void ChatsListComponent::showProfileDialog()
{
    OverlayWidget* overlay = new OverlayWidget(m_chatsWidget->getMainWindow());
    overlay->show();

    QDialog* dialog = new QDialog(m_chatsWidget->getMainWindow()); 
    dialog->setWindowTitle(tr("Profile Editor"));
    dialog->setFixedSize(1000, 2000); 
    dialog->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    dialog->setAttribute(Qt::WA_TranslucentBackground);

    m_profile_editor_widget->setDialog(dialog);

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(10);
    shadowEffect->setColor(QColor(0, 0, 0, 60)); 
    shadowEffect->setXOffset(0);
    shadowEffect->setYOffset(0);

    QWidget* mainWidget = new QWidget(dialog);
    mainWidget->setGraphicsEffect(shadowEffect);
    mainWidget->setObjectName("mainWidget");

    QString mainWidgetStyle ="QWidget#mainWidget { background-color: transparent; }";
    mainWidget->setStyleSheet(mainWidgetStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(mainWidget);

    QVBoxLayout* contentLayout = new QVBoxLayout(mainWidget);
    contentLayout->setAlignment(Qt::AlignCenter);
    contentLayout->setContentsMargins(16, 16, 16, 16);
    contentLayout->setSpacing(20);
    contentLayout->addWidget(m_profile_editor_widget);

    QRect parentGeometry = m_chatsWidget->getMainWindow()->geometry();
    dialog->move(
        parentGeometry.center() - dialog->rect().center()
    );

    QObject::connect(dialog, &QDialog::finished, overlay, &QWidget::deleteLater);

    dialog->exec();
}

void ChatsListComponent::openHiddenButtonHintDialog() {
    if (m_hintDialog) {
        m_hintDialog->show();
        return;
    }

    m_hintOverlay = new OverlayWidget(m_chatsWidget->getMainWindow());
    m_hintOverlay->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    m_hintOverlay->setAttribute(Qt::WA_TranslucentBackground);
    m_hintOverlay->showMaximized();

    m_hintDialog = new QDialog(m_hintOverlay);
    m_hintDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    m_hintDialog->setAttribute(Qt::WA_TranslucentBackground);
    m_hintDialog->setFixedSize(300, 185);

    QPoint pos = QPoint(50, 65);
    QPoint globalPos = mapToGlobal(pos);
    m_hintDialog->move(globalPos);

    QWidget* container = new QWidget(m_hintDialog);
    container->setObjectName("hintContainer");
    container->setFixedSize(300, 185);

    QLabel* hintLabel = new QLabel("Click to hide your status and appear offline. Click again to return online.\nWhile offline, you cannot send messages, share files, or edit your profile.", container);
    hintLabel->setWordWrap(true);
    hintLabel->setAlignment(Qt::AlignCenter);

    QPushButton* getItButton = new QPushButton("Get it", container);
    getItButton->setFixedSize(80, 30);

    QString containerStyle;
    if (m_theme == DARK) {
        containerStyle =
            "QWidget#hintContainer {"
            "   background-color: rgba(40, 40, 40, 0.80);"
            "   border-radius: 12px;"
            "}";
        getItButton->setStyleSheet(style->DarkHideButton);
        hintLabel->setStyleSheet("color: #E0E0E0; font-size: 14px; font-family: 'Segoe UI'; padding: 0 10px;");
    }
    else {
        containerStyle =
            "QWidget#hintContainer {"
            "   background-color: rgba(250, 250, 250, 0.70);"
            "   border-radius: 12px;"
            "}";
        getItButton->setStyleSheet(style->LightHideButton);
        hintLabel->setStyleSheet("color: #100C08; font-size: 14px; font-family: 'Segoe UI'; padding: 0 10px;");
    }
    container->setStyleSheet(containerStyle);

    QVBoxLayout* containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(20, 20, 20, 20);
    containerLayout->setSpacing(25);
    containerLayout->addWidget(hintLabel);
    containerLayout->addWidget(getItButton, 0, Qt::AlignCenter);

    // Main dialog layout (just contains the styled container)
    QVBoxLayout* mainLayout = new QVBoxLayout(m_hintDialog);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(container);

    connect(getItButton, &QPushButton::clicked, this, &ChatsListComponent::closeHiddenButtonHintDialog);
    connect(m_hintDialog, &QDialog::finished, this, [this](int) {
        closeHiddenButtonHintDialog();
    });

    m_hintDialog->show();
}

void ChatsListComponent::closeHiddenButtonHintDialog() {
    if (m_hintDialog) {
        m_hintDialog->hide();
        m_hintDialog->deleteLater();
        m_hintDialog = nullptr;
    }

    if (m_hintOverlay) {
        m_hintOverlay->hide();
        m_hintOverlay->deleteLater();
        m_hintOverlay = nullptr;
    }
}