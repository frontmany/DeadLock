#include "MessagingAreaComponent.h"
#include "chatHeaderComponent.h"
#include "chatsListComponent.h"
#include "delimiterComponent.h"
#include "messageComponent.h"
#include "chatsWidget.h"
#include "mainWindow.h"
#include "utility.h"
#include "buttons.h"
#include "message.h"
#include "client.h"
#include "photo.h"
#include "chat.h"



StyleMessagingAreaComponent::StyleMessagingAreaComponent() {
    darkSlider = R"(
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

    lightSlider = R"(
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

    DarkTextEditStyle = R"(
    QTextEdit {
        background-color: rgb(36, 36, 36);    
        color: white;               
        border: none;     
        border-radius: 15px;         
        padding: 5px;               
    }
    QTextEdit:focus {
        border: 2px solid #888;     
    }
    QTextEdit:disabled {
        background-color: rgb(70, 70, 70);
        color: rgb(150, 150, 150);        
        border: 1px solid rgb(70, 70, 70); 
    }
)";

    LightTextEditStyle = R"(
    QTextEdit {
        background-color: #ffffff;    
        color: black;                 
        border: none;       
        border-radius: 15px;           
        padding: 5px;                 
    }
    QTextEdit:focus {
        border: 2px solid rgb(237, 237, 237);        
    }
    QTextEdit:disabled {
        background-color: rgb(220, 220, 220); 
        color: rgb(150, 150, 150);            
        border: 1px solid rgb(220, 220, 220);  
    }
)";

    LightErrorLabelStyle =
        "QLabel {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   background-color: #FF5252;"
        "   border-radius: 12px;"
        "   padding: 6px 12px;"
        "   border: 1px solid #FF8A80;"
        "   min-width: 60px;"
        "   text-align: center;"
        "   qproperty-alignment: 'AlignCenter';"
        "}";


    DarkErrorLabelStyle =
        "QLabel {"
        "   color: white;"
        "   font-weight: bold;"
        "   font-size: 12px;"
        "   background-color: rgb(252, 81, 81);"
        "   border-radius: 12px;"
        "   padding: 6px 12px;"
        "   border: 1px solid #FF5252;"
        "   min-width: 60px;"
        "   text-align: center;"
        "   qproperty-alignment: 'AlignCenter';"
        "}";

    buttonTransparentDark = R"(
QPushButton {
    background-color: transparent;
    color: rgba(252, 73, 103, 0.9);
    font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
    font-weight: 400;
    font-size: 16px;
    letter-spacing: 0.2px;
    padding: 2px 6px;
    margin-top: 4px;
    border: 1px solid transparent;
    border-radius: 6px;
    min-width: 60px;
}

QPushButton:hover {
    color: rgba(255, 94, 113, 0.95);
    background-color: transparent;
    border: 1px solid transparent;
}

QPushButton:pressed {
    color: rgba(255, 94, 113, 1.0);
    background-color: transparent;
    border: 1px solid transparent;
    padding-top: 3px;
}

QPushButton:disabled {
    color: rgba(110, 110, 110, 0.5);
    background-color: transparent;
}

QPushButton:focus {
    outline: none;
    border: 1px solid transparent;
}
)";

    buttonTransparentLight = R"(
QPushButton {
    background-color: transparent;
    color: rgba(252, 73, 103, 0.9);
    font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
    font-weight: 400;
    font-size: 16px;
    letter-spacing: 0.2px;
    padding: 2px 6px;
    margin-top: 4px;
    border: 1px solid transparent;
    border-radius: 6px;
    min-width: 60px;
}

QPushButton:hover {
    color: rgba(255, 28, 66, 0.95);
    background-color: transparent;
    border: 1px solid transparent;
}

QPushButton:pressed {
    color: rgba(255, 28, 66, 1.0);
    background-color: transparent;
    border: 1px solid transparent;
    padding-top: 3px;
}

