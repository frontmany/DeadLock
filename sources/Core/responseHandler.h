#include <unordered_map>
#include <string>
#include <queue>
#include <sstream>
#include <iostream>
#include <filesystem> 
#include <fstream>

class WorkerUI;
class Client;
class ConfigManager;
class Chat;
enum class QueryType : uint32_t;

namespace net 
{
	class Packet;
}

class ResponseHandler {
public:
	ResponseHandler(Client* client, std::shared_ptr<ConfigManager> configManager);
	void setWorkerUI(WorkerUI* workerImpl);
	WorkerUI* getWorkerUI() { return m_worker_UI; }

	void handleResponse(net::Packet& packet);
	void onAvatar(net::File& file);
	void onFile(net::File& file);

	void onRegistrationSuccess(const std::string& packet);
	void onRegistrationFail();

	void onPasswordVerifySuccess();
	void onPasswordVerifyFail();

	void onCheckNewLoginSuccess(const std::string& packet);
	void onCheckNewLoginFail();

	void onAuthorizationSuccess(const std::string& packet);
	void onAuthorizationFail();

	void onReconnectSuccess();
	void onReconnectFail();

	void onChatCreateSuccess(const std::string& packet);
	void onChatCreateFail();

	void onAvatarsKey(const std::string& packet);
	void onUpdateOffer(const std::string& packet);
	void onMessageReceive(const std::string& packet);
	void onUserInfoSuccess(const std::string& packet);
	void onUserInfoFail(const std::string& packet);
	void onMyInfo(const std::string& packet);
	void onTyping(const std::string& packet);
	void onMessageReadConfirmationReceive(const std::string& packet);

	void onAvatarPreview(net::File& file);
	void onFilePreview(const std::string& packet);
	void processFriendsStatusesSuccess(const std::string& packet);
	void onFoundUsers(const std::string& packet);
	void onStatusReceive(const std::string& packet);

private:
	void processNewVersionLoadedFile(net::File file);
	void processRequestedFile(net::File& file);
	void addDataToMessage(Message* message, net::File& file, bool isPresent);
	void showFilesMessage(Message* message, const std::string& friendLogin, const std::string& myLogin);

private:
	WorkerUI* m_worker_UI;
	Client* m_client;
	std::shared_ptr<ConfigManager> m_configManager;
};