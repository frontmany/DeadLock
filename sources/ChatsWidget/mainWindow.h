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
	MainWindow(QWidget* parent);
	~MainWindow();

	
	void setWorkerUIonClient();		
	LoginWidget* getLoginWidget() {return m_loginWidget; }
	ChatsWidget* getChatsWidget();


public slots:
	void setTheme(bool isDarkTheme) {
		m_theme = isDarkTheme ? DARK : LIGHT;
	}

	void showConnectionErrorDialog();
	void showConfigLoadErrorDialog();
	void showAlreadyRunningDialog();

	void updateRegistrationUIRedBorder();
	void updateAuthorizationUIRedBorder();
	void setupChatsWidget();
	void setupLoginWidget();
	void setupGreetWidget();
	void stopClient();

	void setClient(Client* client) { 
		m_client = client;
	}
	Client* getClient() { return m_client; }

	void setConfigManager(std::shared_ptr<ConfigManager> configManager) { m_config_manager = configManager; }
	std::shared_ptr<ConfigManager> getConfigManager() { return m_config_manager; }

protected:
	void changeEvent(QEvent* event) override;
	void onWindowMinimized();
	void onWindowMaximized();
	void closeEvent(QCloseEvent* event) override;

private:
	void showDoubleConnectionErrorDialog();
	void safeShutdown();

	Client* m_client = nullptr;
	std::shared_ptr<ConfigManager> m_config_manager;

	WorkerQt*		m_worker_Qt = nullptr;
	GreetWidget*	m_greetWidget = nullptr;;
	LoginWidget*	m_loginWidget = nullptr;
	ChatsWidget*	m_chatsWidget = nullptr;
	Theme			m_theme;
};