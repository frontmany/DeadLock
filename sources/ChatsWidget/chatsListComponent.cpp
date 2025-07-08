#include "theme.h"
#include "chatsListComponent.h"
#include "addChatDialogComponent.h"
#include "fieldsEditComponent.h"
#include "chatsWidget.h"
#include "messagingAreaComponent.h"
#include "friendSearchDialogComponent.h"
#include "messageComponent.h"
#include "mainwindow.h"
#include "chatHeaderComponent.h"
#include "buttons.h"
#include "photo.h"
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
        border-radius: 15px;         
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
        border-radius: 15px;           
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
    background-color: #383838;
    border-color: #555;
}

QPushButton:pressed {
    background-color: #383838;
}

QPushButton:checked {
    background-color: #4670db;
    border-color: #4A7DBF;
    color: #FFFFFF;
}

QPushButton:checked:hover {
    background-color: #5885f5;
}

QPushButton:disabled {
    background-color: #252525;
    color: #707070;
    border-color: #333;
}
)";

    LightHideButton = R"(
QPushButton {
    background-color: rgb(224, 224, 224);
    border-radius: 15px;
    padding: 8px 16px;
    color: #404040;
    font-family: 'Segoe UI', sans-serif;
    font-size: 14px;
    font-weight: 800;
}

QPushButton:hover {
    background-color: #E8E8E8;
    border-color: #B8B8B8;
}

QPushButton:pressed {
    background-color: #E8E8E8;
}

QPushButton:checked {
    background-color: #D1E3F6;
    border-color: #7EB1E8;
    color: #2A5885;
}

QPushButton:checked:hover {
    background-color: #C5DDF4;
}

QPushButton:disabled {
    background-color: #F0F0F0;
    color: #A0A0A0;
    border-color: #E0E0E0;
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


}

