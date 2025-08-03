#include "theme.h"
#include "friendSearchDialogComponent.h"
#include "chatsListComponent.h"
#include "utility.h"
#include "chatsWidget.h"
#include "chatComponent.h"
#include "chat.h"
#include "client.h"
#include "avatar.h"
#include "friendInfo.h"
#include "buttons.h"
#include "configManager.h"

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

void FriendComponent::setFriendData(const QString& name, const QString& loginHash, const QPixmap& avatar)
{
    m_name_label->setText(name);
    m_login_hash = loginHash;

    if (!avatar.isNull()) {
        QIcon ic(avatar);
        
        m_avatar_button->setIcon(QIcon(avatar));
    }
    else {
        QPixmap defaultAvatar(":/resources/ChatsWidget/userFriend.png");
        if (!defaultAvatar.isNull()) {
            m_avatar_button->setIcon(QIcon(defaultAvatar));
        }
    }
}


void FriendComponent::slotToSendData() {
    emit sendData(m_login_hash);
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


void FriendSearchDialogComponent::supplyNewFriendsList(const std::vector<FriendInfo*>& friendInfoVec) {
    for (auto& pair : m_suggestions_map) {
        delete pair.second.first;
    }
    m_suggestions_map.clear();

    clearComponentsMapAndUI();
    m_scrollHLayout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Minimum));

    countUsersWithPhoto = 0;
    countUsersWithPhotoReceived = 0;

    if (friendInfoVec.size() == 0) {
        showNoUsersFoundLabel();
    }

    for (FriendInfo* friendInfo : friendInfoVec) {
        if (friendInfo->getIsFriendHasAvatar()) {
            countUsersWithPhoto++;
            m_suggestions_map[friendInfo->getFriendLoginHash()] = std::make_pair(friendInfo, false);
        }
        else {
            m_suggestions_map[friendInfo->getFriendLoginHash()] = std::make_pair(friendInfo, true);
            addToFriendsListUI(friendInfo->getFriendLoginHash());
        }
    }
}

void FriendSearchDialogComponent::supplyAvatar(Avatar* avatar, std::string loginHash) {
    auto& [friendInfo, isPhotoReceived] = m_suggestions_map.at(loginHash);
    isPhotoReceived = true;
    friendInfo->setFriendAvatar(avatar);

    addToFriendsListUI(loginHash);
}

void FriendSearchDialogComponent::showNoUsersFoundLabel() {
    m_not_found_label = new QLabel("User not found. Try a different name or login.", this);
    if (m_theme == Theme::DARK) {
        m_not_found_label->setStyleSheet(m_style->labelStyleDark);
    }
    else {
        m_not_found_label->setStyleSheet(m_style->labelStyleLight);
    }
    m_scrollHLayout->setAlignment(Qt::AlignTop);
    m_scrollHLayout->addWidget(m_not_found_label);

    showDialog(35);
}

void FriendSearchDialogComponent::showDialog(int size) {
    if (m_is_visible) {
        return;
    }

    m_is_visible = true;
    this->show();
    this->setFixedHeight(size);
}

void FriendSearchDialogComponent::closeDialog() {
    this->hide();
    m_is_visible = false;
}

void FriendSearchDialogComponent::clearComponentsMapAndUI() {
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
}


void FriendSearchDialogComponent::addToFriendsListUI(const std::string& loginHash) {
    
    auto& [friendInfo, isPhotoReceived] = m_suggestions_map.at(loginHash);

    FriendComponent* friendComp = new FriendComponent(m_scrollContent, this, m_theme);
    QString name = QString::fromStdString(friendInfo->getFriendName());
    
    deduceAvatarAndSetDataTo(friendComp, friendInfo, isPhotoReceived);

    m_components_map[loginHash] = friendComp;
    m_scrollHLayout->addWidget(friendComp);

    m_scrollHLayout->setAlignment(Qt::AlignLeft);
    showDialog(160);
}

