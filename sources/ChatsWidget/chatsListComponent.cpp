#include "ChatsListComponent.h"
#include "mainwindow.h"
#include <QPainter>
#include <QPaintEvent>


ChatsListComponent::ChatsListComponent(QWidget* parent)
    : QWidget(parent), m_backgroundColor(Qt::transparent) {

    style = new StyleChatsListComponent;
    m_backgroundColor = QColor(20, 20, 20, 200);

    m_mainVLayout = new QVBoxLayout(this);
    m_contentsHLayout = new QHBoxLayout();
    m_contentsHLayout->addSpacing(10);
    m_contentsHLayout->setAlignment(Qt::AlignLeft);


    m_startChatButton = new QPushButton(this);
    m_startChatButton->setStyleSheet(style->addButtonStyle);
    QIcon icon(":/resources/ChatsWidget/addChat.png");
    m_startChatButton->setIcon(icon);
    m_startChatButton->setIconSize(QSize(30, 30));

    m_startChatButton->setFixedSize(35, 30);
    m_contentsHLayout->addWidget(m_startChatButton);

    m_searchLineEdit = new QLineEdit(this);
    m_searchLineEdit->setPlaceholderText("Search...");
    m_searchLineEdit->setMaximumSize(920, 30);
    m_contentsHLayout->addWidget(m_searchLineEdit);
    m_contentsHLayout->addSpacing(20);
    m_mainVLayout->addLayout(m_contentsHLayout);
    

    m_containerWidget = new QWidget();
 
    m_containerVLayout = new QVBoxLayout(m_containerWidget);
    m_containerWidget->setLayout(m_containerVLayout);


    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setMaximumSize(1000, 2080);
    m_scrollArea->setStyleSheet("background: transparent;");
    m_scrollArea->setWidget(m_containerWidget);


    m_mainVLayout->addWidget(m_scrollArea);
}

ChatsListComponent::~ChatsListComponent() {
    // Деструктор, если нужно, можно добавить очистку ресурсов
}

void ChatsListComponent::addChatComponent(const QString& name, const QString& message, const QPixmap& avatar) {
    ChatComponent* chatComponent = new ChatComponent(this);
    chatComponent->setName(name);
    chatComponent->setMessage(message);
    chatComponent->setAvatar(avatar);

    
    m_containerVLayout->addWidget(chatComponent);
}

void ChatsListComponent::paintEvent(QPaintEvent* event) {

}


void ChatsListComponent::setTheme(Theme theme) {
    if (theme == DARK) {
        m_scrollArea->verticalScrollBar()->setStyleSheet(style->darkSlider);
        m_searchLineEdit->setStyleSheet(style->DarkLineEditStyle);
    }
    else {

    }
}