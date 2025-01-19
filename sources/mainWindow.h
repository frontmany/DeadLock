#pragma once

#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QEvent>


#include "loginWidget.h"

class ClientSide;

class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent, ClientSide* client);
	~MainWindow();

	void setAuthorizationWidget();
	void setRegistrationWidget();
	void setChatMenu();


public slots:
	void onLogin(bool isLoggedIn);

private:
	void setupLoginWidget();

private:
	ClientSide* m_client = nullptr;
	LoginWidget* m_loginWidget;
	bool isDarkMode();

};