QPushButton:disabled {
    color: rgba(150, 150, 150, 0.5);
    background-color: transparent;
}

QPushButton:focus {
    outline: none;
    border: 1px solid transparent;
}
)";
};


FriendProfileComponent::FriendProfileComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Theme theme)
    : QWidget(parent), m_theme(theme), m_messagingAreaComponent(messagingAreaComponent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    setupUI();
}

void FriendProfileComponent::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(10);
    m_mainLayout->setContentsMargins(10, 10, 10, 10);

    m_login_label = new QLabel(this);
    m_login_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_login_label->setAlignment(Qt::AlignLeft);

    m_name_label = new QLabel(this);
    m_name_label->setTextInteractionFlags(Qt::TextSelectableByMouse);
    m_name_label->setAlignment(Qt::AlignLeft);


    m_close_button = new ButtonIcon(this, 25, 25);
    QIcon icon1(":/resources/ChatsWidget/closeDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/closeHoverDark.png");
    m_close_button->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/closeLightGray.png");
    QIcon iconHover2(":/resources/ChatsWidget/closeHoverLightGray.png");
    m_close_button->uploadIconsLight(icon2, iconHover2);
    m_close_button->setTheme(m_theme);

    connect(m_close_button, &ButtonIcon::clicked, m_messagingAreaComponent, &MessagingAreaComponent::closeFriendProfile);

    m_close_button->setParent(this);
    m_close_button->setGeometry(
        170, 
        5,
        width(), 
        120 
    );

    m_mainLayout->addWidget(m_login_label);
    m_mainLayout->addWidget(m_name_label);
    setTheme(m_theme);
}

void FriendProfileComponent::setUserData(const QString& login, const QString& name)
{
    m_login_label->setText("login: " + login);
    m_name_label->setText("name: " + name);
}

void FriendProfileComponent::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);
    painter.setClipPath(path);
    applyGlassEffect(painter, path);
}

void FriendProfileComponent::applyGlassEffect(QPainter& painter, const QPainterPath& path)
{
    painter.fillPath(path, *m_color);
}

void FriendProfileComponent::setTheme(Theme theme) {
    m_theme = theme;

    if (m_theme == Theme::DARK) {
        m_close_button->setTheme(m_theme);
        m_color = new QColor(51, 51, 51);
        m_name_label->setStyleSheet("font-size: 12px; font-weight: bold; color: rgb(203, 215, 245);");
        m_login_label->setStyleSheet("font-size: 12px; font-weight: bold; color: rgb(203, 215, 245);");
    }
    else {
        m_close_button->setTheme(m_theme);
        m_color = new QColor(225, 225, 225);
        m_name_label->setStyleSheet("font-size: 12px; font-weight: bold; color: rgb(138, 192, 255);");
        m_login_label->setStyleSheet("font-size: 12px; font-weight: bold; color: rgb(138, 192, 255);");
    }
}






ChatPropertiesComponent::ChatPropertiesComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Theme theme)
    : QWidget(parent), m_theme(theme), m_messagingAreaComponent(messagingAreaComponent)
{
    m_style = new StyleMessagingAreaComponent;

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);
    setupUI();
}

void ChatPropertiesComponent::disable(bool isDisabled) {
    m_delete_chat_button->setDisabled(isDisabled);
}

