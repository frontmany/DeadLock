#include "chatsListComponent.h"
#include "addChatDialogComponent.h"
#include "fieldsEditComponent.h"
#include "chatsWidget.h"
#include "mainwindow.h"
#include "messagingAreaComponent.h"
#include "friendSearchDialogComponent.h"
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
    m_chatsWidget(chatsWidget), m_chatAddDialog(nullptr), m_chats_widget(chatsWidget),
    m_profile_editor_widget(nullptr) 
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
        this->setMaximumSize(540, 3000);
    }

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    m_profileHLayout = new QHBoxLayout();
    m_profileHLayout->addSpacing(20);
    m_profileHLayout->setAlignment(Qt::AlignLeft);


    m_profileButton = new AvatarIcon(this, 0, 0, 32, true, m_theme);
    QIcon avatarIcon(":/resources/ChatsWidget/userFriend.png");
    m_profileButton->setIcon(avatarIcon);
    m_profileHLayout->addWidget(m_profileButton);

    m_logoutButton = new ButtonIcon(this, 40, 40);
    QIcon icon1(":/resources/ChatsWidget/logoutDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/logoutHoverDark.png");
    m_logoutButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/logoutLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/logoutHoverLight.png");
    m_logoutButton->uploadIconsLight(icon2, iconHover2);
    m_logoutButton->setIconSize(QSize(25,25));
    m_profileHLayout->addWidget(m_logoutButton);

    m_newChatButton = new ButtonIcon(this, 50, 50);
    QIcon icon3(":/resources/ChatsWidget/startChatDark.png");
    QIcon iconHover3(":/resources/ChatsWidget/startChatHoverDark.png");
    m_newChatButton->uploadIconsDark(icon3, iconHover3);
    QIcon icon4(":/resources/ChatsWidget/startChatLight.png");
    QIcon iconHover4(":/resources/ChatsWidget/startChatHoverLight.png");
    m_newChatButton->uploadIconsLight(icon4, iconHover4);
    m_profileHLayout->addWidget(m_newChatButton);


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
    m_searchLineEdit->setPlaceholderText("  Search...");
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
    connect(m_logoutButton, &ButtonIcon::clicked, [this]() {
        QDialog logoutDialog(this);
        logoutDialog.setWindowTitle(tr("Exit Confirmation"));
        logoutDialog.setMinimumSize(300, 150);

        QString dialogStyle;
        QString buttonStyle;

        if (m_theme == Theme::DARK) {
            dialogStyle = R"(
            QDialog {
                background-color: #333333;
                color: #f0f0f0;
                font-family: 'Segoe UI';
                font-size: 14px;
                border: 1px solid #444444;
                border-radius: 8px;
            }
            QLabel {
                color: #f0f0f0;
            }
        )";

            buttonStyle = R"(
            QPushButton {
                background-color: #444444;
                color: #f0f0f0;
                border: 1px solid #555555;
                border-radius: 4px;
                padding: 6px 12px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: #555555;
            }
            QPushButton:pressed {
                background-color: #3a3a3a;
            }
        )";
        }
        else {
            dialogStyle = R"(
            QDialog {
                background-color: #ffffff;
                color: #333333;
                font-family: 'Segoe UI';
                font-size: 14px;
                border: 1px solid #dddddd;
                border-radius: 8px;
            }
            QLabel {
                color: #333333;
            }
        )";

            buttonStyle = R"(
            QPushButton {
                background-color: #f0f0f0;
                color: #333333;
                border: 1px solid #cccccc;
                border-radius: 4px;
                padding: 6px 12px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: #e5e5e5;
            }
            QPushButton:pressed {
                background-color: #d9d9d9;
            }
        )";
        }

        logoutDialog.setStyleSheet(dialogStyle);

        QVBoxLayout* layout = new QVBoxLayout(&logoutDialog);

        QLabel* messageLabel = new QLabel(tr("Are you sure you want to quit?"), &logoutDialog);
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setStyleSheet("font-size: 16px; padding: 20px;");

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* confirmButton = new QPushButton(tr("quit"), &logoutDialog);
        QPushButton* cancelButton = new QPushButton(tr("cancel"), &logoutDialog);

        confirmButton->setStyleSheet(buttonStyle);
        cancelButton->setStyleSheet(buttonStyle);

        buttonLayout->addStretch();
        buttonLayout->addWidget(confirmButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addStretch();

        layout->addWidget(messageLabel);
        layout->addLayout(buttonLayout);

        connect(confirmButton, &QPushButton::clicked, [&]() {
            logoutDialog.accept();
            emit logoutRequested(); 
            });

        connect(cancelButton, &QPushButton::clicked, [&]() {
            logoutDialog.reject();
            });

        if (logoutDialog.exec() == QDialog::Accepted) {
        }
    }); //TODO design

    connect(m_newChatButton, &ButtonIcon::clicked, this, &ChatsListComponent::openAddChatDialog);
    connect(this, &ChatsListComponent::sendCreateChatData, m_chatsWidget, &ChatsWidget::onCreateChatButtonClicked);
    connect(this, &ChatsListComponent::sendChangeTheme, m_chatsWidget, &ChatsWidget::onChangeThemeClicked);
    connect(this, &ChatsListComponent::logoutRequested, m_chatsWidget, &ChatsWidget::onLogoutRequested);
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
    m_logoutButton->setTheme(m_theme);
    m_profileButton->setTheme(m_theme);

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