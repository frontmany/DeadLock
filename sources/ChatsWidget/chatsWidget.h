#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>
#include <iostream>

#include <QWidget>
#include <QLayout>
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
class Chat;
class Message;
enum  Theme;

class ChatsWidget : public QWidget {
	Q_OBJECT
public:
	ChatsWidget(QWidget* parent, MainWindow* mainWindow, Client* client, Theme theme);
	~ChatsWidget();

	//have to be called after loading m_client
	void restoreMessagingAreaComponents();
	void restoreChatComponents();


	void setClient(Client* client);
	Client* getClient() { return m_client; }

	void setTheme(Theme theme);
	const Theme getTheme() const { return m_theme; }

	void setIsHelloAreaComponent(bool isHelloComponent) { m_is_hello_component = isHelloComponent; }
	bool getIsHelloAreaComponent() { return m_is_hello_component; }
	
	HelloAreaComponent* getHelloAreaComponent() { return m_helloAreaComponent; }

	void setCurrentMessagingAreaComponent(MessagingAreaComponent* messagingAreaComponent) { m_current_messagingAreaComponent = messagingAreaComponent; }
	MessagingAreaComponent* getCurrentMessagingAreaComponent() { return m_current_messagingAreaComponent; }

	MainWindow* getMainWindow();

	ChatsListComponent* getChatsList() { return m_chatsListComponent; }
	std::vector<MessagingAreaComponent*>& getMessagingAreasVec() { return m_vec_messaging_components; }


	QHBoxLayout* getMainHLayout() { return m_mainHLayout; }

public slots:
	void createAndAddChatComponentToList(Chat* chat);
	void createAndSetMessagingAreaComponent(Chat* chat);
	void removeRightComponent();
	void closeAddChatDialog();
	void selectChatComponent(ChatComponent* component);
	void createMessagingComponent(std::string friendName, Chat* chat);
	void onChangeThemeClicked();
	void onSendMessageData(Message* message, Chat* chat);
	void onCreateChatButtonClicked(QString login);
	void onSetChatMessagingArea(Chat* chat, ChatComponent* component);
	

private:
	bool isValidChatCreation(const std::string& loginToCheck);
	void paintEvent(QPaintEvent* event) override;
	void setBackGround(Theme theme);

private:
	Theme					m_theme;
	QPixmap					m_background;
	Client*					m_client;
	ChatsListComponent*		m_chatsListComponent;
	MessagingAreaComponent* m_current_messagingAreaComponent;
	HelloAreaComponent*		m_helloAreaComponent;
	MainWindow*				m_main_window;
	std::mutex				m_mtx;

	QVBoxLayout*			m_leftVLayout;
	QHBoxLayout*			m_mainHLayout;



	std::vector<MessagingAreaComponent*> m_vec_messaging_components;
	bool m_is_hello_component;
};