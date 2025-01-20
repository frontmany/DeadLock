#pragma once

#include <QWidget>
#include <QLayout>



class ChatsListComponent;
class MainWindow;
enum Theme;

class ChatsWidget : public QWidget {

public:
	ChatsWidget(QWidget* parent);
	~ChatsWidget();
	void setTheme(Theme theme);

private:
	void paintEvent(QPaintEvent* event) override;
	void setBackGround(Theme theme);

private:
	QPixmap         m_background;

	ChatsListComponent* m_chatsListComponent;
	QVBoxLayout* m_mainVLayout;

};