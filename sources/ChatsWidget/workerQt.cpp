#include "workerQt.h"
#include "chatsWidget.h"
#include "client.h"
#include "messagingAreaComponent.h"
#include "messageComponent.h"
#include "mainwindow.h"
#include "chatsListComponent.h"
#include "chatHeaderComponent.h"
#include "mainwindow.h"
#include "chat.h"
#include<chrono>
#include<string>

WorkerQt::WorkerQt(ChatsWidget* chatsWidget, Client* client) 
	: m_chats_widget(chatsWidget), m_client(client) {

}

void WorkerQt::onStatusReceive(std::string packet) {
	std::istringstream iss(packet);
	std::string type;
	std::getline(iss, type);
	std::string login;
	std::getline(iss, login);
	std::string status;
	std::getline(iss, status);

	auto& compsVec = m_chats_widget->getMessagingComponentsCacheVec();

	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&login](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == login;
		});

	MessagingAreaComponent* areaComp = *comp;
	auto chatHeader  = areaComp->getChatHeader();

	QMetaObject::invokeMethod(chatHeader,
		"setLastSeen",
		Qt::QueuedConnection,
		Q_ARG(QString, QString::fromStdString(status)));


	std::unordered_map<std::string, Chat*>& chatsMap = m_client->getMyChatsMap();
	auto chatPair = chatsMap.find(login);
	Message* msg = nullptr;
	if (chatPair != chatsMap.end()) {
		Chat* chat = chatPair->second;
		chat->setFriendLastSeen(status);
	}
}

void WorkerQt::onMessageReceive(std::string packet) {
	std::istringstream iss(packet);
	std::string type;
	std::getline(iss, type);

	std::string myLogin;
	std::getline(iss, myLogin);

	std::string type2;
	std::getline(iss, type2);

	std::string friendLogin;
	std::getline(iss, friendLogin);

	std::string messageBegin;
	std::getline(iss, messageBegin);

	std::string message;
	std::string line;
	while (std::getline(iss, line)) {
		if (line == "MESSAGE_END") {
			break;
		}
		else {
			message += line;
		}
	}
	std::string id;
	std::getline(iss, id);

	std::string timestamp;
	std::getline(iss, timestamp);

	std::unordered_map<std::string, Chat*>& chatsMap = m_client->getMyChatsMap();
	auto chatPair = chatsMap.find(friendLogin);

	Message* msg = new Message(message, timestamp, id, false);
	msg->setIsRead(false);
	
	Chat* chat = chatPair->second;
	chat->getMessagesVec().push_back(msg);

	auto& compsVec = m_chats_widget->getMessagingComponentsCacheVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
		});


	ChatsListComponent* chatsList = m_chats_widget->getChatsList();
	auto& vec = chatsList->getChatComponentsVec();
	auto it = std::find_if(vec.begin(), vec.end(), [&friendLogin](ChatComponent* chatComp) {
		return chatComp->getChat()->getFriendLogin() == friendLogin;
		});

	if (it != vec.end()) {
		ChatComponent* chatComp = *it;
		QMetaObject::invokeMethod(chatComp,
			"setLastMessage", 
			Qt::QueuedConnection,
			Q_ARG(const QString&, QString::fromStdString(message)));
	}

	MessagingAreaComponent* areaComp = *comp;
	QMetaObject::invokeMethod(areaComp,
		"addMessage",
		Qt::QueuedConnection,
		Q_ARG(Message*, msg));
	
	if (m_chats_widget->getCurrentMessagingArea() == nullptr) {}
	else if (m_chats_widget->getCurrentMessagingArea()->getChat()->getFriendLogin() == chat->getFriendLogin()) {
		MessagingAreaComponent* areaComp = *comp;
		QMetaObject::invokeMethod(areaComp,
			"markMessageAsChecked",
			Qt::QueuedConnection,
			Q_ARG(Message*, msg));
	}
}

