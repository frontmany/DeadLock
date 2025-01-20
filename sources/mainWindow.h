#pragma once

#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QEvent>


#include "loginWidget.h"
#include "chatsWidget.h"

class ClientSide;

enum Theme {
	DARK,
	LIGHT
};

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent, ClientSide* client);
	~MainWindow();



public slots:
	void onLogin(bool isLoggedIn);

private:
	void setupLoginWidget();
	void setupChatsWidget();

private:
	ClientSide*		m_client = nullptr;
	LoginWidget*	m_loginWidget;
	ChatsWidget*	m_chatsWidget;
	bool isDarkMode();

};