void ChatPropertiesComponent::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setAlignment(Qt::AlignVCenter);

    m_delete_chat_button = new  QPushButton(" delete chat");
    m_delete_chat_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_delete_chat_button->setFixedHeight(40);
    connect(m_delete_chat_button, &QPushButton::clicked, [this]() {
        QDialog logoutDialog(this);
        logoutDialog.setWindowTitle(tr("Delete Chat ?"));
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

        QLabel* messageLabel = new QLabel(tr("Are you sure you want to delete this chat?\n All history will be deleted."), &logoutDialog);
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setStyleSheet("font-size: 16px; padding: 20px;");

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* confirmButton = new QPushButton(tr("yes"), &logoutDialog);
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
            emit deleteRequested();
            });

        connect(cancelButton, &QPushButton::clicked, [&]() {
            logoutDialog.reject();
            });

        if (logoutDialog.exec() == QDialog::Accepted) {
        }
        });

    m_close_button = new ButtonIcon(this, 25, 25);
    QIcon icon1(":/resources/ChatsWidget/closeDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/closeHoverDark.png");
    m_close_button->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/closeLightGray.png");
    QIcon iconHover2(":/resources/ChatsWidget/closeHoverLightGray.png");
    m_close_button->uploadIconsLight(icon2, iconHover2);
    m_close_button->setTheme(m_theme);
    connect(m_close_button, &ButtonIcon::clicked, m_messagingAreaComponent, &MessagingAreaComponent::closeChatPropertiesDialog);

    QVBoxLayout* vla = new QVBoxLayout;
    vla->addSpacing(8);
    vla->addWidget(m_close_button);
    vla->setAlignment(Qt::AlignVCenter);

    QHBoxLayout* hla = new QHBoxLayout;
    hla->addWidget(m_delete_chat_button);
    hla->addLayout(vla);
    hla->addSpacing(5);

    connect(this, &ChatPropertiesComponent::deleteRequested, m_messagingAreaComponent, &MessagingAreaComponent::onChatDelete);

    m_mainLayout->addLayout(hla);
    m_mainLayout->setContentsMargins(0, 0, 0, 5);
    setTheme(m_theme);
}

void ChatPropertiesComponent::applyGlassEffect(QPainter& painter, const QPainterPath& path) {
    painter.fillPath(path, *m_color);
}

void ChatPropertiesComponent::paintEvent(QPaintEvent* event) {
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 5, 5);
    painter.setClipPath(path);
    applyGlassEffect(painter, path);
}

void ChatPropertiesComponent::setTheme(Theme theme) {
    m_theme = theme;

    m_close_button->setTheme(m_theme);

    if (m_theme == Theme::DARK) {
        m_color = new QColor(51, 51, 51);   
        QIcon icon(":/resources/ChatsWidget/trashBin.png");
        m_delete_chat_button->setIcon(icon);
        m_delete_chat_button->setStyleSheet(m_style->buttonTransparentDark);
    }
    else {
        m_color = new QColor(225, 225, 225);
        QIcon icon(":/resources/ChatsWidget/trashBin.png");
        m_delete_chat_button->setIcon(icon);
        m_delete_chat_button->setStyleSheet(m_style->buttonTransparentLight);
    }
}