void FriendSearchDialogComponent::deduceAvatarAndSetDataTo(FriendComponent* friendComp, FriendInfo* friendInfo, bool isPhotoReceived) {
    QPixmap avatar;

    if (friendInfo->getIsFriendHasAvatar() && isPhotoReceived) {
        std::string binaryData = friendInfo->getFriendAvatar()->getBinaryData();
        QByteArray imageData = QByteArray::fromStdString(binaryData);
        if (avatar.loadFromData(imageData)) {
            avatar = avatar.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    if (!friendInfo->getIsFriendHasAvatar()) {
        avatar = QPixmap(":/resources/ChatsWidget/userFriend.png")
            .scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    friendComp->setFriendData(QString::fromStdString(friendInfo->getFriendName()), QString::fromStdString(friendInfo->getFriendLoginHash()), avatar);
}

void FriendSearchDialogComponent::setTheme(Theme theme) {
    m_theme = theme;

    for (auto [login, component] : m_components_map) {
        component->setTheme(m_theme);
    }

    if (m_theme == Theme::DARK) {
        if (m_not_found_label) {
            m_not_found_label->setStyleSheet(m_style->labelStyleDark);
        }

        m_scrollArea->horizontalScrollBar()->setStyleSheet(m_style->darkSlider);

    }
    else {
        if (m_not_found_label) {
            m_not_found_label->setStyleSheet(m_style->labelStyleLight);
        }

        m_scrollArea->horizontalScrollBar()->setStyleSheet(m_style->lightSlider);
    }
}

void FriendSearchDialogComponent::showExistingChat(const std::string& loginHash) {
    auto& chatCompsVec = m_chats_list_component->getChatComponentsVec();

    auto itComp = std::find_if(chatCompsVec.begin(), chatCompsVec.end(), [loginHash](ChatComponent* comp) {
        return loginHash == utility::calculateHash(comp->getChat()->getFriendLogin());
    });

    if (itComp != chatCompsVec.end()) {
        ChatComponent* foundComp = *itComp;
        m_chats_list_component->getChatsWidget()->onSetChatMessagingArea(foundComp->getChat(), foundComp);
    }

    m_chats_list_component->getSearchLineEdit()->setText("");
    closeDialog();
    return;
}

void FriendSearchDialogComponent::addNewChatAndShow(const std::string& loginHash, FriendInfo* friendInfo) {
    Chat* chat = new Chat;
    chat->setFriendLogin(friendInfo->getFriendLogin());
    chat->setFriendName(friendInfo->getFriendName());
    chat->setFriendLastSeen(friendInfo->getFriendLastSeen());
    chat->setLastReceivedOrSentMessage("no messages yet");
    chat->setPublicKey(friendInfo->getPublicKey());

    chat->setIsFriendHasAvatar(friendInfo->getIsFriendHasAvatar());
    if (friendInfo->getIsFriendHasAvatar()) {
        chat->setFriendAvatar(friendInfo->getFriendAvatar());
    }

    auto chatsWidget = m_chats_list_component->getChatsWidget();
    auto client = chatsWidget->getClient();

    chat->setLayoutIndex(0);
    utility::incrementAllChatLayoutIndexes(client->getMyHashChatsMap());

    client->getMyHashChatsMap().emplace(loginHash, chat);

    client->getConfigManager()->save(
        client->getPublicKey(),
        client->getPrivateKey(),
        client->getSpecialServerKey(),
        client->getMyHashChatsMap(),
        client->getIsHidden(),
        client->getDatabase()
    );

    chatsWidget->createAndSetMessagingAreaComponent(chat);
    chatsWidget->createAndAddChatComponentToList(chat);

    m_chats_list_component->getSearchLineEdit()->setText("");
    closeDialog();
}

void FriendSearchDialogComponent::onFriendComponentClicked(const QString& loginHash) {
    std::string loginHashStd = loginHash.toStdString();
    
    auto& [friendInfo, isPhotoReceived] = m_suggestions_map[loginHashStd];
    auto chatsWidget = m_chats_list_component->getChatsWidget();
    auto client = chatsWidget->getClient();

    auto& chatsMap = client->getMyHashChatsMap();
    if (chatsMap.contains(loginHashStd)) {
        showExistingChat(loginHashStd);
    }
    else {
        addNewChatAndShow(loginHashStd, friendInfo);
    }

    namespace fs = std::filesystem;

    std::string fileName = loginHashStd + ".dph";

    fs::path previewsDir = utility::getAvatarPreviewsDirectory();
    fs::path configsDir = utility::getConfigsAndPhotosDirectory();

    fs::path sourcePath = previewsDir / fileName;
    fs::path destPath = configsDir / fileName;

    try {
        if (fs::exists(sourcePath)) {
            fs::rename(sourcePath, destPath);

            fs::copy_file(sourcePath, destPath, fs::copy_options::overwrite_existing);
        }
        else {
            std::cerr << "File not found: " << sourcePath << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }
}