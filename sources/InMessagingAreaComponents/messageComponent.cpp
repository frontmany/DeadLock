#include "theme.h"
#include "messageComponent.h"
#include "innerComponent.h"
#include "filesComponent.h"
#include "message.h"
#include "messagingAreaComponent.h"
#include "chatsWidget.h"
#include "client.h"
#include "utility.h"


MessageComponent::MessageComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Message* message, Theme theme)
    : QWidget(parent), m_messaging_area_component(messagingAreaComponent), m_theme(theme), m_id(QString::fromStdString(message->getId())), m_isSent(message->getIsSend()),
    m_isRead(message->getIsRead()), m_message(message)
{
    m_main_HLayout = new QHBoxLayout(this);
    if (m_isSent){
        m_main_HLayout->setAlignment(Qt::AlignRight);
    }
    else {
        m_main_HLayout->setAlignment(Qt::AlignLeft);
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    this->setStyleSheet("background-color: transparent;");

    if (message->getRelatedFiles().size() == 0) {
        m_component_structure = ComponentStructure::MESSAGE_COMPONENT;
        m_inner_component = new InnerComponent(this, QString::fromStdString(message->getTimestamp()), QString::fromStdString(message->getMessage()), m_theme, m_isSent);
        m_main_HLayout->addWidget(m_inner_component);
    }
    else {
        m_component_structure = ComponentStructure::FILES_COMPONENT;
        m_files_component = new FilesComponent(this, this, m_message, message->getRelatedFiles(), QString::fromStdString(message->getMessage()), QString::fromStdString(message->getTimestamp()), message->getIsRead(), message->getIsSend(), m_theme);
        m_main_HLayout->addWidget(m_files_component);
    }
    m_main_HLayout->addSpacing(5);
    m_main_HLayout->setContentsMargins(5, 5, 5, 5);
   
    setLayout(m_main_HLayout);
    setIsRead(m_isRead);
    setTheme(m_theme);
}

void MessageComponent::setIsRead(bool isRead) {
    m_isRead = isRead;

    if (m_component_structure == ComponentStructure::MESSAGE_COMPONENT) {
        m_inner_component->setIsRead(isRead);
    }
    else {
        m_files_component->setIsRead(isRead);
    }
}

void MessageComponent::requestedFileUnLoadedError(const fileWrapper& fileWrapper) {
    if (m_component_structure == ComponentStructure::FILES_COMPONENT) {
        m_files_component->requestedFileUnLoaded(fileWrapper);
    }
}

MessageComponent::~MessageComponent() {
    if (m_component_structure == ComponentStructure::MESSAGE_COMPONENT) {
        delete m_inner_component;
    }
    else {
        delete m_files_component;
    }
}

void MessageComponent::setRetry() {
    if (!m_message->getIsSend() || m_retryButtonContainer) {
        return; 
    }

    if (m_component_structure == ComponentStructure::MESSAGE_COMPONENT) {
        m_inner_component->setRetryStyle(true);
    }
    else {
        m_files_component->setRetryStyle(true);
    }


    m_retryButtonContainer = new QWidget(this);
    QVBoxLayout* containerLayout = new QVBoxLayout(m_retryButtonContainer);
    containerLayout->setContentsMargins(0, 0, 0, 8);
    containerLayout->setSpacing(0);
    containerLayout->addStretch();

    m_retryButton = new QPushButton(m_retryButtonContainer);
    m_retryButton->setFixedSize(24, 24);

    QString buttonStyle = (m_theme == DARK) ?
        "QPushButton { background-color: transparent; border: none; padding: 0; }"
        "QPushButton:hover { background-color: rgba(150, 150, 150, 50); border-radius: 12px; }" :
        "QPushButton { background-color: transparent; border: none; padding: 0; }"
        "QPushButton:hover { background-color: rgba(200, 200, 200, 50); border-radius: 12px; }";

    m_retryButton->setStyleSheet(buttonStyle);
    m_retryButton->setCursor(Qt::PointingHandCursor);

    QIcon retryIcon;
    if (m_theme == DARK) {
        retryIcon = QIcon("C:/prj/test_project_qt/reloadDark.png");
    }
    else {
        retryIcon = QIcon("C:/prj/test_project_qt/reloadLight.png");
    }
    m_retryButton->setIcon(retryIcon);
    m_retryButton->setIconSize(QSize(16, 16));

    containerLayout->addWidget(m_retryButton);
    containerLayout->setAlignment(m_retryButton, Qt::AlignLeft | Qt::AlignBottom);

    m_main_HLayout->insertWidget(0, m_retryButtonContainer);
    m_main_HLayout->setAlignment(m_retryButtonContainer, Qt::AlignLeft | Qt::AlignBottom);

    connect(m_retryButton, &QPushButton::clicked, [this]() {
        qDebug() << "Retry button clicked for message:" << m_id;
        if (m_messaging_area_component) {
            m_messaging_area_component->onRetryClicked(m_message);
        }
        removeRetry();
    });
}

void MessageComponent::removeRetry() {
    if (m_retryButtonContainer) {
        m_main_HLayout->removeWidget(m_retryButtonContainer);
        m_retryButtonContainer->deleteLater();
        m_retryButton->deleteLater();
        m_retryButtonContainer = nullptr;
        m_retryButton = nullptr;

        if (m_component_structure == ComponentStructure::MESSAGE_COMPONENT) {
            m_inner_component->setRetryStyle(false);
        }
        else {
            m_files_component->setRetryStyle(false);
        }
    }
}


void MessageComponent::onSendMeFile(const fileWrapper& fileWrapper) {
    m_messaging_area_component->getChatsWidget()->getClient()->requestFile(fileWrapper);
}

void MessageComponent::requestedFileLoaded(const fileWrapper& fileWrapper) {
    m_files_component->requestedFileLoaded(fileWrapper);
}

void MessageComponent::setMessageText(const QString& message) { m_inner_component->setText(message); }
const QString& MessageComponent::getMessageText() const { return m_inner_component->getText(); }

void MessageComponent::setTimestamp(const QString& timestamp) { m_inner_component->setTimestamp(timestamp); }
const QString& MessageComponent::getTimestamp() const { return m_inner_component->getTimestamp(); }

bool MessageComponent::getReadStatus() const { return  m_inner_component->getIsRead(); }

void MessageComponent::setTheme(Theme theme) {
    m_theme = theme;

    if (m_component_structure == ComponentStructure::MESSAGE_COMPONENT) {
        m_inner_component->setTheme(theme);
    }
    else {
        m_files_component->setTheme(theme);
    }


    if (m_retryButtonContainer != nullptr) {
        QIcon retryIcon;
        if (m_theme == DARK) {
            retryIcon = QIcon("C:/prj/test_project_qt/reloadDark.png");
        }
        else {
            retryIcon = QIcon("C:/prj/test_project_qt/reloadLight.png");
        }

        m_retryButton->setIcon(retryIcon);
        m_retryButton->setIconSize(QSize(16, 16));
    }
}