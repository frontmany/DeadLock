#include "chatsListComponent.h"
#include "addChatDialogComponent.h"
#include "fieldsEditComponent.h"
#include "chatsWidget.h"
#include "mainwindow.h"
#include "messagingAreaComponent.h"
#include "messageComponent.h"
#include "buttons.h"
#include "photo.h"
#include "utility.h"
#include "client.h"
#include "profileEditorWidget.h"

#include <QPainter>
#include <QPaintEvent>




void ChatsListComponent::loadAvatarFromPC(const std::string & login) {
    QString dir = QString::fromStdString(utility::getSaveDir());
    QString fileNameFinal = QString::fromStdString(login) + "myMainPhoto.png";
    QDir saveDir(dir);
    QString fullPath = saveDir.filePath(fileNameFinal);

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open the file:" << fileNameFinal.toStdString();
        return;
    }
    SetAvatar(Photo(fullPath.toStdString()));
}

ChatsListComponent::ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme)
    : QWidget(parent), m_backgroundColor(Qt::transparent),
    m_chatsWidget(chatsWidget), m_chatAddDialog(nullptr), m_chats_widget(chatsWidget){

    style = new StyleChatsListComponent;
    m_backgroundColor = QColor(20, 20, 20, 200);
    m_theme = theme;

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);

    this->setMinimumSize(200, 300);
    this->setMaximumSize(800, 3000);

    m_profileHLayout = new QHBoxLayout();
    m_profileHLayout->addSpacing(20);
    m_profileHLayout->setAlignment(Qt::AlignLeft);


    m_profileButton = new AvatarIcon(this, 0, 0, 32, true);
    QIcon avatarIcon(":/resources/ChatsWidget/userFriend.png");
    m_profileButton->setIcon(avatarIcon);
    m_profileHLayout->addWidget(m_profileButton);

    m_newChatButton = new ButtonIcon(this, 50, 50);
    QIcon icon1(":/resources/ChatsWidget/startChatDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/startChatHoverDark.png");
    m_newChatButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/startChatLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/startChatHoverLight.png");
    m_newChatButton->uploadIconsLight(icon2, iconHover2);
    m_profileHLayout->addWidget(m_newChatButton);

    m_moon_icon = new ButtonIcon(this, 50, 50);
    QIcon icon3(":/resources/ChatsWidget/moon.png");
    QIcon iconHover3(":/resources/ChatsWidget/moon.png");
    m_moon_icon->uploadIconsDark(icon3, iconHover3);
    QIcon icon4(":/resources/ChatsWidget/moon.png");
    QIcon iconHover4(":/resources/ChatsWidget/moon.png");
    m_moon_icon->uploadIconsLight(icon4, iconHover4);
    m_moon_icon->setTheme(DARK);

    m_darkModeSwitch = new ToggleSwitch(this, m_theme);
    m_darkModeSwitch->setTheme(m_theme);
    QObject::connect(m_darkModeSwitch, &ToggleSwitch::toggled, this, &ChatsListComponent::toSendChangeTheme);
    m_profileHLayout->addSpacing(1500);
    m_profileHLayout->addWidget(m_moon_icon);
    m_profileHLayout->addWidget(m_darkModeSwitch);
    m_profileHLayout->addSpacing(15);

    m_contentsHLayout = new QHBoxLayout();
    m_contentsHLayout->addSpacing(10);
    m_contentsHLayout->setAlignment(Qt::AlignLeft);
    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("  Search...");
    m_searchLineEdit->setMinimumSize(10, 32);
    m_searchLineEdit->setMaximumSize(975, 32);
    m_contentsHLayout->addWidget(m_searchLineEdit);
    m_contentsHLayout->addSpacing(15);


    m_mainVLayout->addLayout(m_profileHLayout);
    m_mainVLayout->addSpacing(10);
    m_mainVLayout->addLayout(m_contentsHLayout);
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

    connect(m_profileButton, &AvatarIcon::clicked, this, &ChatsListComponent::openEditUserDialogWidnow);
    connect(m_newChatButton, &ButtonIcon::clicked, this, &ChatsListComponent::openAddChatDialog);
    connect(this, &ChatsListComponent::sendCreateChatData, m_chatsWidget, &ChatsWidget::onCreateChatButtonClicked);
    connect(this, &ChatsListComponent::sendChangeTheme, m_chatsWidget, &ChatsWidget::onChangeThemeClicked);
}

void ChatsListComponent::openEditUserDialogWidnow() {
    if (m_isEditDialog) {
        return;
    }
    else {
        m_isEditDialog = true;
        m_profile_editor_widget = new ProfileEditorWidget(this, this, m_chatsWidget->getClient(), m_theme);
        m_containerVLayout->insertWidget(0, m_profile_editor_widget);
    }
     
}

void ChatsListComponent::closeEditUserDialogWidnow() {
    m_containerVLayout->removeWidget(m_profile_editor_widget);
    delete m_profile_editor_widget;
    m_profile_editor_widget = nullptr;
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
        chatComponent->setLastMessage(QString::fromStdString(lastMessage->getMessage()));
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
    if (m_profile_editor_widget != nullptr) {
        m_profile_editor_widget->setTheme(theme);
    }

    if (theme == DARK) {
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->darkSlider);
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
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->lightSlider);
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

void ChatsListComponent::SetAvatar(const Photo& photo) {
    QIcon avatarIcon(QString::fromStdString(photo.getPhotoPath()));
    m_profileButton->setIcon(avatarIcon);
}