#include<vector>
#include<mutex>
#include<QPixmap>
#include "workerUI.h"

class ChatsWidget;
class Client;
class Message;

class WorkerQt : public WorkerUI {
public:
	WorkerQt(ChatsWidget* chatsWidget, Client* client);

	void onStatusReceive(std::string packet) override;
	void onMessageReceive(std::string packet) override;
	void onFirstMessageReceive(std::string packet) override;
	void onMessageReadConfirmationReceive(std::string packet) override;
	void onAuthorization(std::string packet) override;
	void onFriendInfoReceive(std::string packet);

private:
	std::mutex	 m_mtx;
	ChatsWidget* m_chats_widget;
	Client*		 m_client;
};