ChatsListComponent::ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme, bool isHidden)
    : QWidget(parent), m_backgroundColor(Qt::transparent),
    m_chatsWidget(chatsWidget), m_chatAddDialog(nullptr), m_chats_widget(chatsWidget),
    m_profile_editor_widget(nullptr), m_is_hidden(isHidden)
{
    style = new StyleChatsListComponent;
    m_backgroundColor = QColor(20, 20, 20, 200);
    m_theme = theme;

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);

    this->setMinimumSize(250, 300);

    if (utility::getDeviceScaleFactor() <= 1.25) {
        this->setMaximumSize(640, 3000);
    }
    if (utility::getDeviceScaleFactor() <= 1) {
        this->setMaximumSize(740, 3000);
    }
    else {
        this->setMaximumSize(580, 3000);
    }

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

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
    m_hideButton->setFlat(true);
    m_hideButton->setCursor(Qt::PointingHandCursor);
    m_hideButton->setCheckable(true);
    m_hideButton->setFixedSize(125, 32);
    m_hideButton->setIconSize(QSize(32, 32));
    m_hideButton->installEventFilter(this);

    if (m_is_hidden) {
        m_hideButton->click();
    }
    updateHideButton();

    connect(m_hideButton, &QPushButton::toggled, this, [this](bool checked) {
        m_is_hidden = checked;
        auto client = m_chatsWidget->getClient();
        client->setIsHidden(checked);
        updateHideButton();

        auto currentMessagingAreaComponent = m_chatsWidget->getCurrentMessagingAreaComponent();
        if (currentMessagingAreaComponent != nullptr) {
            currentMessagingAreaComponent->markVisibleMessagesAsChecked();
        }

        if (m_is_hidden) {
            auto messagingAreasVec = m_chatsWidget->getMessagingAreasVec();
            for (auto comp : messagingAreasVec) {
                comp->getTextEdit()->setDisabled(true);
                comp->getAttachFileButton()->setDisabled(true);
                comp->getChatPropertiesComponent()->disable(true);
                comp->hideSendMessageButton();
            }
            if (m_isEditDialog) {
                closeEditUserDialogWidnow();
            }
            m_profileButton->setDisabled(true);

            client->broadcastMyStatus(utility::getCurrentFullDateAndTime());
        }
        else {
            auto messagingAreasVec = m_chatsWidget->getMessagingAreasVec();
            for (auto comp : messagingAreasVec) {
                comp->getTextEdit()->setDisabled(false);
                comp->getAttachFileButton()->setDisabled(false);
                comp->getChatPropertiesComponent()->disable(false);
            }
            m_profileButton->setDisabled(false);

            client->broadcastMyStatus("online");
        }
    });
    m_profileHLayout->addWidget(m_hideButton);

    m_noConnectionLabel = new QLabel;
    m_noConnectionLabel->hide();
    m_noConnectionLabel->setFixedSize(240, 36);

    m_profileHLayout->addWidget(m_noConnectionLabel);


    m_moon_icon = new QLabel(this);
    m_moon_icon->setFixedSize(50, 50);
    m_moon_icon->setPixmap(QPixmap(":/resources/ChatsWidget/moon.png"));

    m_darkModeSwitch = new ToggleSwitch(this, m_theme);
    m_darkModeSwitch->setTheme(m_theme);
    QObject::connect(m_darkModeSwitch, &ToggleSwitch::toggled, this, &ChatsListComponent::toSendChangeTheme);
    m_profileHLayout->addSpacing(1500);
    m_profileHLayout->addWidget(m_moon_icon);
    m_profileHLayout->addSpacing(-10);
    m_profileHLayout->addWidget(m_darkModeSwitch);
    m_profileHLayout->addSpacing(15);

    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("  Search for friends...");
    m_searchLineEdit->setMinimumSize(10, 32);
    m_searchLineEdit->setMaximumSize(975, 32);

    m_search_timer = new QTimer(this);
    m_search_timer->setInterval(500);
    m_search_timer->setSingleShot(true);

    connect(m_searchLineEdit, &QLineEdit::textChanged, [this]() {
        m_search_timer->start();
    });

    connect(m_search_timer, &QTimer::timeout, [this]() {
        if (m_friend_search_dialog) {
            std::string text = m_searchLineEdit->text().trimmed().toStdString();
            if (text != "" && text.find_first_not_of(' ') != std::string::npos) {
                m_chatsWidget->getClient()->findUser(text);
            }
            else if (text == ""){
                m_friend_search_dialog->close();
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
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setMaximumSize(1000, 2080);
    m_scrollArea->setStyleSheet("background: transparent;");
    m_scrollArea->setWidget(m_containerWidget);

    m_mainVLayout->addWidget(m_scrollArea);

    m_profile_editor_widget = new ProfileEditorWidget(this, this, m_chatsWidget->getClient(), m_chatsWidget->getConfigManager(), m_theme);
    m_containerVLayout->insertWidget(0, m_profile_editor_widget);
    m_profile_editor_widget->hide();

    connect(m_profileButton, &AvatarIcon::clicked, this, &ChatsListComponent::openEditUserDialogWidnow);
    connect(m_newChatButton, &ButtonIcon::clicked, this, &ChatsListComponent::openAddChatDialog);
    connect(this, &ChatsListComponent::sendCreateChatData, m_chatsWidget, &ChatsWidget::onCreateChatButtonClicked);
    connect(this, &ChatsListComponent::sendChangeTheme, m_chatsWidget, &ChatsWidget::onChangeThemeClicked);
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
    if (m_isEditDialog) {
        return;
    }
    else {
        m_profile_editor_widget->show();
        m_isEditDialog = true;
    }
}

void ChatsListComponent::closeEditUserDialogWidnow() {
    m_profile_editor_widget->hide();
    m_isEditDialog = false;
}

void ChatsListComponent::toSendChangeTheme(bool fl) {
    emit sendChangeTheme();
}

ChatsListComponent::~ChatsListComponent() {
    
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
    updateHideButton();

    if (m_profile_editor_widget != nullptr) {
        m_profile_editor_widget->setTheme(theme);
    }

    if (theme == DARK) {
        m_noConnectionLabel->setStyleSheet(style->DarkNoConnectionLabelStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->DarkSlider);
        m_searchLineEdit->setStyleSheet(style->DarkLineEditStyle);
        m_newChatButton->setTheme(theme);
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
        m_noConnectionLabel->setStyleSheet(style->LightNoConnectionLabelStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->LightSlider);
        m_searchLineEdit->setStyleSheet(style->LightLineEditStyle);
        m_newChatButton->setTheme(theme);
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

void ChatsListComponent::setAvatar(const Photo& photo) {
    try {
        std::string path = photo.getPhotoPath();
        if (path.empty()) {
            throw std::runtime_error("Empty photo path");
        }

        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open photo file");
        }

        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string fileData(fileSize, '\0');
        file.read(&fileData[0], fileSize);
        file.close();

        size_t delimiterPos = fileData.find('\n');
        if (delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid photo file format");
        }

        std::string encryptedKey = fileData.substr(0, delimiterPos);
        std::string encryptedData = fileData.substr(delimiterPos + 1);

        auto aesKey = utility::RSADecryptKey(m_chats_widget->getClient()->getPrivateKey(), encryptedKey);

        std::string decryptedData = utility::AESDecrypt(aesKey, encryptedData);

        QByteArray imageData(decryptedData.data(), decryptedData.size());
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

void ChatsListComponent::setAvatarInProfileEditorWidget(const Photo& photo) {
    m_profile_editor_widget->updateAvatar(photo);
}

void ChatsListComponent::showNoConnectionLabel() {
    m_hideButton->hide();
    m_hideButton->setChecked(true);
    m_noConnectionLabel->setText("Lost in Space (No internet connection)");
    m_noConnectionLabel->show();
}

void ChatsListComponent::showServerOfflineLabel() {
    m_hideButton->hide();
    m_hideButton->setChecked(true);
    m_noConnectionLabel->setText("Galactic Silence (Server Down)");
    m_noConnectionLabel->show();
}