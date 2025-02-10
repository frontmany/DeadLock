#include "chatsWidget.h"
#include "ChatsListComponent.h"
#include "messagingAreaComponent.h"
#include "addChatDialogComponent.h"
#include "helloAreaComponent.h"
#include "chatHeaderComponent.h"
#include "clientSide.h"
#include "request.h"
#include "chat.h"
#include "chatComponent.h"
#include"mainWindow.h"


ChatsWidget::ChatsWidget(QWidget* parent, ClientSide* client, Theme theme) 
    : QWidget(parent), m_client(client), m_theme(theme) {

    m_mainHLayout = new QHBoxLayout;
    m_mainHLayout->setAlignment(Qt::AlignLeft);

	m_leftVLayout = new QVBoxLayout;
    m_background.load(":/resources/LoginWidget/lightLoginBackground.jpg");
    m_current_messagingAreaComponent = nullptr;
    m_isFirstChatSet = true;
	m_chatsListComponent = new ChatsListComponent(this, this, m_theme);
    
    
    m_helloAreaComponent = new HelloAreaComponent(m_theme);
	m_leftVLayout->addWidget(m_chatsListComponent);
    m_mainHLayout->addLayout(m_leftVLayout);
    m_mainHLayout->addWidget(m_helloAreaComponent);
    this->setLayout(m_mainHLayout);
	
}



ChatsWidget::~ChatsWidget() {
    serialize();
    for (auto comp : m_vec_messagingComponents_cache) {
        delete comp;
    }
}


void ChatsWidget::onCreateChatButtonClicked(QString login) {
    QString login2 = login;
    Chat* chat = nullptr;
    try {
        chat = m_client->createChatWith(login2.toStdString());
    }
    catch (...) {
        auto ChatsVec = m_client->getMyChatsVec();
        ChatsVec.erase(ChatsVec.end() - 1);
        m_chatsListComponent->getAddChatDialogComponent()->getEditComponent()->setRedBorderToChatAddDialog();
        return;
    }
    
    if (m_isFirstChatSet == true) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
        m_isFirstChatSet = false;
    }
    else {
        m_mainHLayout->removeWidget(m_current_messagingAreaComponent);
        m_current_messagingAreaComponent->hide();
        m_vec_messagingComponents_cache.push_back(m_current_messagingAreaComponent);
    }

    auto messagingAreaComponent = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat, this);
    m_current_messagingAreaComponent = messagingAreaComponent;
    m_current_messagingAreaComponent->setTheme(m_theme);
    m_vec_messagingComponents_cache.push_back(m_current_messagingAreaComponent);
    m_mainHLayout->addWidget(m_current_messagingAreaComponent);

    m_chatsListComponent->addChatComponent(m_theme, chat);
    m_chatsListComponent->closeAddChatDialog();
}

void ChatsWidget::onSetChatMessagingArea(Chat* chat, ChatComponent* component) {
    if (m_isFirstChatSet == true) {
        m_mainHLayout->removeWidget(m_helloAreaComponent);
        delete m_helloAreaComponent;
        m_isFirstChatSet = false;
    }
    else {
        m_mainHLayout->removeWidget(m_current_messagingAreaComponent);
        m_current_messagingAreaComponent->hide();
    }

    auto itMsgComp = std::find_if(m_vec_messagingComponents_cache.begin(), m_vec_messagingComponents_cache.end(), [chat](MessagingAreaComponent* msgComp) {
        return msgComp->getChatConst()->getFriendLogin() == chat->getFriendLogin();
        });

    if (itMsgComp == m_vec_messagingComponents_cache.end()) {
        std::cout << "error can not find mesaging Area Component";
    }
    else {
        m_current_messagingAreaComponent = *itMsgComp;
        m_current_messagingAreaComponent->setTheme(m_theme);
        m_current_messagingAreaComponent->show();
        m_mainHLayout->addWidget(m_current_messagingAreaComponent);

        if (chat->getNotReadReceivedMsgVec().size() > 0) {
            m_client->sendMessagesReadPacket(chat->getFriendLogin(), chat->getNotReadReceivedMsgVec());
            chat->getNotReadReceivedMsgVec().clear();

        }

    }

    for (auto chatComp : m_chatsListComponent->getChatComponentsVec()) {
        chatComp->setSelected(false);
    }
    component->setSelected(true);
}

void ChatsWidget::onSendMessageData(QString message, const QString& timeStamp, Chat* chat, double id) {
    auto& chatsComponentsVec = m_chatsListComponent->getChatComponentsVec();
    auto itComponentsVec = std::find_if(chatsComponentsVec.begin(), chatsComponentsVec.end(), [chat](ChatComponent* chatComponent) {
        return chat->getFriendLogin() == chatComponent->getChatConst()->getFriendLogin();
        });

    ChatComponent* comp = *itComponentsVec;
    if (message.length() > 15) {
        std::string s = message.toStdString().substr(0, 15) + "...";
        comp->setLastMessage(QString::fromStdString(s), false);
    }
    else {
        comp->setLastMessage(message, false);
    }
    
    Msg* msg = new Msg;
    msg->setId(id);
    msg->setIsSend(true);
    msg->setMessage(message.toStdString());
    msg->setTimestamp(timeStamp.toStdString());
    m_client->sendMessage(chat, message.toStdString(), timeStamp.toStdString(), id);
}

