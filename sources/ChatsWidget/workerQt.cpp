#include "workerQt.h"
#include "chatsWidget.h"
#include "messagingAreaComponent.h"
#include "messageComponent.h"
#include "mainwindow.h"
#include "chatsListComponent.h"
#include "addChatDialogComponent.h"
#include "fieldsEditComponent.h"
#include "passwordEditorComponent.h"
#include "chatComponent.h"
#include "chatHeaderComponent.h"
#include "helloAreaComponent.h"
#include "registrationComponent.h"
#include "authorizationComponent.h"
#include "profileEditorWidget.h"
#include "loginWidget.h"
#include "utility.h"
#include "chat.h"
#include "friendInfo.h"
#include "friendSearchDialogComponent.h"
#include "photo.h"




WorkerQt::WorkerQt(MainWindow* mainWindow)
	: m_main_window(mainWindow) {
}

void WorkerQt::onRegistrationSuccess() {
	QMetaObject::invokeMethod(m_main_window, "setupGreetWidget", Qt::QueuedConnection);
}

void WorkerQt::onRegistrationFail() {
	QMetaObject::invokeMethod(m_main_window, "updateRegistrationUIRedBorder", Qt::QueuedConnection);
}

void WorkerQt::onAuthorizationSuccess() {
	QMetaObject::invokeMethod(m_main_window, "setupChatsWidget", Qt::QueuedConnection);
}

void WorkerQt::onAuthorizationFail() {
	QMetaObject::invokeMethod(m_main_window, "updateAuthorizationUIRedBorder", Qt::QueuedConnection);
}

void WorkerQt::onPasswordVerifySuccess() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsListComponent = chatsWidget->getChatsList();
	ProfileEditorWidget* profileEditorWidget = chatsListComponent->getProfileEditorWidget();
	
	if (profileEditorWidget != nullptr) {
		PasswordEditComponent* passwordEditComponent = profileEditorWidget->getPasswordEditComponent();
		QMetaObject::invokeMethod(passwordEditComponent,
			"showNewPasswordInput",
			Qt::QueuedConnection);
	}
	
}
void WorkerQt::onPasswordVerifyFail() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsListComponent = chatsWidget->getChatsList();
	ProfileEditorWidget* profileEditorWidget = chatsListComponent->getProfileEditorWidget();

	if (profileEditorWidget != nullptr) {
		PasswordEditComponent* passwordEditComponent = profileEditorWidget->getPasswordEditComponent();
		QMetaObject::invokeMethod(passwordEditComponent,
			"showErrorLabelPasswordInput",
			Qt::QueuedConnection);
	}

}

void WorkerQt::onChatCreateSuccess(Chat* chat) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	QMetaObject::invokeMethod(chatsWidget, "removeRightComponent", Qt::QueuedConnection);
	QMetaObject::invokeMethod(chatsWidget, "createAndSetMessagingAreaComponent", Qt::QueuedConnection, Q_ARG(Chat*, chat));
	QMetaObject::invokeMethod(chatsWidget, "createAndAddChatComponentToList", Qt::QueuedConnection, Q_ARG(Chat*, chat));
	QMetaObject::invokeMethod(chatsWidget, "closeAddChatDialog", Qt::QueuedConnection);
}


void WorkerQt::onChatCreateFail() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	QMetaObject::invokeMethod(chatsWidget, [chatsWidget]() {
		if (auto chatsList = chatsWidget->getChatsList()) {
			if (auto addDialog = chatsList->getAddChatDialogComponent()) {
				if (auto editComp = addDialog->getEditComponent()) {
					editComp->setRedBorderToChatAddDialog();
				}
			}
		}
	}, Qt::QueuedConnection);
}

void WorkerQt::showConfigLoadErrorDialog() {
	QMetaObject::invokeMethod(m_main_window, []() {
		QMessageBox msgBox;
		msgBox.setWindowTitle("Configuration loading error");
		msgBox.setIcon(QMessageBox::Critical);

		QString errorMessage = "The configuration could not be loaded.\n"
			"Possible causes:\n"
			"- The configuration file is corrupted\n"
			"- No access rights\n"
			"- File not found\n"
			"- Invalid data format";

		msgBox.setText(errorMessage);
		msgBox.setStandardButtons(QMessageBox::Ok);
		msgBox.exec();
		}, Qt::QueuedConnection);
}

