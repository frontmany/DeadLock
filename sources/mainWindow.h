#pragma once

#include <Qmainwindow>
#include <QScrollArea>
#include <Qlayout>
#include <QDebug>
#include <QEvent>
#include <windows.h>


#include "loginWidget.h"


class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

	void setAuthorizationWidget();
	void setRegistrationWidget();
	void setChatMenu();


private:
	void setupLoginWidget();

private:
	LoginWidget* loginWidget;
	bool isDarkMode();

};