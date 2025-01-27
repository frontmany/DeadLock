#pragma once

#include <QWidget>
#include <vector>
#include <thread>
#include <QLayout>

#include "chat.h"



class ChatsListComponent;
class MessagingAreaComponent;
class HelloAreaComponent;
class MainWindow;
class ClientSide;
enum Theme;

class ChatsWidget : public QWidget {

public:
	ChatsWidget(QWidget* parent, ClientSide* client, Theme theme);
	~ChatsWidget();
	void setTheme(Theme theme);
	
	MessagingAreaComponent* getMessagingArea() { return m_messagingAreaComponent; }
	ChatsListComponent* getChatsList() { return m_chatsListComponent; }


public slots:
	void onCreateChatButtonClicked(QString login);
	void onSetChatMessagingArea(Chat* chat);
	

private:
	void loadChats() {/*TODO*/ }
	void paintEvent(QPaintEvent* event) override;
	void setBackGround(Theme theme);

private:
	Theme					m_theme;
	QPixmap					m_background;
	ClientSide*				m_client;
	ChatsListComponent*		m_chatsListComponent;
	MessagingAreaComponent* m_messagingAreaComponent;
	HelloAreaComponent*		m_helloAreaComponent;
	QVBoxLayout*			m_leftVLayout;
	QHBoxLayout*			m_mainHLayout;

	bool                    m_isFirstChatSet;
};