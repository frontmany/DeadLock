#pragma once
#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QEvent>
#include <QDir>

#include <filesystem>

enum Theme;
class Client;
class GreetWidget;
class LoginWidget;
class ChatsWidget;
class WorkerQt;


class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget* parent, Client* client);
	~MainWindow();

public slots:
	void showConnectionErrorDialog();
	void updateRegistrationUIRedBorder();
	void updateAuthorizationUIRedBorder();
	void setupChatsWidget();
	void setupLoginWidget();
	void setupGreetWidget();

	LoginWidget* getLoginWidget();
	ChatsWidget* getChatsWidget();


private:
	WorkerQt*		m_worker_Qt;
	Client*			m_client;
	GreetWidget*	m_greetWidget;
	LoginWidget*	m_loginWidget;
	ChatsWidget*	m_chatsWidget;
	Theme			m_theme;
};