MessagingAreaComponent::MessagingAreaComponent(QWidget* parent, QString friendName, Theme theme, Chat* chat, ChatsWidget* chatsWidget)
    : QWidget(parent), m_friendName(friendName), m_theme(theme), m_chat(chat),
    m_chatsWidget(chatsWidget), m_style(new StyleMessagingAreaComponent())
{
    setMinimumSize(300, 400);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    if (chat->getFriendPhoto()->getPhotoPath() != "")
        chat->setIsFriendHasPhoto(true);

    if (chat->getIsFriendHasPhoto() == true) 
        m_header = new ChatHeaderComponent(this, this, m_theme, QString::fromStdString(m_chat->getFriendName()), QString::fromStdString(m_chat->getFriendLastSeen()), QPixmap(QString::fromStdString(chat->getFriendPhoto()->getPhotoPath())));
    else 
        m_header = new ChatHeaderComponent(this, this, m_theme, QString::fromStdString(m_chat->getFriendName()), QString::fromStdString(m_chat->getFriendLastSeen()), QPixmap());

    m_containerWidget = new QWidget(this);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setWidget(m_containerWidget);
    m_scrollArea->setStyleSheet("background: transparent;");

    m_friend_profile_component = new FriendProfileComponent(this, this, m_theme);
    m_friend_profile_component->hide();
    m_friend_profile_component->setFixedSize(200, 70);
    m_friend_profile_component->setUserData(QString::fromStdString(chat->getFriendLogin()), QString::fromStdString(chat->getFriendName()));
    m_friend_profile_component->setParent(m_scrollArea->viewport());
    m_friend_profile_component->setGeometry(
        17,
        0,
        width(),
        120
    );
    m_friend_profile_component->raise();

    bool isHiddenv = chatsWidget->getClient()->getIsHidden();

    m_chat_properties_component = new ChatPropertiesComponent(this, this, m_theme);
    if (isHiddenv) {
        m_chat_properties_component->disable(true);
    }

    m_chat_properties_component->hide();
    m_chat_properties_component->setFixedSize(170, 50);
    m_chat_properties_component->setParent(m_scrollArea->viewport());
    m_chat_properties_component->setGeometry(
        width() - (m_chat_properties_component->width() + 37),  
        0,                                         
        120,         
        50         
    );


    if (m_theme == DARK) {
        m_backColor = QColor(25, 25, 25);
    }
    else {
        m_backColor = QColor(240, 240, 240);
    }

    

    m_containerVLayout = new QVBoxLayout(m_containerWidget);
    m_containerVLayout->setAlignment(Qt::AlignBottom);



    m_sendMessageButton = new ButtonCursor(this, m_theme);
    QIcon icon1(":/resources/ChatsWidget/sendDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/sendHoverDark.png");
    m_sendMessageButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/sendLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/sendHoverLight.png");
    m_sendMessageButton->uploadIconsLight(icon2, iconHover2);
    m_sendMessageButton->setTheme(m_theme);
    m_sendMessageButton->hide();


    m_sendMessage_VLayout = new QVBoxLayout;
    m_sendMessage_VLayout->setAlignment(Qt::AlignBottom);
    m_sendMessage_VLayout->addWidget(m_sendMessageButton);


    m_messageInputEdit = new MyTextEdit(this);
    m_messageInputEdit->setMinimumHeight(30);
    m_messageInputEdit->setPlaceholderText("Type your message...");
    m_messageInputEdit->setAcceptRichText(false);
    m_messageInputEdit->setFixedHeight(m_messageInputEdit->document()->size().height());

    if (isHiddenv)
        m_messageInputEdit->setDisabled(true);

    QHBoxLayout* m_button_sendHLayout = new QHBoxLayout;
    m_button_sendHLayout->addWidget(m_messageInputEdit);
    m_button_sendHLayout->addLayout(m_sendMessage_VLayout);


    m_error_label = new QLabel;
    m_error_label->hide();

    m_error_labelLayout = new QHBoxLayout;
    m_error_labelLayout->setAlignment(Qt::AlignCenter);
    m_error_labelLayout->setContentsMargins(5, 10, 5, 10);
    m_error_labelLayout->addWidget(m_error_label);


    m_main_VLayout = new QVBoxLayout(this);
    m_main_VLayout->addWidget(m_header);
    m_main_VLayout->addWidget(m_scrollArea);
    m_main_VLayout->addLayout(m_button_sendHLayout);
    m_main_VLayout->addLayout(m_error_labelLayout);

    m_main_VLayout->setContentsMargins(10, 10, 10, 10);
    m_main_VLayout->setSpacing(5);

    m_move_slider_down_button = new ButtonIcon(this, 40, 40);
    QIcon icon3(":/resources/ChatsWidget/arrowDownDark.png");
    QIcon iconHover3(":/resources/ChatsWidget/arrowDownDarkHover.png");
    m_move_slider_down_button->uploadIconsDark(icon3, iconHover3);

    QIcon icon4(":/resources/ChatsWidget/arrowDownLight.png");
    QIcon iconHover4(":/resources/ChatsWidget/arrowDownLightHover.png");
    m_move_slider_down_button->uploadIconsLight(icon4, iconHover4);
    m_move_slider_down_button->setTheme(m_theme);
    m_move_slider_down_button->setIconSize(QSize(40, 40));
    m_move_slider_down_button->hide();
    updateSliderButtonPosition();
    connect(m_move_slider_down_button, &ButtonIcon::clicked, [this]() {moveSliderDown(false); });

    m_typingTimer = new QTimer(this);
    m_typingTimer->setInterval(2200);
    connect(m_typingTimer, &QTimer::timeout, this, &MessagingAreaComponent::onTypingTimeout);

    connect(m_messageInputEdit, &MyTextEdit::textChanged, [this]() {
        adjustTextEditHeight();
        onTypeMessage();

        if (!m_isTypingActive) {
            if (!m_messageInputEdit->m_lastKeyIsBackspaceOrEnter) {
                auto client = m_chatsWidget->getClient();
                client->typingNotify(m_chat->getFriendLogin(), true);
                m_typingTimer->start();
            }
        }
    });

    connect(m_messageInputEdit, &MyTextEdit::enterPressed, this, &MessagingAreaComponent::onSendMessageClicked);
    connect(m_messageInputEdit, &MyTextEdit::pasteExceeded, this, &MessagingAreaComponent::setErrorLabelText);


    connect(m_sendMessageButton, &ButtonCursor::clicked, this, &MessagingAreaComponent::onSendMessageClicked);
    connect(this, &MessagingAreaComponent::sendMessageData, m_chatsWidget, &ChatsWidget::onSendMessageData);

    connect(m_scrollArea->verticalScrollBar(), &QScrollBar::valueChanged,
        this, &MessagingAreaComponent::handleScroll);

    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    adjustTextEditHeight();

    for (auto message : m_chat->getMessagesVec()) {
        addMessage(message, true);
    }
}

