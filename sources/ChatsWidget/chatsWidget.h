#pragma once

#include <QWidget>
#include <QLayout>



class ChatsListComponent;
class MainWindow;
class ClientSide;
enum Theme;

class ChatsWidget : public QWidget {

public:
	ChatsWidget(QWidget* parent, ClientSide* client);
	~ChatsWidget();
	void setTheme(Theme theme);

public slots:
	void onCreateChatButtonClicked(QString login);

private:
	void paintEvent(QPaintEvent* event) override;
	void setBackGround(Theme theme);

private:
	QPixmap					m_background;
	ClientSide*				m_client;
	ChatsListComponent*		m_chatsListComponent;
	QVBoxLayout*			m_mainVLayout;

};