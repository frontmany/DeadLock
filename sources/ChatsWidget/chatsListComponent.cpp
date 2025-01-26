#include "chatsListComponent.h"
#include "chatsWidget.h"
#include "mainwindow.h"
#include "buttons.h"

#include <QPainter>
#include <QPaintEvent>




ChatsListComponent::ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme)
    : QWidget(parent), m_backgroundColor(Qt::transparent), m_chatsWidget(chatsWidget){

    style = new StyleChatsListComponent;
    m_backgroundColor = QColor(20, 20, 20, 200);
    m_theme = theme;

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);

    this->setMinimumSize(600, 300);

    m_profileHLayout = new QHBoxLayout();
    m_profileHLayout->addSpacing(20);
    m_profileHLayout->setAlignment(Qt::AlignLeft);


    m_profileButton = new RoundIconButton(this);
    m_profileButton->setStyleSheet(style->addButtonStyle);
    m_profileButton->setFixedSize(40, 40);
    m_profileHLayout->addWidget(m_profileButton);

    m_newChatButton = new ButtonIcon(this);

    QIcon icon1(":/resources/ChatsWidget/startChatDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/startChatHoverDark.png");
    m_newChatButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/startChatLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/startChatHoverLight.png");
    m_newChatButton->uploadIconsLight(icon2, iconHover2);

    m_profileHLayout->addWidget(m_newChatButton);
    connect(m_newChatButton, &ButtonIcon::clicked, this, &ChatsListComponent::showChatAddDialog);
    


    m_contentsHLayout = new QHBoxLayout();
    m_contentsHLayout->addSpacing(10);
    m_contentsHLayout->setAlignment(Qt::AlignLeft);
    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("Search...");
    m_searchLineEdit->setMaximumSize(975, 30);
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
}

ChatsListComponent::~ChatsListComponent() {
    
}

void ChatsListComponent::addChatComponent(const QString& name, const QString& message, const QPixmap& avatar, Theme theme) {
    ChatComponent* chatComponent = new ChatComponent(this, m_chatsWidget);
    chatComponent->setName(name);
    chatComponent->setMessage(message);
    chatComponent->setAvatar(avatar);
    chatComponent->setTheme(theme);

    
    m_containerVLayout->addWidget(chatComponent);
}

void ChatsListComponent::onTextChanged(const QString& text) {
    if (text == "") {
        if (m_theme == DARK) {
            m_friendLoginEdit->setStyleSheet(style->DarkLineEditStyle);
        }
        else {
            m_friendLoginEdit->setStyleSheet(style->LightLineEditStyle);
        }
    }
}


