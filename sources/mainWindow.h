#pragma once

#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QEvent>
#include <QDir>

#include "loginWidget.h"
#include "chatsWidget.h"
#include "greetWidget.h"

class Client;
class WorkerQt;


/*
using asio::ip::tcp;

class MyWorkerUI : public WorkerUI {
public:
	void onStatusReceive() override {
		std::cout << "status received!" << std::endl;
	}

	void onMessageReceive() override {
		std::cout << "Message received!" << std::endl;
	}

	void onMessageReadConfirmationReceive() override {
		std::cout << "Confirmation of reading the message has been received!" << std::endl;
	}
};
*/

enum Theme {
	DARK,
	LIGHT
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent);
	~MainWindow();


public slots:
	void onLogin(bool isRegistration);

private:
	void setupLoginWidget();
	void setupChatsWidget();
	void setupGreetWidget();
	bool isDarkMode();

private:
	WorkerQt*		m_worker;
	Theme			m_theme;
	Client*			m_client;
	
	GreetWidget*	m_greetWidget;
	LoginWidget*	m_loginWidget;
	ChatsWidget*	m_chatsWidget;
};