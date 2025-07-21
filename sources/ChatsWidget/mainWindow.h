#pragma once
#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QSharedMemory>
#include <QScreen>
#include <QEvent>
#include <QDir>
#include <QTimer>
#include <QWindow>

#include <filesystem>

enum Theme;
class Client;
class GreetWidget;
class LoginWidget;
class ChatsWidget;
class ConfigManager;
class WorkerQt;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget* parent, Client* client, std::shared_ptr<ConfigManager> configManager);
	~MainWindow();

	LoginWidget* getLoginWidget() {return m_loginWidget; }
	ChatsWidget* getChatsWidget();


public slots:
	void setTheme(bool isDarkTheme) {
		m_theme = isDarkTheme ? DARK : LIGHT;
	}

	void showDoubleConnectionErrorDialog();
	void showConnectionErrorDialog();
	void showAlreadyRunningDialog();

	void updateRegistrationUIRedBorder();
	void updateAuthorizationUIRedBorder();
	void setupChatsWidget();
	void setupLoginWidget();
	void setupGreetWidget();
	void stopClient();

protected:
	void closeEvent(QCloseEvent* event) override;

private:
	Client* m_client;
	std::shared_ptr<ConfigManager> m_config_manager;

	WorkerQt*		m_worker_Qt;
	GreetWidget*	m_greetWidget;
	LoginWidget*	m_loginWidget;
	ChatsWidget*	m_chatsWidget;
	Theme			m_theme;
};