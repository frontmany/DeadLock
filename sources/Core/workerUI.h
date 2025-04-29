#include <string>

class Message;
class Chat;

class WorkerUI {
public:
	virtual void onRegistrationSuccess() = 0;
	virtual void onRegistrationFail() = 0;

	virtual void onAuthorizationSuccess() = 0;
	virtual void onAuthorizationFail() = 0;

	virtual void onChatCreateSuccess(Chat* chat) = 0;
	virtual void onChatCreateFail() = 0;

	virtual void onPasswordVerifySuccess() = 0;
	virtual void onPasswordVerifyFail() = 0;

	virtual void onCheckNewLoginFail() = 0;
	virtual void onCheckNewLoginSuccess() = 0;

	virtual void updateFriendsStatuses(const std::vector<std::pair<std::string, std::string>>& loginToStatusPairsVec) = 0;
	virtual void showConfigLoadErrorDialog() = 0;
	virtual void onMessageReceive(const std::string& friendLogin, Message* message) = 0;
	virtual void showNewChatOrUpdateExisting(Chat* chat) = 0;
	virtual void onMessageReadConfirmationReceive(const std::string& friendLogin, const std::string& id) = 0;
	virtual void onStatusReceive(const std::string& friendLogin, const std::string& status) = 0;
};