void WorkerQt::updateFriendsStatuses(const std::vector<std::pair<std::string, std::string>>& loginToStatusPairsVec) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	QMetaObject::invokeMethod(chatsWidget, [chatsWidget, loginToStatusPairsVec]() {
		auto& messagingAreaCompsVec = chatsWidget->getMessagingAreasVec();

		for (auto messagingArea : messagingAreaCompsVec) {
			auto chatHeader = messagingArea->getChatHeader();
			std::string messagingAreaFriendLogin = messagingArea->getChat()->getFriendLogin();

			auto itPair = std::find_if(loginToStatusPairsVec.begin(), loginToStatusPairsVec.end(),
				[&messagingAreaFriendLogin](const auto& pair) {
					return pair.first == messagingAreaFriendLogin;
				});

			if (itPair != loginToStatusPairsVec.end()) {
				chatHeader->setLastSeen(QString::fromStdString(itPair->second));
			}
		}
		}, Qt::QueuedConnection);
}

void WorkerQt::onMessageReceive(const std::string& friendLogin, Message* message) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsList = chatsWidget->getChatsList();
	auto& vec = chatsList->getChatComponentsVec();
	auto it = std::find_if(vec.begin(), vec.end(), [friendLogin](ChatComponent* chatComp) {
		return chatComp->getChat()->getFriendLogin() == friendLogin;
		});

	if (it == vec.end()) {
		return;
	}

	ChatComponent* chatComp = *it;
	if (chatComp->getChat()->getLayoutIndex() != 0){
		QMetaObject::invokeMethod(chatsList,
			"popUpComponent",
			Qt::QueuedConnection,
			Q_ARG(ChatComponent*, chatComp));
	}

	QMetaObject::invokeMethod(chatComp,
		"setLastMessage",
		Qt::QueuedConnection,
		Q_ARG(const QString&, QString::fromStdString(message->getMessage())));

	if (chatsWidget->getCurrentMessagingAreaComponent() == nullptr) {
		QMetaObject::invokeMethod(chatComp,
			"setUnreadMessageDot",
			Qt::QueuedConnection,
			Q_ARG(bool, true));
	}
	else if (chatsWidget->getCurrentMessagingAreaComponent()->getChat()->getFriendLogin() != friendLogin) {
		QMetaObject::invokeMethod(chatComp,
			"setUnreadMessageDot",
			Qt::QueuedConnection,
			Q_ARG(bool, true));
	}


	auto& compsVec = chatsWidget->getMessagingAreasVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
		});
	MessagingAreaComponent* areaComp = *comp;

	QMetaObject::invokeMethod(areaComp,
		"addMessage",
		Qt::QueuedConnection,
		Q_ARG(Message*, message),
		Q_ARG(bool, false));

	if (chatsWidget->getCurrentMessagingAreaComponent() == nullptr) {}
	else if (chatsWidget->getCurrentMessagingAreaComponent()->getChat()->getFriendLogin() == friendLogin) {
		MessagingAreaComponent* areaComp = *comp;

		if (!message->getIsSend() &&
			qAbs(areaComp->getScrollArea()->verticalScrollBar()->value() -
				areaComp->getScrollArea()->verticalScrollBar()->maximum()) < 15) {

			QMetaObject::invokeMethod(areaComp,
				"moveSliderDown",
				Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
	}
}

void WorkerQt::showNewChatOrUpdateExisting(Chat* chat) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();

	bool isExistingChat = updateExistingChatComp(chatsWidget, chat);
	bool isExistingChatSecondCheck = updateExistingMessagingAreaComp(chatsWidget, chat);

	if (isExistingChat && isExistingChatSecondCheck) {
		return;
	}

	QMetaObject::invokeMethod(chatsWidget,
		"createMessagingComponent",
		Qt::QueuedConnection,
		Q_ARG(std::string, chat->getFriendLogin()),
		Q_ARG(Chat*, chat));

	ChatsListComponent* chatsList = chatsWidget->getChatsList();
	QMetaObject::invokeMethod(chatsList,
		"addChatComponent",
		Qt::QueuedConnection,
		Q_ARG(Theme, chatsWidget->getTheme()),
		Q_ARG(Chat*, chat),
		Q_ARG(bool, false));
}

void WorkerQt::onMessageReadConfirmationReceive(const std::string& friendLogin, const std::string& id) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& messagingAreaCompsVec = chatsWidget->getMessagingAreasVec();
	auto messagingAreaIt = std::find_if(messagingAreaCompsVec.begin(), messagingAreaCompsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
		});

	if (messagingAreaIt != messagingAreaCompsVec.end()) {
		MessagingAreaComponent* messagingArea = *messagingAreaIt;
		auto& messagesCompsVec = messagingArea->getMessagesComponentsVec();

		auto messageCompIt = std::find_if(messagesCompsVec.begin(), messagesCompsVec.end(), [&id](MessageComponent* comp) {
			return comp->getId() == QString::fromStdString(id);
			});

		MessageComponent* messageComp = *messageCompIt;
		QMetaObject::invokeMethod(messageComp,
			"setIsRead",
			Qt::QueuedConnection,
			Q_ARG(bool, true));
	}
}

