#include <unordered_map>
#include <string>
#include <mutex>
#include <queue>
#include <sstream>

class WorkerUI;
class Client;
class Chat;
enum class QueryType : uint32_t;

namespace net {
	template <typename T>
	class owned_message;
}

typedef net::owned_message<QueryType> ownedMessageT;

class ResponseHandler {
public:
	ResponseHandler(Client* client);
	void setWorkerUI(WorkerUI* workerImpl);
	WorkerUI* getWorkerUI() { return m_worker_UI; }


	void handleResponse(ownedMessageT& msg);

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



	void processFriendsStatusesSuccess(const std::string& packet);
	void processFoundUsers(const std::string& packet);
	void onStatusReceive(const std::string& packet);

private:
	WorkerUI* m_worker_UI;
	Client* m_client;
};