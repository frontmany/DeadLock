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

namespace net {
	template <typename T>
	class message;
	template <typename T>
	class file;

}

class ResponseHandler {
public:
	ResponseHandler(Client* client, std::shared_ptr<ConfigManager> configManager);
	void setWorkerUI(WorkerUI* workerImpl);
	WorkerUI* getWorkerUI() { return m_worker_UI; }

	void handleResponse(net::message<QueryType>& msg);
	void handleFile(net::file<QueryType>& file);

	void onRegistrationSuccess(const std::string& packet);
	void onRegistrationFail();

	void onPasswordVerifySuccess();
	void onPasswordVerifyFail();

	void onCheckNewLoginSuccess(const std::string& packet);
	void onCheckNewLoginFail();

	void onAuthorizationSuccess(const std::string& packet);
	void onAuthorizationFail();

	void onChatCreateSuccess(const std::string& packet);
	void onChatCreateFail();

	void onMessageReceive(const std::string& packet);
	void onUserInfo(const std::string& packet);
	void onTyping(const std::string& packet);
	void onMessageReadConfirmationReceive(const std::string& packet);

	void onFilePreview(const std::string& packet);
	void processFriendsStatusesSuccess(const std::string& packet);
	void onFoundUsers(const std::string& packet);
	void onStatusReceive(const std::string& packet);

private:
	WorkerUI* m_worker_UI;
	Client* m_client;
	std::shared_ptr<ConfigManager> m_configManager;
};