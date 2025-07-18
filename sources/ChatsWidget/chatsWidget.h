#pragma once
#include <vector>
#include <thread>
#include <algorithm>
#include <mutex>
#include <iostream>

#include <QWidget>
#include <QSplitter>
#include <QLayout>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

class ChatsListComponent;
class MessagingAreaComponent;
class NotificationWidget;
class HelloAreaComponent;
class ChatComponent;
class ConfigManager;
class MainWindow;
class Client;
class Chat;
class Message;
enum  Theme;

class ChatsWidget : public QWidget {
	Q_OBJECT
public:
	ChatsWidget(QWidget* parent, MainWindow* mainWindow, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme);
	~ChatsWidget();

	//have to be called after loading m_client
	void restoreMessagingAreaComponents();
	void restoreChatComponents();


	void setClient(Client* client);
	Client* getClient() { return m_client; }
	std::shared_ptr<ConfigManager> getConfigManager() { return m_config_manager; }

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
	void onNotificationClicked(Chat* chat);
	void showNotification(Chat* chat);
	void createAndAddChatComponentToList(Chat* chat);
	void createAndSetMessagingAreaComponent(Chat* chat);
	void removeRightComponent();
	void closeAddChatDialog();
	void selectChatComponent(ChatComponent* component);
	void createMessagingComponent(std::string friendName, Chat* chat);
	void onChangeThemeClicked();

	void onSendMessageData(Message* message, Chat* chat);
	void onFilesData(Message* message, Chat* chat, size_t filesCount);

	void onCreateChatButtonClicked(QString login);
	void onSetChatMessagingArea(Chat* chat, ChatComponent* component);
	void onChatDelete(const QString& loginOfRemovedChat);

private:
	bool isValidChatCreation(const std::string& loginToCheck);
	void paintEvent(QPaintEvent* event) override;

private:
	QSplitter* m_splitter;
	QWidget* m_leftWidget;

	Theme						   m_theme;
	Client*						   m_client;
	std::shared_ptr<ConfigManager> m_config_manager;
	ChatsListComponent*			   m_chatsListComponent;
	MessagingAreaComponent*		   m_current_messagingAreaComponent = nullptr;
	HelloAreaComponent*			   m_helloAreaComponent;
	MainWindow*					   m_main_window;
	std::mutex					   m_mtx;
	QVBoxLayout*				   m_leftVLayout;
	QHBoxLayout*				   m_mainHLayout;


	std::vector<MessagingAreaComponent*> m_vec_messaging_components;


	bool m_is_hello_component = true;
};