#include "workerQt.h"
#include "chatsWidget.h"
#include "client.h"
#include "messagingAreaComponent.h"
#include "messageComponent.h"
#include "mainwindow.h"
#include "chatsListComponent.h"
#include "chatComponent.h"
#include "chatHeaderComponent.h"
#include "mainwindow.h"
#include "utility.h"
#include "base64.h"
#include "chat.h"
#include<chrono>
#include<string>

WorkerQt::WorkerQt(ChatsWidget* chatsWidget, Client* client) 
	: m_chats_widget(chatsWidget), m_client(client) {

}

void WorkerQt::onFriendInfoReceive(std::string packet) {
	while (m_client->chatsWidgetState == false) {

	}
	std::lock_guard<std::mutex> guard(m_mtx);

	std::istringstream iss(packet);
	std::string type;
	std::getline(iss, type);


	std::string login;
	std::getline(iss, login);

	std::string name;
	std::getline(iss, name);

	std::string isHasPhotoStr;
	std::getline(iss, isHasPhotoStr);
	bool isHasPhoto = isHasPhotoStr == "true";

	std::string photoSizeStr;
	std::getline(iss, photoSizeStr);
	size_t size = std::stoi(photoSizeStr);

	if (isHasPhoto) {
		std::string photoStr;
		std::getline(iss, photoStr);
		Photo* photo = Photo::deserialize(photoStr, size, login);

		auto& map = m_client->getMyChatsMap();
		auto it = map.find(login);
		if (it != map.end()) {
			Chat* chat = it->second;
			chat->setFriendPhoto(photo);
			chat->setIsFriendHasPhoto(true);
		}
		else {
			std::cout << "unexisting friend";
		}

		auto chatsList = m_chats_widget->getChatsList();
		auto& vec = chatsList->getChatComponentsVec();
		auto itComp = std::find_if(vec.begin(), vec.end(), [&login](ChatComponent* comp) {
			return comp->getChat()->getFriendLogin() == login;
			});
		
		if (itComp != vec.end()) {
			ChatComponent* chatComp = *itComp;
			QMetaObject::invokeMethod(chatComp,
				"setAvatar",
				Qt::QueuedConnection,
				Q_ARG(const QPixmap&, QPixmap(QString::fromStdString(photo->getPhotoPath()))));
		}

		auto& areasVec = m_chats_widget->getMessagingComponentsCacheVec();
		auto itArea = std::find_if(areasVec.begin(), areasVec.end(), [&login](MessagingAreaComponent* area) {
			return area->getChat()->getFriendLogin() == login;
			});

		if (itArea != areasVec.end()) {
			MessagingAreaComponent* area = *itArea;
			QMetaObject::invokeMethod(area,
				"setAvatar",
				Qt::QueuedConnection,
				Q_ARG(const QPixmap&, QPixmap(QString::fromStdString(photo->getPhotoPath()))));
		}
	}

}

void WorkerQt::onStatusReceive(std::string packet) {
	std::lock_guard<std::mutex> guard(m_mtx);

	std::istringstream iss(packet);
	std::string type;
	std::getline(iss, type);
	std::string login;
	std::getline(iss, login);
	std::string status;
	std::getline(iss, status);

	auto& compsVec = m_chats_widget->getMessagingComponentsCacheVec();

	auto itComp = std::find_if(compsVec.begin(), compsVec.end(), [&login](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == login;
		});

	if (itComp == compsVec.end()) {
		return;
	}
	MessagingAreaComponent* areaComp = *itComp;
	auto chatHeader  = areaComp->getChatHeader();

	QString date = Utility::parseDate(QString::fromStdString(status));

	QMetaObject::invokeMethod(chatHeader,
		"setLastSeen",
		Qt::QueuedConnection,
		Q_ARG(QString, date));


	std::unordered_map<std::string, Chat*>& chatsMap = m_client->getMyChatsMap();
	auto chatPair = chatsMap.find(login);
	Message* msg = nullptr;
	if (chatPair != chatsMap.end()) {
		Chat* chat = chatPair->second;
		chat->setFriendLastSeen(status);
	}
}

