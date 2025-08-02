#include <string>
#include <vector>

#include "queryType.h"

class Message;
class FriendInfo;
class Chat;
class Photo;
class fileWrapper;

namespace net {
	class File;
}

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

	virtual void updateFriendAvatarPreview(Avatar* avatar) = 0;
	virtual void updateFriendAvatar(Avatar* avatar, const std::string& friendLogin) = 0;
	virtual void showConnectionDownLabel() = 0;
	virtual void setupRegistrationWidget() = 0;
	virtual void removeConnectionErrorLabel() = 0;
	virtual void processFoundUsers(std::vector<FriendInfo*>&& vec) = 0;
	virtual void blockProfileEditing() = 0;
	virtual void activateProfileEditing() = 0;
	virtual void updateFriendsStatuses(const std::vector<std::pair<std::string, std::string>>& loginToStatusPairsVec) = 0;
	virtual void initializeUIWithConfigErrorDialog() = 0;
	virtual void showTypingLabel(const std::string& friendLogin) = 0;
	virtual void showNowReceiving(const std::string& friendLoginHash) = 0;
	virtual void hideTypingLabel(const std::string& friendLogin) = 0;
	virtual void onMessageReceive(const std::string& friendLogin, Message* message) = 0;
	virtual void showNewChatOrUpdateExisting(Chat* chat) = 0;
	virtual void onMessageReadConfirmationReceive(const std::string& friendLogin, const std::string& id) = 0;
	virtual void onStatusReceive(const std::string& friendLogin, const std::string& status) = 0;
	virtual void supplyTheme(bool isDarkTheme) = 0;
	virtual void showUpdateButton() = 0;
	virtual void updateAndRestart() = 0;

	virtual void updateFileLoadingState(const std::string& friendLoginHash, fileWrapper& fileWrapper, bool isError) = 0;
	virtual void updateFileLoadingProgress(const std::string& friendLoginHash, const net::File& file, uint32_t progressPercent) = 0;
	virtual void updateFileSendingProgress(const std::string& friendLoginHash, const net::File& file, uint32_t progressPercent) = 0;

	virtual void onMessageSendingError(const std::string& friendLogin, Message* message) = 0;
	virtual void onRequestedFileError(const std::string& friendLoginHash, fileWrapper wrapper) = 0;
	virtual void onConnectionDown() = 0;

	virtual void setNameFieldInProfileEditorWidget(const std::string& name) = 0;
	virtual void setRecoveredAvatar(Photo* myRecoveredAvatar) = 0;
};