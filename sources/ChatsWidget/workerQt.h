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
class Photo;

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
	void blockProfileEditing() override;
	void activateProfileEditing() override;
	void updateFriendsStatuses(const std::vector<std::pair<std::string, std::string>>& loginToStatusPairsVec) override;
	void showConfigLoadErrorDialog() override;
	void showTypingLabel(const std::string& friendLogin) override;
	void showNowReceiving(const std::string& friendLoginHash) override;
	void hideTypingLabel(const std::string& friendLogin) override;
	void onMessageReceive(const std::string& friendLogin, Message* message) override;
	void showNewChatOrUpdateExisting(Chat* chat) override;
	void onMessageReadConfirmationReceive(const std::string& friendLogin, const std::string& id) override;

	void updateFileLoadingState(const std::string& friendLoginHash, fileWrapper& fileWrapper, bool isError) override;
	void updateFileLoadingProgress(const std::string& friendLogin, const net::file<QueryType>& file, uint32_t progressPercent) override;
	void updateFileSendingProgress(const std::string& friendLoginHash, const net::file<QueryType>& file, uint32_t progressPercent) override;

	void onMessageSendingError(const std::string& friendLogin, Message* message) override;
	void onRequestedFileError(const std::string& friendLoginHash, fileWrapper fileWrapper) override;
	void onConnectError() override;
	void onNetworkError() override;
	void onServerDown() override;
	void onStatusReceive(const std::string& friendLoginHash, const std::string& status);
	void supplyTheme(bool isDarkTheme) override;
	void showUpdateButton() override;
	void updateAndRestart() override;

	void setRecoveredAvatar(Photo* myRecoveredAvatar) override;
	void setNameFieldInProfileEditorWidget(const std::string& name) override;

private:
	bool updateExistingChatComp(ChatsWidget* chatsWidget, Chat* chat);
	bool updateExistingMessagingAreaComp(ChatsWidget* chatsWidget, Chat* chat);
	
private:
	MainWindow*  m_main_window;
};