void MessagingAreaComponent::onTypingTimeout() {
    m_isTypingActive = false;
    auto client = m_chatsWidget->getClient();
    client->typingNotify(m_chat->getFriendLogin(), false);
}

void MessagingAreaComponent::updateSliderButtonPosition() {
    if (!m_move_slider_down_button) return;

    const int bottomMargin = 90;
    QSize btnSize = m_move_slider_down_button->size();

    m_move_slider_down_button->move(
        (width() - btnSize.width()) / 2,  
        height() - btnSize.height() - bottomMargin
    );
}

void MessagingAreaComponent::resizeEvent(QResizeEvent* event) {
    updateSliderButtonPosition();
    QWidget::resizeEvent(event);
    if (m_chat_properties_component) {
        m_chat_properties_component->setGeometry(
            width() - (m_chat_properties_component->width() + 37),
            0,
            m_chat_properties_component->width(),
            m_chat_properties_component->height()
        );
    }
}

void MessagingAreaComponent::markVisibleMessagesAsChecked() {
    auto client = m_chatsWidget->getClient();
    int sentCount = 0;
        std::vector<MessageComponent*> skippedVec;
        bool isWasSentAtLeastOneConfirmation = false;
        for (auto msgComp : m_vec_unread_messagesComponents) {
            if (isMessageVisible(msgComp)) {
                auto message = msgComp->getMessage();
                message->setIsRead(true);
                client->sendMessageReadConfirmation(m_chat->getFriendLogin(), message);
                isWasSentAtLeastOneConfirmation = true;
                sentCount++;
            }
        }

        if (isWasSentAtLeastOneConfirmation && skippedVec.size() != 0) {
            for (auto msgComp : skippedVec) {
                auto message = msgComp->getMessage();
                message->setIsRead(true);
                client->sendMessageReadConfirmation(m_chat->getFriendLogin(), message);
                sentCount++;
            }
        }

        if (sentCount <= m_vec_unread_messagesComponents.size()) {
            m_vec_unread_messagesComponents.erase(
                m_vec_unread_messagesComponents.begin(),
                m_vec_unread_messagesComponents.begin() + sentCount
            );
        }
}

