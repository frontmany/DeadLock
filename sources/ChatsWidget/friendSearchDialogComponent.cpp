#include "theme.h"
#include "friendSearchDialogComponent.h"
#include "chatsListComponent.h"
#include "utility.h"
#include "chatsWidget.h"
#include "chatComponent.h"
#include "chat.h"
#include "client.h"
#include "photo.h"
#include "friendInfo.h"
#include "buttons.h"

StyleFriendComponent::StyleFriendComponent()
{
    labelStyleDark = R"(
        QLabel {
            background-color: transparent;
            color: #f0f0f0;
            font-family: 'Segoe UI';
            font-weight: bold;
            font-size: 14px;
            padding: 2px;
            margin-top: 4px;
        }
    )";

    labelStyleLight = R"(
        QLabel {
            background-color: transparent;
            color: rgb(125, 125, 125);
            font-family: 'Segoe UI';
            font-weight: bold;
            font-size: 14px;
            padding: 2px;
            margin-top: 4px;
        }
    )";

    avatarHoverStyleDark = R"(
        AvatarIcon {
            background-color: #505050;
            border: 1px solid #606060;
        }
        AvatarIcon:hover {
            background-color: #606060;
        }
    )";

    avatarHoverStyleLight = R"(
        AvatarIcon {
            background-color: #e0e0e0;
            border: 1px solid #d0d0d0;
        }
        AvatarIcon:hover {
            background-color: #d0d0d0;
        }
    )";
}

StyleFriendSearchDialogComponent::StyleFriendSearchDialogComponent() {
    labelStyleDark = R"(
    QLabel {
        background-color: transparent;
        color: rgba(110, 110, 110, 0.9);
        font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
        font-style: italic;
        font-weight: 400;
        font-size: 14px;
        letter-spacing: 0.2px;
        padding: 2px;
        margin-top: 4px;
        opacity: 0.9;
    }
)";

    labelStyleLight = R"(
    QLabel {
        background-color: transparent;
        color: rgba(110, 110, 110, 0.9);
        font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
        font-style: italic;
        font-weight: 400;
        font-size: 14px;
        letter-spacing: 0.2px;
        padding: 2px;
        margin-top: 4px;
    }
)";

    addButtonStyle = R"(
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

    DarkButtonStyleBlue = R"(
    QPushButton {
        background-color: rgb(21, 119, 232);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        font-family: "Segoe UI";  
        font-size: 14px;          
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";

    LightButtonStyleBlue = R"(
    QPushButton {
        background-color: rgb(26, 133, 255);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        font-family: "Segoe UI";  
        font-size: 14px;          
    }
    QPushButton:hover {
        background-color: rgb(21, 119, 232);   
    }
    QPushButton:pressed {
        background-color: rgb(21, 119, 232);      
    }
)";


    darkSlider = R"(
    QScrollBar:horizontal {
        border: 1px solid #242424;
        background: #242424;
        height: 10px;
        margin: 0px;
        border-radius: 5px;
    }
    QScrollBar::handle:horizontal {
        background: #383838;
        min-width: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:horizontal:hover {
        background: #484848;
    }
    QScrollBar::handle:horizontal:pressed {
        background: #585858;
    }
    QScrollBar::add-line:horizontal,
    QScrollBar::sub-line:horizontal {
        border: none;
        background: none;
    }
    QScrollBar::add-page:horizontal,
    QScrollBar::sub-page:horizontal {
        background: none;
    }
)";

    lightSlider = R"(
    QScrollBar:horizontal {
        border: 1px solid #fafafa;
        background: #fafafa;
        height: 10px;
        margin: 0px;
        border-radius: 5px;
    }
    QScrollBar::handle:horizontal {
        background: #dadbe3;
        min-width: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:horizontal:hover {
        background: #cacbd3;
    }
    QScrollBar::handle:horizontal:pressed {
        background: #babac3;
    }
    QScrollBar::add-line:horizontal,
    QScrollBar::sub-line:horizontal {
        border: none;
        background: none;
    }
    QScrollBar::add-page:horizontal,
    QScrollBar::sub-page:horizontal {
        background: none;
    }
)";
};



