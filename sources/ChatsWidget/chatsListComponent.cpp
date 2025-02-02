#include "chatsListComponent.h"
#include "addChatDialogComponent.h"
#include "chatsWidget.h"
#include "mainwindow.h"
#include "buttons.h"

#include <QPainter>
#include <QPaintEvent>




ChatsListComponent::ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme)
    : QWidget(parent), m_backgroundColor(Qt::transparent),
    m_chatsWidget(chatsWidget), m_chatAddDialog(nullptr) {

    style = new StyleChatsListComponent;
    m_backgroundColor = QColor(20, 20, 20, 200);
    m_theme = theme;

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);

    this->setMinimumSize(200, 300);
    this->setMaximumSize(1000, 3000);

    m_profileHLayout = new QHBoxLayout();
    m_profileHLayout->addSpacing(20);
    m_profileHLayout->setAlignment(Qt::AlignLeft);


    m_profileButton = new RoundIconButton(this);
    m_profileButton->setStyleSheet(style->transparentButtonStyle);
    m_profileButton->setFixedSize(40, 40);
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

    connect(m_newChatButton, &ButtonIcon::clicked, this, &ChatsListComponent::openAddChatDialog);
    connect(this, &ChatsListComponent::sendCreateChatData, m_chatsWidget, &ChatsWidget::onCreateChatButtonClicked);
    connect(this, &ChatsListComponent::sendChangeTheme, m_chatsWidget, &ChatsWidget::onChangeThemeClicked);
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

void ChatsListComponent::addChatComponent(Theme theme, Chat* chat) {
    for (auto chatComp : m_vec_chatComponents) {
        chatComp->setSelected(false);
    }
    ChatComponent* chatComponent = new ChatComponent(this, m_chatsWidget, chat);
    chatComponent->setName(QString::fromStdString(chat->getFriendName()));
    chatComponent->setTheme(theme);
    m_containerVLayout->insertWidget(0, chatComponent);
    m_vec_chatComponents.push_back(chatComponent);
    chatComponent->setSelected(true);
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



void ChatsListComponent::setTheme(Theme theme) {
    m_theme = theme;
    m_darkModeSwitch->setTheme(m_theme);
    if (theme == DARK) {
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->darkSlider);
        m_searchLineEdit->setStyleSheet(style->DarkLineEditStyle);
        m_profileButton->setTheme(theme);
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
        m_profileButton->setTheme(theme);
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
    m_containerVLayout->removeWidget(comp);
    m_containerVLayout->insertWidget(0, comp);
}

void ChatsListComponent::addChatComponentSlot(QString theme, Chat* chat) {
    Theme themeT;
    if (theme == "DARK") {
        themeT = DARK;
    }
    else {
        themeT = LIGHT;
    }

    for (auto chatComp : m_vec_chatComponents) {
        chatComp->setSelected(false);
    }
    ChatComponent* chatComponent = new ChatComponent(this, m_chatsWidget, chat);
    chatComponent->setLastMessage(QString::fromStdString(chat->getLastIncomeMessage()), true);
    chatComponent->setUnreadMessageDot(true);
    chatComponent->setName(QString::fromStdString(chat->getFriendName()));
    chatComponent->setTheme(themeT);
    m_containerVLayout->insertWidget(0, chatComponent);
    m_vec_chatComponents.push_back(chatComponent);
    chatComponent->setSelected(false);
}