void ChatsWidget::onChangeThemeClicked() {
    if (m_theme == DARK) {
        m_theme = LIGHT;
        setTheme(LIGHT);
    }
    else {
        m_theme = DARK;
        setTheme(DARK);
    }
    
}

void ChatsWidget::setTheme(Theme theme) {
    m_theme = theme;
    setBackGround(theme);
    m_chatsListComponent->setTheme(theme);
    if (m_current_messagingAreaComponent != nullptr) {
        m_current_messagingAreaComponent->setTheme(m_theme);
    }
    else {
        m_helloAreaComponent->setTheme(m_theme);
    }
}


void ChatsWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawPixmap(this->rect(), m_background);
    QWidget::paintEvent(event);
}


void ChatsWidget::createMessagingAreaFromClientSide(QString message, QString timeStamp, Chat* chat, double id) {
    MessagingAreaComponent* newComp = new MessagingAreaComponent(this, QString::fromStdString(chat->getFriendName()), m_theme, chat, this);
    newComp->setTheme(m_theme);
    newComp->addMessageReceived(message, timeStamp, id);
    m_vec_messagingComponents_cache.push_back(newComp);
}

void ChatsWidget::setBackGround(Theme theme) {
    if (theme == DARK) {
        if (m_background.load(":/resources/ChatsWidget/darkChatsBackground.jpg")) {
        }
    }
    else {
        if (m_background.load(":/resources/ChatsWidget/lightChatsBackground.jpg")) {
        }
    }
    update();
}


void ChatsWidget::setClient(ClientSide* client) {
    m_client = client;
}

void ChatsWidget::serialize() {
    QJsonObject jsonObject;
    QJsonArray chatsArray;
    for (const auto& messagingArea : m_vec_messagingComponents_cache) {
        if (messagingArea) {
            chatsArray.append(messagingArea->serialize());
        }
    }

    jsonObject["messagingComponentsCache"] = chatsArray;
    jsonObject["client"] = m_client->serialize();
    // Получаем директорию для сохранения
    QString dir = getSaveDir();

    qDebug() << m_client->getMyInfo().getLogin();
    QString fileName = QString::fromStdString(m_client->getMyInfo().getLogin()) + ".json";

    // Создаем полный путь к файлу
    QDir saveDir(dir);
    if (!saveDir.exists()) {
        if (!saveDir.mkpath(".")) { // Создаем директорию, если она не существует
            qWarning() << "Не удалось создать директорию:" << dir;
            return;
        }
    }

    QString fullPath = saveDir.filePath(fileName); // Полный путь к файлу
    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument saveDoc(jsonObject);
        file.write(saveDoc.toJson());
        file.close();
    }
    else {
        qWarning() << "Не удалось открыть файл для записи:" << fullPath;
    }
}


ChatsWidget* ChatsWidget::deserialize(const QString& fileName, QWidget* parent, ClientSide* client, Theme theme) {
    QString dir = getSaveDir();
    QDir saveDir(dir);
    QString fullPath = saveDir.filePath(fileName); // Полный путь к файлу

    if (!QFile::exists(fullPath)) {
        qWarning() << "Файл не найден: " << fullPath;
        return new ChatsWidget(parent, client, theme);
    }

    QFile file(fullPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Не удалось открыть файл для чтения:" << file.errorString();
        return new ChatsWidget(parent, client, theme);
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument loadDoc(QJsonDocument::fromJson(fileData));
    if (loadDoc.isNull() || !loadDoc.isObject()) {
        qWarning() << "Error loading JSON from a file.";
        return new ChatsWidget(parent, client, theme);
    }

    QJsonObject jsonObject = loadDoc.object();
    ChatsWidget* chatsWidget = new ChatsWidget(parent, client, theme);

    QJsonObject clientObject = jsonObject["client"].toObject();
    auto pair =  ClientSide::deserialize(clientObject);
    client->setMyInfo(pair.second);
    client->getMyChatsVec() = pair.first;

    if (jsonObject.contains("messagingComponentsCache")) {
        QJsonArray chatsArray = jsonObject["messagingComponentsCache"].toArray();
        for (const auto& chatValue : chatsArray) {
            QJsonObject chatObject = chatValue.toObject();
            MessagingAreaComponent* messagingArea = MessagingAreaComponent::deserialize(chatObject, chatsWidget, chatsWidget);
            if (messagingArea) {
                chatsWidget->getMessagingComponentsCacheVec().push_back(messagingArea);
            }
            else {
                qWarning() << "Ошибка десериализации messagingArea.";
            }
        }
    }

    return chatsWidget;
}