FriendComponent::FriendComponent(QWidget* parent,
    FriendSearchDialogComponent* friendSearchDialogComponent,
    Theme theme)
    : QWidget(parent),
    m_theme(theme),
    m_friend_search_dialog_component(friendSearchDialogComponent)
{
    setAttribute(Qt::WA_Hover);
    setFixedHeight(140);
    setMinimumWidth(100);

    m_style = new StyleFriendComponent();

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setSpacing(8);
    m_mainVLayout->setContentsMargins(10, 10, 10, 10);

    m_avatar_button = new AvatarIcon(this, 50, 70, true, m_theme);

    QPixmap defaultAvatar(":/resources/ChatsWidget/userFriend.png");
    if (!defaultAvatar.isNull()) {
        m_avatar_button->setIcon(QIcon(defaultAvatar));
    }

    m_name_label = new QLabel("Friend Name", this);
    m_name_label->setAlignment(Qt::AlignCenter);
    m_name_label->setWordWrap(true);
    m_name_label->setMaximumWidth(200);

    m_mainVLayout->addWidget(m_avatar_button, 0, Qt::AlignHCenter);
    m_mainVLayout->addWidget(m_name_label);
    m_mainVLayout->setAlignment(Qt::AlignCenter);

    connect(m_avatar_button, &AvatarIcon::clicked, this, &FriendComponent::slotToSendData);
    connect(this, &FriendComponent::sendData, m_friend_search_dialog_component, &FriendSearchDialogComponent::onFriendComponentClicked);

    setTheme(m_theme);
}

void FriendComponent::setTheme(Theme theme) {
    m_theme = theme;

    m_avatar_button->setTheme(m_theme);

    if (m_theme == Theme::DARK) {
        m_name_label->setStyleSheet(m_style->labelStyleDark);
    }
    else {
        m_name_label->setStyleSheet(m_style->labelStyleLight);
    }
}

void FriendComponent::setFriendData(const QString& name, const QString& login, const QPixmap& photo)
{
    m_name_label->setText(name);
    m_login = login;

    if (!photo.isNull()) {
        QIcon ic(photo);
        
        m_avatar_button->setIcon(QIcon(photo));
    }
    else {
        QPixmap defaultAvatar(":/resources/ChatsWidget/userFriend.png");
        if (!defaultAvatar.isNull()) {
            m_avatar_button->setIcon(QIcon(defaultAvatar));
        }
    }
}


void FriendComponent::slotToSendData() {
    emit sendData(m_login);
}








FriendSearchDialogComponent::FriendSearchDialogComponent(QWidget* parent, ChatsListComponent* chatsListComponent, Theme theme)
    : QWidget(parent), m_chats_list_component(chatsListComponent), m_theme(theme), m_is_visible(false), m_scrollHLayout(nullptr), m_mainVLayout(nullptr)
{
    m_style = new StyleFriendSearchDialogComponent();

    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setStyleSheet("background-color: transparent;");

    m_scrollContent = new QWidget();
    m_scrollContent->setAttribute(Qt::WA_TranslucentBackground);
    m_scrollContent->setStyleSheet("background-color: transparent;");

    m_scrollHLayout = new QHBoxLayout(m_scrollContent);
    m_scrollHLayout->setAlignment(Qt::AlignLeft);
    m_scrollHLayout->setSpacing(10);
    m_scrollHLayout->setContentsMargins(5, 5, 5, 5);
    m_scrollContent->setLayout(m_scrollHLayout);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setStyleSheet(
        "QScrollArea { background: transparent; border: none; }"
        "QScrollArea > QWidget > QWidget { background-color: transparent; }"
        "QScrollArea > QWidget > QScrollBar { background-color: transparent; }"
    );
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_scrollArea->setFixedHeight(160);
    m_scrollArea->setWidget(m_scrollContent);

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setContentsMargins(0, 0, 0, 0);
    m_mainVLayout->setSpacing(0);
    m_mainVLayout->addWidget(m_scrollArea);

    setLayout(m_mainVLayout);
    setTheme(m_theme);
}


void FriendSearchDialogComponent::refreshFriendsList(const std::vector<FriendInfo*>& friendInfoVec) {
    for (auto& pair : m_suggestions_map) {
        delete pair.second;
    }
    m_suggestions_map.clear();

    for (FriendInfo* friendInfo : friendInfoVec) {
        m_suggestions_map[friendInfo->getFriendLogin()] = friendInfo;
    }

    updateFriendsListUI();
}

void FriendSearchDialogComponent::showDialog(int size) {
    if (m_is_visible) {
        return;
    }
    this->show();
    this->setFixedHeight(size);
}

void FriendSearchDialogComponent::closeDialog() {
    this->hide();
    m_is_visible = false;
}

