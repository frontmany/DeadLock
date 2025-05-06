#include "friendSearchDialogComponent.h"
#include "chatsListComponent.h"
#include "mainwindow.h"
#include "chat.h"
#include "photo.h"
#include "friendInfo.h"
#include "buttons.h"

StyleFriendComponent::StyleFriendComponent()
{
    widgetStyleDark = R"(
        FriendComponent {
            background-color: #383838;
            border-radius: 12px;
            border: 1px solid #444444;
        }
    )";

    widgetStyleLight = R"(
        FriendComponent {
            background-color: #f5f5f5;
            border-radius: 12px;
            border: 1px solid #e0e0e0;
        }
    )";

    labelStyleDark = R"(
        QLabel {
            color: #f0f0f0;
            font-family: 'Segoe UI';
            font-size: 13px;
            padding: 2px;
            margin-top: 4px;
        }
    )";

    labelStyleLight = R"(
        QLabel {
            color: #333333;
            font-family: 'Segoe UI';
            font-size: 13px;
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

};



FriendComponent::FriendComponent(QWidget* parent,
    FriendSearchDialogComponent* friendSearchDialogComponent,
    Theme theme)
    : QWidget(parent),
    m_theme(theme),
    m_friend_search_dialog_component(friendSearchDialogComponent)
{
    m_style = new StyleFriendComponent();

    setupUi();
    setTheme(m_theme);
    setMinimumSize(100, 140);
    setMaximumWidth(120);
}

void FriendComponent::setupUi() 
{
    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setSpacing(8);
    m_mainVLayout->setContentsMargins(10, 10, 10, 10);

    m_avatar_button = new AvatarIcon(this, 0, 0, 50, true, m_theme);
    m_avatar_button->setFixedSize(50, 50);

    QPixmap defaultAvatar(":/resources/ChatsWidget/userFriend.png");
    if (!defaultAvatar.isNull()) {
        m_avatar_button->setIcon(QIcon(defaultAvatar));
    }

    m_name_label = new QLabel("Friend Name", this);
    m_name_label->setAlignment(Qt::AlignCenter);
    m_name_label->setWordWrap(true);
    m_name_label->setMaximumWidth(100);

    m_mainVLayout->addWidget(m_avatar_button, 0, Qt::AlignHCenter);
    m_mainVLayout->addWidget(m_name_label);
    m_mainVLayout->setAlignment(Qt::AlignTop);
}

void FriendComponent::setTheme(Theme theme) {
    m_theme = theme;

    if (m_theme == Theme::DARK) {
        this->setStyleSheet(m_style->widgetStyleDark);
         m_avatar_button->setStyleSheet(m_style->widgetStyleDark);
         m_name_label->setStyleSheet(m_style->labelStyleDark);
    }
    else {
        this->setStyleSheet(m_style->widgetStyleLight);
        m_avatar_button->setStyleSheet(m_style->widgetStyleLight);
        m_name_label->setStyleSheet(m_style->labelStyleLight);
    }
}

void FriendComponent::setFriendData(const QString& name, const QPixmap& photo)
{
    m_name_label->setText(name);

    if (!photo.isNull()) {
        m_avatar_button->setIcon(QIcon(photo));
    }
    else {
        QPixmap defaultAvatar(":/resources/ChatsWidget/userFriend.png");
        if (!defaultAvatar.isNull()) {
            m_avatar_button->setIcon(QIcon(defaultAvatar));
        }
    }
}

void FriendComponent::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect().adjusted(1, 1, -1, -1);
    QPainterPath path;
    path.addRoundedRect(rect, 12, 12);

    if (m_theme == Theme::DARK) {
        painter.fillPath(path, QColor(0x38, 0x38, 0x38));
        painter.strokePath(path, QPen(QColor(0x44, 0x44, 0x44), 1));
    }
    else {
        painter.fillPath(path, QColor(0xF5, 0xF5, 0xF5));
        painter.strokePath(path, QPen(QColor(0xE0, 0xE0, 0xE0), 1));
    }

    QWidget::paintEvent(event);
}








FriendSearchDialogComponent::FriendSearchDialogComponent(QWidget* parent, ChatsListComponent* chatsListComponent, Theme theme)
    : QWidget(parent), m_chats_list_component(chatsListComponent), m_theme(theme), m_is_visible(false), m_scrollHLayout(nullptr), m_mainVLayout(nullptr)
{
    m_style = new StyleFriendSearchDialogComponent();
    setupUI();
    setupScrollArea();
}

void FriendSearchDialogComponent::setupScrollArea() {
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_scrollArea->setFixedHeight(160);

    m_scrollContent = new QWidget();
    m_scrollHLayout = new QHBoxLayout(m_scrollContent);
    m_scrollHLayout->setAlignment(Qt::AlignLeft);
    m_scrollHLayout->setSpacing(10);
    m_scrollHLayout->setContentsMargins(5, 5, 5, 5);

    m_scrollContent->setLayout(m_scrollHLayout);
    m_scrollArea->setWidget(m_scrollContent);

    m_mainVLayout->addWidget(m_scrollArea);
}


void FriendSearchDialogComponent::refreshFriendsList(const std::vector<FriendInfo*>& friendInfoVec) {
    m_suggestions_map.clear();

    for (FriendInfo* friendInfo : friendInfoVec) {
        m_suggestions_map[friendInfo->getFriendLogin()] = friendInfo;
    }

    updateFriendsListUI();
}

void FriendSearchDialogComponent::showDialog() {
    if (m_is_visible) {
        return;
    }
    this->show();
    this->setFixedHeight(160);
}

void FriendSearchDialogComponent::closeDialog() {
    this->hide();
    m_is_visible = false;
}

void FriendSearchDialogComponent::updateFriendsListUI() {
    QLayoutItem* item;
    while ((item = m_scrollHLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const auto& [login, friendInfo] : m_suggestions_map) {
        FriendComponent* friendComp = new FriendComponent(m_scrollContent, this, m_theme);

        QString name = QString::fromStdString(friendInfo->getFriendName());

        QPixmap avatar;
        if (friendInfo->getIsFriendHasPhoto() && friendInfo->getFriendPhoto() != nullptr) {
            avatar = QPixmap(QString::fromStdString(friendInfo->getFriendPhoto()->getPhotoPath()));
        }
        else {
            avatar = QPixmap(":/resources/ChatsWidget/userFriend.png")
                .scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }

        friendComp->setFriendData(name, avatar);

        m_scrollHLayout->addWidget(friendComp);
    }

    setTheme(m_theme);
}

void FriendSearchDialogComponent::setTheme(Theme theme) {
    if (m_theme == Theme::DARK) {
        m_scrollArea->setStyleSheet("background-color: #333;");
        m_scrollContent->setStyleSheet("background-color: #333;");
    }
    else {
        m_scrollArea->setStyleSheet("background-color: #fff;");
        m_scrollContent->setStyleSheet("background-color: #fff;");
    }
}

void FriendSearchDialogComponent::setupUI() {
    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setContentsMargins(0, 0, 0, 0);
    m_mainVLayout->setSpacing(0);
    setLayout(m_mainVLayout);
}