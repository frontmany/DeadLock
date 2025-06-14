#pragma once
#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QSharedMemory>
#include <QScreen>
#include <QEvent>
#include <QDir>

#include <filesystem>

enum Theme;
class Client;
class GreetWidget;
class LoginWidget;
class ChatsWidget;
class WorkerQt;

class OverlayWidget : public QWidget {
public:
	using QWidget::QWidget;
protected:
	void paintEvent(QPaintEvent*) override {
		setGeometry(QApplication::primaryScreen()->geometry());
		QPainter painter(this);
		painter.fillRect(rect(), QColor(25, 25, 25, 160));
	}
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	MainWindow(QWidget* parent, Client* client);
	~MainWindow();

	LoginWidget* getLoginWidget();
	ChatsWidget* getChatsWidget();


public slots:
	void showDoubleConnectionErrorDialog();
	void showConnectionErrorDialog();
	void showAlreadyRunningDialog();

	void updateRegistrationUIRedBorder();
	void updateAuthorizationUIRedBorder();
	void setupChatsWidget();
	void setupLoginWidget();
	void setupGreetWidget();

	//logic
	void stopClient();

private:
	WorkerQt*		m_worker_Qt;
	Client*			m_client;
	GreetWidget*	m_greetWidget;
	LoginWidget*	m_loginWidget;
	ChatsWidget*	m_chatsWidget;
	Theme			m_theme;
};