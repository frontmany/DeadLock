#include "theme.h"
#include "messageComponent.h"
#include "innerComponent.h"
#include "filesComponent.h"
#include "messagingAreaComponent.h"
#include "chatsWidget.h"
#include "message.h"
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
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    this->setStyleSheet("background-color: transparent;");
    if (message->getRelatedFiles().size() == 0) {
        m_inner_component = new InnerComponent(this, QString::fromStdString(message->getTimestamp()), QString::fromStdString(message->getMessage()), m_theme, m_isSent);
        m_main_HLayout->addWidget(m_inner_component);
    }
    else {
        m_files_component = new FilesComponent(this, this, m_message, message->getRelatedFiles(), QString::fromStdString(message->getMessage()), QString::fromStdString(message->getTimestamp()), message->getIsRead(), message->getIsSend(), m_theme);
        m_main_HLayout->addWidget(m_files_component);
    }
    m_main_HLayout->addSpacing(5);
    m_main_HLayout->setContentsMargins(5, 5, 5, 5);
    m_main_HLayout->setSizeConstraint(QLayout::SetMaximumSize);
   
    setLayout(m_main_HLayout);
    setIsRead(false);
    setTheme(theme);
    m_inner_component->setReadStatus(false);

   
}

void MessageComponent::setIsRead(bool isRead) {
    m_inner_component->setIsRead(isRead);
    m_inner_component->setReadStatus(isRead);
    m_isRead = isRead;
}

MessageComponent::~MessageComponent() {
    delete m_inner_component;
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
void MessageComponent::setTheme(Theme theme) { m_inner_component->setTheme(theme); }