void MessagingAreaComponent::handleScroll(int value) {
    auto client = m_chatsWidget->getClient();

    QScrollBar* scrollBar = m_scrollArea->verticalScrollBar();
    bool isNearBottom = (value >= scrollBar->maximum() - 400);
    if (m_move_slider_down_button) 
        m_move_slider_down_button->setVisible(!isNearBottom);

    bool isHidden = client->getIsHidden();

    if (!isHidden) {
        markVisibleMessagesAsChecked();
    }
    
}

std::vector<MessageComponent*>& MessagingAreaComponent::getUreadMessageComponents() {
    std::vector<MessageComponent*> vec;
    for (auto msgComponent : m_vec_messagesComponents) {
        if (!msgComponent->getIsRead() && !msgComponent->getIsSent()) {
            vec.emplace_back(msgComponent);
        }
    }

    return m_vec_unread_messagesComponents;
}

MessagingAreaComponent::~MessagingAreaComponent() {
    delete m_style;

    delete m_error_label;     
    delete m_sendMessageButton;   


    delete m_main_VLayout;         
    delete m_containerVLayout;    
    delete m_header;             
    delete m_containerWidget;     
    delete m_messageInputEdit; 
    delete m_friend_profile_component;
}

void MessagingAreaComponent::openChatPropertiesDialog() {
    m_chat_properties_component->show();
}

void MessagingAreaComponent::closeChatPropertiesDialog() {
    m_chat_properties_component->hide();
}

void MessagingAreaComponent::openFriendProfile() {
    m_friend_profile_component->show();
}

void MessagingAreaComponent::closeFriendProfile() {
    m_friend_profile_component->hide();
}

void MessagingAreaComponent::onChatDelete() {
    m_chatsWidget->onChatDelete(QString::fromStdString(m_chat->getFriendLogin()));
}

void MessagingAreaComponent::setErrorLabelText(const QString& errorText) {
    m_error_label->setText(errorText);
    m_error_label->show();

    if (!m_error_label->graphicsEffect()) {
        m_error_label->setGraphicsEffect(new QGraphicsOpacityEffect(this));
    }

    QGraphicsOpacityEffect* opacityEffect =
        qobject_cast<QGraphicsOpacityEffect*>(m_error_label->graphicsEffect());
    opacityEffect->setOpacity(1.0);

    QPropertyAnimation* fadeIn = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeIn->setDuration(500);
    fadeIn->setStartValue(0.0);
    fadeIn->setEndValue(1.0);
    fadeIn->setEasingCurve(QEasingCurve::InQuad);


    QPropertyAnimation* fadeOut = new QPropertyAnimation(opacityEffect, "opacity", this);
    fadeOut->setDuration(1500);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::OutQuad);

    QSequentialAnimationGroup* animationGroup = new QSequentialAnimationGroup(this);
    animationGroup->addAnimation(fadeIn);
    animationGroup->addPause(500); 
    animationGroup->addAnimation(fadeOut);


    connect(animationGroup, &QSequentialAnimationGroup::finished, [this]() {
        m_error_label->hide();
        });

    animationGroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void MessagingAreaComponent::setAvatar(const QPixmap& pixMap) {
    m_header->setAvatar(pixMap);
    update();
}

void MessagingAreaComponent::setName(const QString& name) {
    m_header->setName(name);
}

void MessagingAreaComponent::adjustTextEditHeight() {
    m_messageInputEdit->setFixedHeight(m_messageInputEdit->document()->size().height() + 12); 
}

