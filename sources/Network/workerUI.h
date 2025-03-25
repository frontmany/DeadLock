#include <string>

class Message;

class WorkerUI {
public:
	virtual void onStatusReceive(std::string packet) = 0;
	virtual void onFriendInfoReceive(std::string packet) = 0;
	virtual void onMessageReceive(std::string packet) = 0;
	virtual void onFirstMessageReceive(std::string packet) = 0;
	virtual void onMessageReadConfirmationReceive(std::string packet) = 0;
	virtual void onAuthorization(std::string packet) = 0;
};