void ChatsListComponent::showChatAddDialog() {
    if (m_isChatAddDialog) {
        return;
    }

    m_isChatAddDialog = true;

    m_searchLineEdit->setVisible(false);
    StyleChatsListComponent style;

    m_chatAddDialog = new QWidget(this);
    m_dialogLayout = new QVBoxLayout(m_chatAddDialog);
    m_buttonsLayout = new QHBoxLayout();
    m_buttonsLayout->setAlignment(Qt::AlignRight);
    m_buttonsLayout->setAlignment(Qt::AlignLeft);

    m_friendLoginEdit = new QLineEdit(m_chatAddDialog);
    m_friendLoginEdit->setMaximumSize(930, 30);
    m_friendLoginEdit->setPlaceholderText("friend's login...");
    m_buttonsLayout->addWidget(m_friendLoginEdit);
    connect(m_friendLoginEdit, &QLineEdit::textChanged, this, &ChatsListComponent::onTextChanged);

    m_aVLayout = new QVBoxLayout();
    m_cancelButton = new ButtonIcon(m_chatAddDialog);
    QIcon icon1(":/resources/ChatsWidget/close.png");
    QIcon iconHover1(":/resources/ChatsWidget/closeHover.png");
    m_cancelButton->uploadIconsDark(icon1, iconHover1);
    m_cancelButton->uploadIconsLight(icon1, iconHover1);
    m_cancelButton->setMaximumSize(25, 25);
    m_cancelButton->setTheme(m_theme);
    m_aVLayout->addWidget(m_cancelButton);
    m_aVLayout->addSpacing(35);
    m_buttonsLayout->addSpacing(-10);
    m_buttonsLayout->addLayout(m_aVLayout);
    m_buttonsLayout->addSpacing(-10);

    m_createChatButton = new QPushButton("Create Chat", m_chatAddDialog);
    connect(m_createChatButton, &QPushButton::clicked, this, &ChatsListComponent::slotToSendCreateChatData);
    connect(this, &ChatsListComponent::sendCreateChatData, m_chatsWidget, &ChatsWidget::onCreateChatButtonClicked);
    m_createChatButton->setMaximumSize(965, 30);

    m_dialogLayout->addLayout(m_buttonsLayout);
    m_dialogLayout->addWidget(m_createChatButton);

    m_chatAddDialog->setLayout(m_dialogLayout);
    m_chatAddDialog->setFixedHeight(100);

    if (m_theme == DARK) {
        m_friendLoginEdit->setStyleSheet(style.DarkLineEditStyle);
        m_createChatButton->setStyleSheet(style.DarkButtonStyleBlue);
        m_cancelButton->setStyleSheet(style.DarkButtonStyleRed);
    }
    else {
        m_friendLoginEdit->setStyleSheet(style.LightLineEditStyle);
        m_createChatButton->setStyleSheet(style.DarkButtonStyleBlue);
        m_cancelButton->setStyleSheet(style.DarkButtonStyleRed);
    }

    QRect searchLineRect = m_searchLineEdit->geometry();
    int startY = searchLineRect.y() + searchLineRect.height()- 30;
    int endY = 103 - 30;

    m_chatAddDialog->setGeometry(searchLineRect.x(), startY, searchLineRect.width(), m_chatAddDialog->height());
    m_mainVLayout->insertWidget(m_mainVLayout->count() - 2, m_chatAddDialog);


    QPropertyAnimation* animation = new QPropertyAnimation(m_chatAddDialog, "geometry");
    animation->setDuration(800);
    animation->setStartValue(QRect(searchLineRect.x(), startY, searchLineRect.width(), 0)); // Начальная высота 0
    animation->setEndValue(QRect(searchLineRect.x(), endY, searchLineRect.width(), m_chatAddDialog->height())); // Конечная высота
    animation->setEasingCurve(QEasingCurve::InOutCubic);
    animation->start();

    connect(m_cancelButton, &ButtonIcon::clicked, this, [this]() {

        QPropertyAnimation* closeAnimation = new QPropertyAnimation(m_chatAddDialog, "geometry");
        closeAnimation->setDuration(200);
        QRect searchLineRect = m_searchLineEdit->geometry();
        closeAnimation->setStartValue(m_chatAddDialog->geometry());
        closeAnimation->setEndValue(QRect(searchLineRect.x(), searchLineRect.y() - 20 + searchLineRect.height(), searchLineRect.width(), 0)); // Конечная высота 0
        closeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_isChatAddDialog = false;

        connect(closeAnimation, &QPropertyAnimation::finished, m_chatAddDialog, &QWidget::deleteLater);
        connect(closeAnimation, &QPropertyAnimation::finished, this, [this]() {
            m_searchLineEdit->setVisible(true);
            });

        closeAnimation->start();
        });
}

void ChatsListComponent::slotToSendCreateChatData() {
    QString chatName = m_friendLoginEdit->text();
    emit sendCreateChatData(m_friendLoginEdit->text());

    if (m_ableToCreateChat == true) {
        QPropertyAnimation* closeAnimation = new QPropertyAnimation(m_chatAddDialog, "geometry");
        closeAnimation->setDuration(200);
        QRect searchLineRect = m_searchLineEdit->geometry();
        closeAnimation->setStartValue(m_chatAddDialog->geometry());
        closeAnimation->setEndValue(QRect(searchLineRect.x(), searchLineRect.y() - 20 + searchLineRect.height(), searchLineRect.width(), m_chatAddDialog->height()));
        closeAnimation->setEasingCurve(QEasingCurve::OutCubic);
        m_isChatAddDialog = false;

        connect(closeAnimation, &QPropertyAnimation::finished, m_chatAddDialog, &QWidget::deleteLater);
        connect(closeAnimation, &QPropertyAnimation::finished, this, [this]() {
            m_scrollArea->setVisible(true);
            });

        closeAnimation->start();
    }
    
}

void ChatsListComponent::setRedBorderToChatAddDialog() {
    if (m_theme == DARK) {
        m_friendLoginEdit->setStyleSheet(style->DarkLineEditStyleRed);

    }
    else {
        m_friendLoginEdit->setStyleSheet(style->LightLineEditStyleRed);
    }
}

void ChatsListComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (theme == DARK) {
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->darkSlider);
        m_searchLineEdit->setStyleSheet(style->DarkLineEditStyle);
        m_profileButton->setTheme(theme);
        m_newChatButton->setTheme(theme);
    }
    else {
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->lightSlider);
        m_searchLineEdit->setStyleSheet(style->LightLineEditStyle);
        m_profileButton->setTheme(theme);
        m_newChatButton->setTheme(theme);
    }
}