void WorkerQt::onFirstMessageReceive(std::string packet) {
	std::istringstream iss(packet);
	std::string type;
	std::getline(iss, type);

	std::string myLogin;
	std::getline(iss, myLogin);

	std::string type2;
	std::getline(iss, type2);

	std::string friendLogin;
	std::getline(iss, friendLogin);

	std::string messageBegin;
	std::getline(iss, messageBegin);

	std::string message;
	std::string line;
	while (std::getline(iss, line)) {
		if (line == "MESSAGE_END") {
			break;
		}
		else {
			message += line;
		}
	}
	std::string id;
	std::getline(iss, id);

	std::string timestamp;
	std::getline(iss, timestamp);

	std::string friendName;
	std::getline(iss, friendName);

	std::string isHasPhotoStr;
	std::getline(iss, isHasPhotoStr);
	bool isHasPhoto = isHasPhotoStr == "true" ? true : false;

	std::string photoStr;
	std::getline(iss, photoStr);
	Photo* ph = Photo::deserialize(photoStr);

	Message* msg = new Message(message, timestamp, id, false);
	msg->setIsRead(false);

	Chat* chat = new Chat;
	chat->setFriendLastSeen("online");
	chat->setFriendLogin(friendLogin);
	chat->setFriendName(friendName);
	chat->setIsFriendHasPhoto(isHasPhoto);
	chat->setFriendPhoto(ph);
	chat->setLastIncomeMsg(message);
	auto& msgsVec = chat->getMessagesVec();
	msgsVec.push_back(msg);

	std::unordered_map<std::string, Chat*>& chatsMap = m_client->getMyChatsMap();
	chatsMap[friendLogin] = chat;

	std::vector<std::string>& tmpLoginsVec = m_client->getVecToSendStatusTmp();

	auto it = std::find(tmpLoginsVec.begin(), tmpLoginsVec.end(), friendLogin);
	if (it != tmpLoginsVec.end()) {
		tmpLoginsVec.erase(it);
	}
	
	QMetaObject::invokeMethod(m_chats_widget,
		"createMessagingComponent",
		Qt::QueuedConnection,
		Q_ARG(std::string, friendLogin),
		Q_ARG(Chat*, chat));

	ChatsListComponent* chatsList = m_chats_widget->getChatsList();
	QMetaObject::invokeMethod(chatsList,
		"addChatComponent",
		Qt::QueuedConnection,
		Q_ARG(Theme, m_chats_widget->getTheme()),
		Q_ARG(Chat*, chat),
		Q_ARG(bool, false));
}

void WorkerQt::onAuthorization(std::string packet) {
	std::unordered_map<std::string, Chat*>& chatsMap = m_client->getMyChatsMap();

	std::istringstream iss(packet);
	std::string line;

	std::getline(iss, line); // vecBegin

	while (std::getline(iss, line)){
		size_t pos = line.find(',');
		std::string login = line.substr(0, pos);
		std::string status = line.substr(++pos);

		auto chatPair = chatsMap.find(login);
		if (chatPair == chatsMap.end()) {
			continue;
		}
		else {
			Chat* chat = chatPair->second;
			chat->setFriendLastSeen(status);
		}
	}
}

void WorkerQt::onMessageReadConfirmationReceive(std::string packet) {
	std::istringstream iss(packet);

	std::string type;
	std::getline(iss, type);

	std::string myLogin;
	std::getline(iss, myLogin);

	std::getline(iss, type);

	std::string friendLogin;
	std::getline(iss, friendLogin);

	std::string vecBegin;
	std::getline(iss, vecBegin);

	std::unordered_map<std::string, Chat*>& chatsMap = m_client->getMyChatsMap();
	auto chatPair = chatsMap.find(friendLogin);
	Chat* chat;
	std::vector<Message*> messagesVec;
	if (chatPair != chatsMap.end()) {
		chat = chatPair->second;
		messagesVec = chat->getMessagesVec();
	}

	auto& compsVec = m_chats_widget->getMessagingComponentsCacheVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
		});
	MessagingAreaComponent* areaComp = *comp;
	auto& messagesCompsVec = areaComp->getMessagesComponentsVec();

	std::string id;
	while (std::getline(iss, id)) {
		if (id == "VEC_END") {
			break;
		}
		else {
			auto msg_chat = std::find_if(messagesVec.begin(), messagesVec.end(), [&id](Message* msg) {
				return msg->getId() == id;
				});
			Message* msg = *msg_chat;
			msg->setIsRead(true);

			auto msg_comp = std::find_if(messagesCompsVec.begin(), messagesCompsVec.end(), [&id](MessageComponent* comp) {
				return comp->getId() == QString::fromStdString(id);
				});
			MessageComponent* messageComp = *msg_comp;

			QMetaObject::invokeMethod(messageComp,
				"setIsRead",
				Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
	}
}