void MessagingAreaComponent::setTheme(Theme theme) {
    m_theme = theme;

    m_chat_properties_component->setTheme(m_theme);
    m_friend_profile_component->setTheme(m_theme);
    m_move_slider_down_button->setTheme(m_theme);

    if (m_delimiter_component_unread != nullptr) {
        m_delimiter_component_unread->setTheme(m_theme);
    }
    

    if (m_theme == DARK) {
        m_backColor = QColor(25, 25, 25);
        m_messageInputEdit->setStyleSheet(m_style->DarkTextEditStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(m_style->darkSlider);
        m_error_label->setStyleSheet(m_style->DarkErrorLabelStyle);
        m_header->setTheme(DARK);
        for (auto msgComp : m_vec_messagesComponents) {
            msgComp->setTheme(DARK);
        }
        update();

    }
    else {
        m_backColor = QColor(240, 240, 240, 200);
        m_messageInputEdit->setStyleSheet(m_style->LightTextEditStyle);
        m_scrollArea->verticalScrollBar()->setStyleSheet(m_style->lightSlider);
        m_error_label->setStyleSheet(m_style->LightErrorLabelStyle);
        m_header->setTheme(LIGHT);
        for (auto msgComp : m_vec_messagesComponents) {
            msgComp->setTheme(LIGHT);
        }
        update();
    }
}

void MessagingAreaComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backColor); 
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 15, 15); 
}


void MessagingAreaComponent::onSendMessageClicked() {
    onTypingTimeout();

    std::string msg = m_messageInputEdit->toPlainText().toStdString();
    if (msg.find_first_not_of(' ') == std::string::npos) {
        return;
    }

    msg.erase(0, msg.find_first_not_of(' '));
    msg.erase(msg.find_last_not_of(' ') + 1);

    size_t spaceCount = std::count(msg.begin(), msg.end(), ' ');
    if (spaceCount > 1 && msg.length() > 18) {
        msg += '\n';  
    }

    auto& map = m_chatsWidget->getClient()->getMyChatsMap();


    auto chatsList = m_chatsWidget->getChatsList();
    auto& chatCompsVec = chatsList->getChatComponentsVec();
    auto itComp = std::find_if(chatCompsVec.begin(), chatCompsVec.end(), [this](ChatComponent* comp) {
        return m_chat->getFriendLogin() == comp->getChat()->getFriendLogin();
        });
    if (itComp != chatCompsVec.end()) {
        ChatComponent* foundComp = *itComp;
        int index = foundComp->getChat()->getLayoutIndex();
        if (index != 0)
            chatsList->popUpComponent(foundComp);
    }


    if (m_chat->getLayoutIndex() != 0) {
        utility::increasePreviousChatIndexes(map, m_chat);
    }
    m_chat->setLayoutIndex(0);

    updateRelatedChatComponentLastMessage();

    Message* message = new Message(msg, utility::getTimeStamp(), utility::generateId(), true, false);
    addMessage(message, false);
    m_containerWidget->adjustSize();
    onTypeMessage();
    QString s = m_messageInputEdit->toPlainText();
    m_messageInputEdit->setText("");
    emit sendMessageData(message, m_chat);

    moveSliderDown();
}

void MessagingAreaComponent::updateRelatedChatComponentLastMessage() {
    ChatsListComponent* comp = m_chatsWidget->getChatsList();
    std::vector<ChatComponent*> chatCompVec = comp->getChatComponentsVec();
    auto it = std::find_if(chatCompVec.begin(), chatCompVec.end(), [this](ChatComponent* chatComp) {
        return chatComp->getChat()->getFriendLogin() == m_chat->getFriendLogin();
        });

    if (it != chatCompVec.end()) {
        ChatComponent* relatedComp = *it;
        relatedComp->setLastMessage(m_messageInputEdit->toPlainText());
    }

    m_chat->setLastReceivedOrSentMessage((m_messageInputEdit->toPlainText().toStdString()));
}



void MessagingAreaComponent::onTypeMessage() {
    QString text = m_messageInputEdit->toPlainText();
    if (text == "") {
        m_sendMessageButton->hide();
    }
    else {
        m_sendMessageButton->show();
    }
}


