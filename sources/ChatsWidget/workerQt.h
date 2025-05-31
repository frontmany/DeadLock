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
#include "fileWrapper.h"

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

	void onPasswordVerifySuccess() override;
	void onPasswordVerifyFail() override;

	void onCheckNewLoginSuccess() override;
	void onCheckNewLoginFail() override;

	void onChatCreateSuccess(Chat* chat) override;
	void onChatCreateFail() override;

	void processFoundUsers(std::vector<FriendInfo*>&& vec) override;
	void updateFriendsStatuses(const std::vector<std::pair<std::string, std::string>>& loginToStatusPairsVec) override;
	void showConfigLoadErrorDialog() override;
	void showTypingLabel(const std::string& friendLogin) override;
	void hideTypingLabel(const std::string& friendLogin) override;
	void onMessageReceive(const std::string& friendLogin, Message* message) override;
	void showNewChatOrUpdateExisting(Chat* chat) override;
	void onMessageReadConfirmationReceive(const std::string& friendLogin, const std::string& id) override;
	void updateFileLoadingState(const std::string& friendLogin, fileWrapper& fileWrapper, bool isError) override;

	void onStatusReceive(const std::string& friendLogin, const std::string& status);

private:
	bool updateExistingChatComp(ChatsWidget* chatsWidget, Chat* chat);
	bool updateExistingMessagingAreaComp(ChatsWidget* chatsWidget, Chat* chat);
	
private:
	MainWindow*  m_main_window;
};