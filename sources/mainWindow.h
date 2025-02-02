#pragma once

#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QEvent>
#include <QDir>


#include "loginWidget.h"
#include "chatsWidget.h"

class ClientSide;
std::string wideStringToString(const WCHAR* wideStr);
QString getSaveDir();

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
	void onLogin(bool isLoggedIn);

private:
	void setupLoginWidget();
	void setupChatsWidget();
	bool isDarkMode();

private:
	Theme			m_theme;
	ClientSide*		m_client;
	LoginWidget*	m_loginWidget;
	ChatsWidget*	m_chatsWidget;

};