void WorkerQt::onMessageReceive(std::string packet) {

	bool isLocked = m_mtx.try_lock();

	while (m_client->chatsWidgetState == false) {

	}

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
	chat->setLastIncomeMsg(msg->getMessage());
	chat->getMessagesVec().push_back(msg);
	
	for (auto pair : chatsMap) {
		Chat* chatTmp = pair.second;
		if (chatTmp->getFriendLogin() == chat->getFriendLogin()) {
			continue;
		}
		else if (chatTmp->getLayoutIndex() < chat->getLayoutIndex()){
			chatTmp->setLayoutIndex(chatTmp->getLayoutIndex() + 1);
		}
	}
	chat->setLayoutIndex(0);

	ChatsListComponent* chatsList = m_chats_widget->getChatsList();
	auto& vec = chatsList->getChatComponentsVec();
	auto it = std::find_if(vec.begin(), vec.end(), [&friendLogin](ChatComponent* chatComp) {
		return chatComp->getChat()->getFriendLogin() == friendLogin;
		});

	if (it != vec.end()) {
		ChatComponent* chatComp = *it;

		QMetaObject::invokeMethod(chatsList,
			"popUpComponent",
			Qt::QueuedConnection,
			Q_ARG(ChatComponent*, chatComp));

		QMetaObject::invokeMethod(chatComp,
			"setLastMessage", 
			Qt::QueuedConnection,
			Q_ARG(const QString&, QString::fromStdString(message)));

		if (m_chats_widget->getCurrentMessagingArea() == nullptr) {
			QMetaObject::invokeMethod(chatComp,
				"setUnreadMessageDot",
				Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
		else if (m_chats_widget->getCurrentMessagingArea()->getChat()->getFriendLogin() != chat->getFriendLogin()) {
			QMetaObject::invokeMethod(chatComp,
				"setUnreadMessageDot",
				Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
	}

	auto& compsVec = m_chats_widget->getMessagingComponentsCacheVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
		});
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

		chat->getMessagesVec().back()->setIsRead(true);
	}

	if (isLocked) {
		m_mtx.unlock();
	}
}

void WorkerQt::onFirstMessageReceive(std::string packet) {
	while (m_client->chatsWidgetState == false) {

	}

	std::lock_guard<std::mutex> guard(m_mtx);

	std::unordered_map<std::string, Chat*>& chatsMap = m_client->getMyChatsMap();

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


	auto itChat = chatsMap.find(friendLogin);

	if (itChat != chatsMap.end()) {
		std::vector<std::string>& tmpLoginsVec = m_client->getVecToSendStatusTmp();
		auto it = std::find(tmpLoginsVec.begin(), tmpLoginsVec.end(), friendLogin);
		if (it != tmpLoginsVec.end()) {
			tmpLoginsVec.erase(it);
		}
		onMessageReceive(packet);
		return;
	}


	std::string friendName;
	std::getline(iss, friendName);

	std::string isHasPhotoStr;
	std::getline(iss, isHasPhotoStr);
	bool isHasPhoto = isHasPhotoStr == "true" ? true : false;

	std::string photoSizeStr;
	std::getline(iss, photoSizeStr);
	size_t size = std::stoi(photoSizeStr);

	std::string photoStr;
	std::getline(iss, photoStr);
	Photo* ph = Photo::deserialize(base64_decode(photoStr), size, friendLogin);

	Message* msg = new Message(message, timestamp, id, false);
	msg->setIsRead(false);

	Chat* chat = new Chat;
	chat->setFriendLastSeen("online");
	chat->setFriendLogin(friendLogin);
	chat->setFriendName(friendName);

	if (size > 0) {
		chat->setIsFriendHasPhoto(true);
	}
	else {
		chat->setIsFriendHasPhoto(false);
	}

	chat->setFriendPhoto(ph);
	chat->setLastIncomeMsg(message);
	chat->setLayoutIndex(0);

	auto& msgsVec = chat->getMessagesVec();
	msgsVec.push_back(msg);

	for (auto& pair : chatsMap) {
		Chat* chat = pair.second;
		chat->setLayoutIndex(chat->getLayoutIndex() + 1);
	}
	
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
	if (packet == "") {
		return;
	}

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
	std::lock_guard<std::mutex> guard(m_mtx);

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