void FriendSearchDialogComponent::updateFriendsListUI() {
    QLayoutItem* item;
    while ((item = m_scrollHLayout->takeAt(0)) != nullptr) {
        delete item;
    }

    if (m_not_found_label != nullptr) {
        delete m_not_found_label;
        m_not_found_label = nullptr;
    }

    for (auto& pair : m_components_map) {
        delete pair.second;
    }
    m_components_map.clear();

    m_scrollHLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum));

    for (const auto& [login, friendInfo] : m_suggestions_map) {
        FriendComponent* friendComp = new FriendComponent(m_scrollContent, this, m_theme);
        QString name = QString::fromStdString(friendInfo->getFriendName());

        QPixmap avatar;
        bool avatarLoaded = false;

        if (friendInfo->getIsFriendHasPhoto() && friendInfo->getFriendPhoto() != nullptr) {
            std::string binaryData = friendInfo->getFriendPhoto()->getBinaryData();
            QByteArray imageData = QByteArray::fromStdString(binaryData);
            if (avatar.loadFromData(imageData)) {
                avatar = avatar.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                avatarLoaded = true;
            }
        }
        if (!avatarLoaded) {
            avatar = QPixmap(":/resources/ChatsWidget/userFriend.png")
                .scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        
        }

        friendComp->setFriendData(name, QString::fromStdString(login), avatar);
        m_components_map[login] = friendComp;
        m_scrollHLayout->addWidget(friendComp);
    }

    int size = 0;
    if (m_components_map.size() == 0) {
        m_not_found_label = new QLabel("User not found. Try a different name or login.", this);
        if (m_theme == Theme::DARK) {
            m_not_found_label->setStyleSheet(m_style->labelStyleDark);
        }
        else {
            m_not_found_label->setStyleSheet(m_style->labelStyleLight);
        }

        size = 35;
        m_scrollHLayout->setAlignment(Qt::AlignTop);
        m_scrollHLayout->addWidget(m_not_found_label);
    }
    else {
        size = 160;
        m_scrollHLayout->setAlignment(Qt::AlignLeft);
    }

    setTheme(m_theme);
    showDialog(size);
}

void FriendSearchDialogComponent::setTheme(Theme theme) {
    m_theme = theme;

    for (auto [login, component] : m_components_map) {
        component->setTheme(m_theme);
    }

    if (m_theme == Theme::DARK) {
        m_scrollArea->horizontalScrollBar()->setStyleSheet(m_style->darkSlider);

    }
    else {
        m_scrollArea->horizontalScrollBar()->setStyleSheet(m_style->lightSlider);
    }
}

void FriendSearchDialogComponent::onFriendComponentClicked(const QString& login) {
    auto& friendInfo = m_suggestions_map[login.toStdString()];
    auto chatsWidget = m_chats_list_component->getChatsWidget();
    auto client = chatsWidget->getClient();

    auto& chatsMap = client->getMyHashChatsMap();

    auto it = chatsMap.find(utility::calculateHash(login.toStdString()));
    if (it != chatsMap.end()) {
        auto& chatCompsVec = m_chats_list_component->getChatComponentsVec();
        auto itComp = std::find_if(chatCompsVec.begin(), chatCompsVec.end(), [login](ChatComponent* comp) {
            return login.toStdString() == comp->getChat()->getFriendLogin();
        });
        if (itComp != chatCompsVec.end()) {
            ChatComponent* foundComp = *itComp;

            chatsWidget->onSetChatMessagingArea(foundComp->getChat(), foundComp);
        }

        m_chats_list_component->getSearchLineEdit()->setText("");
        closeDialog();
        return;
    }

    Chat* chat = new Chat;
    chat->setFriendLogin(login.toStdString());
    chat->setFriendName(friendInfo->getFriendName());
    chat->setFriendLastSeen(friendInfo->getFriendLastSeen());
    chat->setLastReceivedOrSentMessage("no messages yet");
    chat->setIsFriendHasPhoto(friendInfo->getIsFriendHasPhoto());

    Photo* photo = Photo::deserializeAndSaveOnDisc(friendInfo->getFriendPhoto()->getBinaryData(), login.toStdString());
    chat->setFriendPhoto(photo);
    

    chat->setLayoutIndex(0);
    utility::incrementAllChatLayoutIndexes(client->getMyHashChatsMap());

    client->getMyHashChatsMap().emplace(utility::calculateHash(login.toStdString()), chat);

    chatsWidget->removeRightComponent();
    chatsWidget->createAndSetMessagingAreaComponent(chat);
    chatsWidget->createAndAddChatComponentToList(chat);

    m_chats_list_component->getSearchLineEdit()->setText("");
    closeDialog();
}