void MessagingAreaComponent::addMessage(Message* message, bool isRecoveringMessages) {
    MessageComponent* messageComp = new MessageComponent(m_containerWidget, message, m_theme);

    if (message->getIsSend()) {
        messageComp->setIsRead(message->getIsRead());
    }

    m_vec_messagesComponents.push_back(messageComp);

    if (!message->getIsSend() && !message->getIsRead()) {
        m_vec_unread_messagesComponents.push_back(messageComp);
    }

    if (!isRecoveringMessages) {
        bool isVisible = isMessageVisible(messageComp);

        if ((!isDelimiterUnread && !message->getIsSend() && m_chatsWidget->getCurrentMessagingAreaComponent() != this) ||
            (m_chatsWidget->getCurrentMessagingAreaComponent() == this && !isVisible && !isDelimiterUnread && !message->getIsSend()))
        {
            m_delimiter_component_unread = new DelimiterComponent("unread messages", this, m_theme);
            m_containerVLayout->addWidget(m_delimiter_component_unread);
            isDelimiterUnread = true;
        }

        if (m_chatsWidget->getCurrentMessagingAreaComponent() == this && !message->getIsSend()) {
            if (isVisible) {
                auto client = m_chatsWidget->getClient();
                client->sendMessageReadConfirmation(m_chat->getFriendLogin(), message);
            }
        }
    }
    


    m_containerVLayout->addWidget(messageComp);
    m_containerWidget->adjustSize();
}


void MessagingAreaComponent::removeDelimiterComponentUnread() {
    m_containerVLayout->removeWidget(m_delimiter_component_unread);
    delete m_delimiter_component_unread;
    m_delimiter_component_unread = nullptr;
    isDelimiterUnread = false;
}

void MessagingAreaComponent::moveDelimiterComponentUnreadDown() {
    if (m_delimiter_component_unread != nullptr) {
        removeDelimiterComponentUnread();
    }

    if (m_vec_unread_messagesComponents.empty())
        return;

    auto* msgComp = m_vec_unread_messagesComponents.front();

    m_delimiter_component_unread = new DelimiterComponent("unread messages", this, m_theme);

    int insertIndex = m_containerVLayout->indexOf(msgComp);
    isDelimiterUnread = true;

    if (insertIndex >= 0) {
        m_containerVLayout->insertWidget(insertIndex, m_delimiter_component_unread);
    }
}

void MessagingAreaComponent::moveSliderDown(bool isCalledFromWorker) {

    if (isCalledFromWorker) {
        QCoreApplication::processEvents();
    }

    QTimer::singleShot(0, this, [this]() {
        if (m_containerWidget->layout()->count() > 0) {
            QWidget *lastMessage = m_containerWidget->layout()->itemAt(m_containerWidget->layout()->count() - 1)->widget();
            m_scrollArea->ensureWidgetVisible(lastMessage);
        }
    });
}


bool MessagingAreaComponent::isMessageVisible(MessageComponent* msgComp) const
{
    if (!msgComp || !m_scrollArea || !m_scrollArea->viewport() || m_vec_messagesComponents.empty()) {
        return false;
    }

    QRect msgRect = msgComp->frameGeometry();

    QPoint msgTopLeft = m_containerWidget->mapTo(m_scrollArea->viewport(), msgRect.topLeft());
    QPoint msgBottomRight = m_containerWidget->mapTo(m_scrollArea->viewport(), msgRect.bottomRight());

    QRect viewportRect = m_scrollArea->viewport()->rect();

    bool isVisible = viewportRect.intersects(QRect(msgTopLeft, msgBottomRight));

    if (!isVisible) {
        int scrollBarValue = m_scrollArea->verticalScrollBar()->value();
        int msgBottomPos = m_containerWidget->mapTo(m_scrollArea->viewport(), msgRect.bottomRight()).y();
        int viewportBottom = viewportRect.bottom();

        isVisible = (msgBottomPos > viewportBottom) &&
            (msgBottomPos - viewportBottom < 50);
    }

    return isVisible;
}