void WorkerQt::onStatusReceive(const std::string& friendLogin, const std::string& status) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& messagingAreaCompsVec = chatsWidget->getMessagingAreasVec();

	auto messagingAreaIt = std::find_if(messagingAreaCompsVec.begin(), messagingAreaCompsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
		});

	if (messagingAreaIt != messagingAreaCompsVec.end()) {
		MessagingAreaComponent* areaComp = *messagingAreaIt;
		auto chatHeader = areaComp->getChatHeader();
		std::string date = utility::parseDate(status);

		QMetaObject::invokeMethod(chatHeader,
			"setLastSeen",
			Qt::QueuedConnection,
			Q_ARG(QString, QString::fromStdString(date)));
	}
}


bool  WorkerQt::updateExistingChatComp(ChatsWidget* chatsWidget, Chat* chat) {
	auto chatsList = chatsWidget->getChatsList();
	auto& chatsComponentsVec = chatsList->getChatComponentsVec();

	auto chatCompIt = std::find_if(chatsComponentsVec.begin(), chatsComponentsVec.end(), [chat](ChatComponent* chatComp) {
		return chat->getFriendLogin() == chatComp->getChat()->getFriendLogin();
		});

	if (chatCompIt != chatsComponentsVec.end()) {
		ChatComponent* chatComp = *chatCompIt;

		QMetaObject::invokeMethod(chatComp,
			"setName",
			Qt::QueuedConnection,
			Q_ARG(const QString&, QString::fromStdString(chat->getFriendName())));

		const std::string& photoPath = chat->getFriendPhoto()->getPhotoPath();
		if (photoPath != "") {
			QPixmap pixMap(QString::fromStdString(photoPath));
			QMetaObject::invokeMethod(chatComp,
				"setAvatar",
				Qt::QueuedConnection,
				Q_ARG(QPixmap, pixMap));
		}
		

		return true;
	}

	return false;
}

void WorkerQt::processFoundUsers(std::vector<FriendInfo*>&& vec) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto chatsList = chatsWidget->getChatsList();
	auto friendSearchDialogComponent = chatsList->getFriendSearchDialogComponent();
	
	QMetaObject::invokeMethod(friendSearchDialogComponent,
		"refreshFriendsList",
		Qt::QueuedConnection,
		Q_ARG(const std::vector<FriendInfo*>&, std::move(vec)));
	
}

bool WorkerQt::updateExistingMessagingAreaComp(ChatsWidget* chatsWidget, Chat* chat) {
	auto& messagingAreasVec = chatsWidget->getMessagingAreasVec();
	auto messagingAreaIt = std::find_if(messagingAreasVec.begin(), messagingAreasVec.end(), [chat](MessagingAreaComponent* messagingAreaComp) {
		return chat->getFriendLogin() == messagingAreaComp->getChat()->getFriendLogin();
		});


	if (messagingAreaIt != messagingAreasVec.end()) {
		MessagingAreaComponent* messagingArea = *messagingAreaIt;


		QMetaObject::invokeMethod(messagingArea,
			"setName",
			Qt::QueuedConnection,
			Q_ARG(const QString&, QString::fromStdString(chat->getFriendName())));

		const std::string& photoPath = chat->getFriendPhoto()->getPhotoPath();
		if (photoPath != "") {
			QPixmap pixMap(QString::fromStdString(photoPath));
			QMetaObject::invokeMethod(messagingArea,
				"setAvatar",
				Qt::QueuedConnection,
				Q_ARG(QPixmap, pixMap));
		}
		return true;
	}

	return false;
}



void WorkerQt::onCheckNewLoginSuccess() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsListComponent = chatsWidget->getChatsList();
	ProfileEditorWidget* profileEditorWidget = chatsListComponent->getProfileEditorWidget();
	QMetaObject::invokeMethod(profileEditorWidget,
		"close",
		Qt::QueuedConnection);
}

void WorkerQt::onCheckNewLoginFail() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsListComponent = chatsWidget->getChatsList();
	ProfileEditorWidget* profileEditorWidget = chatsListComponent->getProfileEditorWidget();

	if (profileEditorWidget != nullptr) {
		auto fieldsEditComponent = profileEditorWidget->getFieldsEditComponent();
		QMetaObject::invokeMethod(fieldsEditComponent,
			"setErrorText",
			Qt::QueuedConnection,
			Q_ARG(const QString&, "login already taken"));
	}
}