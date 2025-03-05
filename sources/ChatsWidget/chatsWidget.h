#pragma once

#include <QWidget>
#include <vector>
#include <thread>
#include <QLayout>
#include <algorithm>

#include "chat.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class ChatsListComponent;
class MessagingAreaComponent;
class HelloAreaComponent;
class ChatComponent;
class MainWindow;
class Client;
enum Theme;

class ChatsWidget : public QWidget {
	Q_OBJECT
public:
	ChatsWidget(QWidget* parent, Client* client, Theme theme);
	ChatsWidget() = default;
	~ChatsWidget();
	void setTheme(Theme theme);
	const Theme getTheme() const { return m_theme; }
	
	MessagingAreaComponent* getCurrentMessagingArea() { return m_current_messagingAreaComponent; }
	Client* getClientSide() { return m_client; }
	ChatsListComponent* getChatsList() { return m_chatsListComponent; }
	std::vector<MessagingAreaComponent*>& getMessagingComponentsCacheVec() { return m_vec_messagingComponents_cache; }
	void setClient(Client* client);

	//have to be called after loading m_client
	void setup(); 
	void setupChatComponents();

public slots:
	void createMessagingComponent(std::string friendName, Chat* chat);
	void onChangeThemeClicked();
	void onSendMessageData(Message* message, Chat* chat);
	void onCreateChatButtonClicked(QString login);
	void onSetChatMessagingArea(Chat* chat, ChatComponent* component);
	

private:
	void paintEvent(QPaintEvent* event) override;
	void setBackGround(Theme theme);

private:
	Theme					m_theme;
	QPixmap					m_background;
	Client*					m_client;
	ChatsListComponent*		m_chatsListComponent;
	MessagingAreaComponent* m_current_messagingAreaComponent;
	HelloAreaComponent*		m_helloAreaComponent;
	QVBoxLayout*			m_leftVLayout;
	QHBoxLayout*			m_mainHLayout;

	std::vector<MessagingAreaComponent*> m_vec_messagingComponents_cache;
	bool m_isFirstChatSet;
};