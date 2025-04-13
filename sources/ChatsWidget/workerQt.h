#include <vector>
#include <mutex>
#include <QPixmap>
#include <QHBoxLAyout>
#include <QWidget>
#include <QString>
#include <QMessageBox>
#include <chrono>
#include <string>

#include "workerUI.h"

class MainWindow;
class ChatsWidget;
class Chat;

class WorkerQt : public WorkerUI {
public:
	WorkerQt(MainWindow* mainWindow);

	void onRegistrationSuccess() override;
	void onRegistrationFail() override;

	void onAuthorizationSuccess() override;
	void onAuthorizationFail() override;

	void onChatCreateSuccess(Chat* chat) override;
	void onChatCreateFail() override;

	void updateFriendsStatuses(const std::vector<std::pair<std::string, std::string>>& loginToStatusPairsVec) override;
	void showConfigLoadErrorDialog() override;
	void onMessageReceive(const std::string& friendLogin, Message* message) override;
	void showNewChatOrUpdateExisting(Chat* chat) override;
	void onMessageReadConfirmationReceive(const std::string& friendLogin, const std::string& id) override;
	void onStatusReceive(const std::string& friendLogin, const std::string& status);

private:
	bool updateExistingChatComp(ChatsWidget* chatsWidget, Chat* chat);
	bool updateExistingMessagingAreaComp(ChatsWidget* chatsWidget, Chat* chat);
	
private:
	MainWindow*  m_main_window;
};