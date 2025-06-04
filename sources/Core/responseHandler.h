#include <unordered_map>
#include <string>
#include <queue>
#include <sstream>
#include <iostream>
#include <filesystem> 
#include <fstream>

class WorkerUI;
class Client;
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
	ResponseHandler(Client* client);
	void setWorkerUI(WorkerUI* workerImpl);
	WorkerUI* getWorkerUI() { return m_worker_UI; }


	void handleResponse(net::message<QueryType>& msg);
	void handleFile(const net::file<QueryType>& file);


	void onRegistrationSuccess();
	void onRegistrationFail();

	void onPasswordVerifySuccess();
	void onPasswordVerifyFail();

	void onCheckNewLoginSuccess(const std::string& packet);
	void onCheckNewLoginFail();

	void onAuthorizationSuccess();
	void onAuthorizationFail();

	void onChatCreateSuccess(const std::string& packet);
	void onChatCreateFail();

	void onMessageReceive(const std::string& packet);
	void onUserInfo(const std::string& packet);
	void onTyping(const std::string& packet);
	void onMessageReadConfirmationReceive(const std::string& packet);

	//new 
	void prepareToReceiveFile(const std::string& packet);
	void prepareToReceiveRequestedFile(const std::string& packet);
	void onFilePreview(const std::string& packet);
	

	void processFriendsStatusesSuccess(const std::string& packet);
	void processFoundUsers(const std::string& packet);
	void onStatusReceive(const std::string& packet);

	bool getIsThisFileRequested() { return m_is_received_file_requested; }

private:
	WorkerUI* m_worker_UI;
	Client* m_client;
	bool m_is_received_file_requested = false;
};