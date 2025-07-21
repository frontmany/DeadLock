#include "workerQt.h"
#include "chatsWidget.h"
#include "messagingAreaComponent.h"
#include "fileWrapper.h"
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
#include "client.h"
#include "friendInfo.h"
#include "friendSearchDialogComponent.h"
#include "photo.h"

void WorkerQt::onMessageSendingError(const std::string& friendLogin, Message* message) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& compsVec = chatsWidget->getMessagingAreasVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
		});
	MessagingAreaComponent* areaComp = *comp;
	auto& messagesCompsVec = areaComp->getMessagesComponentsVec();

	auto messageCompIt = std::find_if(messagesCompsVec.begin(), messagesCompsVec.end(), [message](MessageComponent* comp) {
		return comp->getId() == QString::fromStdString(message->getId());
	});
	MessageComponent* messageComp = *messageCompIt;
	QMetaObject::invokeMethod(messageComp,
		"setRetry",
		Qt::QueuedConnection);

}

void WorkerQt::onRequestedFileError(const std::string& friendLoginHash, fileWrapper fileWrapper) {
	updateFileLoadingState(friendLoginHash, fileWrapper, true);
}

void WorkerQt::onConnectError() {
	QMetaObject::invokeMethod(m_main_window,
		"showConnectionErrorDialog",
		Qt::QueuedConnection);
}

void WorkerQt::onNetworkError() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsList = chatsWidget->getChatsList();
	QMetaObject::invokeMethod(chatsList,
		"showNoConnectionLabel",
		Qt::QueuedConnection);
}

void WorkerQt::onServerDown() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsList = chatsWidget->getChatsList();
	QMetaObject::invokeMethod(chatsList,
		"showServerOfflineLabel",
		Qt::QueuedConnection);
}

void WorkerQt::setRecoveredAvatar(Photo* myRecoveredAvatar) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsList = chatsWidget->getChatsList();
	QMetaObject::invokeMethod(chatsList,
		"setAvatar",
		Qt::QueuedConnection,
		Q_ARG(const Photo&, *myRecoveredAvatar));

	QMetaObject::invokeMethod(chatsList,
		"setAvatarInProfileEditorWidget",
		Qt::QueuedConnection,
		Q_ARG(const Photo&, *myRecoveredAvatar));
}

void WorkerQt::setNameFieldInProfileEditorWidget(const std::string& name) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsList = chatsWidget->getChatsList();
	QMetaObject::invokeMethod(chatsList,
		"setNameFieldInProfileEditorWidget",
		Qt::QueuedConnection,
		Q_ARG(const std::string&, name));
}

void WorkerQt::updateFileLoadingState(const std::string& friendLoginHash, fileWrapper& file, bool isError) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& compsVec = chatsWidget->getMessagingAreasVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLoginHash](MessagingAreaComponent* comp) {
		return utility::calculateHash(comp->getChat()->getFriendLogin()) == friendLoginHash;
	});
	MessagingAreaComponent* areaComp = *comp;
	auto& messagesCompsVec = areaComp->getMessagesComponentsVec();

	std::string blobUID = file.file.blobUID;
	auto messageCompIt = std::find_if(messagesCompsVec.begin(), messagesCompsVec.end(), [&blobUID](MessageComponent* comp) {
		return comp->getId() == QString::fromStdString(blobUID);
	});

	MessageComponent* messageComp = *messageCompIt; 
	if (!isError) {
		QMetaObject::invokeMethod(messageComp,
			"requestedFileLoaded",
			Qt::QueuedConnection,
			Q_ARG(const fileWrapper&, file));
	}
	else {
		QMetaObject::invokeMethod(messageComp,
			"requestedFileUnLoadedError",
			Qt::QueuedConnection,
			Q_ARG(const fileWrapper&, file));
		
	}
}

void WorkerQt::updateFileLoadingProgress(const std::string& friendLoginHash, const net::file<QueryType>& file, uint32_t progressPercent) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& compsVec = chatsWidget->getMessagingAreasVec();
	try {
		auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLoginHash](MessagingAreaComponent* comp) {
			return utility::calculateHash(comp->getChat()->getFriendLogin()) == friendLoginHash;
		});


		if (comp != compsVec.end()) {
			MessagingAreaComponent* areaComp = *comp;
			auto& messagesCompsVec = areaComp->getMessagesComponentsVec();
			std::string blobUID = file.blobUID;
			auto messageCompIt = std::find_if(messagesCompsVec.begin(), messagesCompsVec.end(), [&blobUID](MessageComponent* comp) {
				return comp->getId() == QString::fromStdString(blobUID);
				});

			if (messageCompIt != messagesCompsVec.end()) {
				MessageComponent* messageComp = *messageCompIt;

				QMetaObject::invokeMethod(messageComp,
					"setProgress",
					Qt::QueuedConnection,
					Q_ARG(const net::file<QueryType>&, file),
					Q_ARG(int, progressPercent));
			}
		}
	}
	catch (...){
		std::cout << "update progress error";
	}
}

void WorkerQt::showNowReceiving(const std::string& friendLoginHash) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& compsVec = chatsWidget->getMessagingAreasVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLoginHash](MessagingAreaComponent* comp) {
		return utility::calculateHash(comp->getChat()->getFriendLogin()) == friendLoginHash;
	});

	if (comp != compsVec.end()) {
		MessagingAreaComponent* areaComp = *comp;
		QMetaObject::invokeMethod(areaComp,
			"addDelimiterComponentIncomingFilesLoading",
			Qt::QueuedConnection);


		auto& vec = chatsWidget->getChatsList()->getChatComponentsVec();
		auto it = std::find_if(vec.begin(), vec.end(), [friendLoginHash](ChatComponent* chatComp) {
			return utility::calculateHash(chatComp->getChat()->getFriendLogin()) == friendLoginHash;
			});

		if (it == vec.end()) {
			return;
		}

		ChatComponent* chatComp = *it;
		QMetaObject::invokeMethod(chatComp,
			"setLastMessageAsIncomingFilesIndicator",
			Qt::QueuedConnection);
	}

}

void WorkerQt::updateFileSendingProgress(const std::string& friendLoginHash, const net::file<QueryType>& file, uint32_t progressPercent) {
	updateFileLoadingProgress(friendLoginHash, file, progressPercent);
}

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

void WorkerQt::supplyTheme(bool isDarkTheme) {
	QMetaObject::invokeMethod(m_main_window,
		"setTheme",
		Qt::QueuedConnection,
		Q_ARG(bool, isDarkTheme));
}

void WorkerQt::showUpdateButton() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsListComponent = chatsWidget->getChatsList();

	QMetaObject::invokeMethod(chatsListComponent,
		"showUpdateButton",
		Qt::QueuedConnection);
}

void WorkerQt::updateAndRestart() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	QMetaObject::invokeMethod(chatsWidget,
		"updateAndRestart",
		Qt::QueuedConnection);
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

void WorkerQt::onMessageReceive(const std::string& friendLoginHash, Message* message) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsList = chatsWidget->getChatsList();
	auto& vec = chatsList->getChatComponentsVec();
	auto it = std::find_if(vec.begin(), vec.end(), [friendLoginHash](ChatComponent* chatComp) {
		return utility::calculateHash(chatComp->getChat()->getFriendLogin()) == friendLoginHash;
	});

	if (it == vec.end()) {
		return;
	}



	ChatComponent* chatComp = *it;

	QMetaObject::invokeMethod(chatsWidget,
		"showNotification",
		Qt::QueuedConnection,
		Q_ARG(Chat*, chatComp->getChat()));


	if (chatComp->getChat()->getLayoutIndex() != 0){
		QMetaObject::invokeMethod(chatsList,
			"popUpComponent",
			Qt::QueuedConnection,
			Q_ARG(ChatComponent*, chatComp));
	}

	QString msg = "";
	if (message->getRelatedFiles().size() > 0) {
		msg = "file";
	}
	else {
		msg = QString::fromStdString(message->getMessage());
	}

	QMetaObject::invokeMethod(chatComp,
		"setLastMessage",
		Qt::QueuedConnection,
		Q_ARG(const QString&, msg));

	if (chatsWidget->getCurrentMessagingAreaComponent() == nullptr) {
		QMetaObject::invokeMethod(chatComp,
			"setUnreadMessageDot",
			Qt::QueuedConnection,
			Q_ARG(bool, true));
	}
	else if (utility::calculateHash(chatsWidget->getCurrentMessagingAreaComponent()->getChat()->getFriendLogin()) != friendLoginHash) {
		QMetaObject::invokeMethod(chatComp,
			"setUnreadMessageDot",
			Qt::QueuedConnection,
			Q_ARG(bool, true));
	}


	auto& compsVec = chatsWidget->getMessagingAreasVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLoginHash](MessagingAreaComponent* comp) {
		return utility::calculateHash(comp->getChat()->getFriendLogin()) == friendLoginHash;
	});
	MessagingAreaComponent* areaComp = *comp;

	QMetaObject::invokeMethod(areaComp,
		"addMessage",
		Qt::QueuedConnection,
		Q_ARG(Message*, message),
		Q_ARG(bool, false));

	if (chatsWidget->getCurrentMessagingAreaComponent() == nullptr) {}
	else if (utility::calculateHash(chatsWidget->getCurrentMessagingAreaComponent()->getChat()->getFriendLogin()) == friendLoginHash) {
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

void WorkerQt::showTypingLabel(const std::string& friendLogin) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& compsVec = chatsWidget->getMessagingAreasVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
	});

	if (comp != compsVec.end()) {
		MessagingAreaComponent* areaComp = *comp;
		auto headerComp = areaComp->getChatHeader();

		QMetaObject::invokeMethod(headerComp,
			"swapLastSeenLabel",
			Qt::QueuedConnection,
			Q_ARG(bool, true));
	}
}

void WorkerQt::hideTypingLabel(const std::string& friendLogin) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& compsVec = chatsWidget->getMessagingAreasVec();
	auto comp = std::find_if(compsVec.begin(), compsVec.end(), [&friendLogin](MessagingAreaComponent* comp) {
		return comp->getChat()->getFriendLogin() == friendLogin;
	});
	if (comp != compsVec.end()) {
		MessagingAreaComponent* areaComp = *comp;
		auto headerComp = areaComp->getChatHeader();

		QMetaObject::invokeMethod(headerComp,
			"swapLastSeenLabel",
			Qt::QueuedConnection,
			Q_ARG(bool, false));
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

void WorkerQt::onStatusReceive(const std::string& friendLoginHash, const std::string& status) {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	auto& messagingAreaCompsVec = chatsWidget->getMessagingAreasVec();

	auto messagingAreaIt = std::find_if(messagingAreaCompsVec.begin(), messagingAreaCompsVec.end(), [&friendLoginHash](MessagingAreaComponent* comp) {
		return utility::calculateHash(comp->getChat()->getFriendLogin()) == friendLoginHash;
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


		auto photo = chat->getFriendPhoto();
		if (photo != nullptr) {
			try {
				std::string path = photo->getPhotoPath();
				if (path.empty()) {
					throw std::runtime_error("Empty photo path");
				}

				std::ifstream file(path, std::ios::binary);
				if (!file) {
					throw std::runtime_error("Failed to open photo file");
				}

				file.seekg(0, std::ios::end);
				size_t fileSize = file.tellg();
				file.seekg(0, std::ios::beg);

				std::string fileData(fileSize, '\0');
				file.read(&fileData[0], fileSize);
				file.close();

				size_t delimiterPos = fileData.find('\n');
				if (delimiterPos == std::string::npos) {
					throw std::runtime_error("Invalid photo file format");
				}

				std::string encryptedKey = fileData.substr(0, delimiterPos);
				std::string encryptedData = fileData.substr(delimiterPos + 1);

				auto aesKey = utility::RSADecryptKey(chatsWidget->getClient()->getPrivateKey(), encryptedKey);

				std::string decryptedData = utility::AESDecrypt(aesKey, encryptedData);

				QByteArray imageData(decryptedData.data(), decryptedData.size());
				QPixmap avatarPixmap;
				if (!avatarPixmap.loadFromData(imageData)) {
					throw std::runtime_error("Failed to load decrypted avatar");
				}


				QMetaObject::invokeMethod(chatComp,
					"setAvatar",
					Qt::QueuedConnection,
					Q_ARG(QPixmap, avatarPixmap));

			}
			catch (const std::exception& e) {
				qWarning() << "Avatar load error:" << e.what();
			}
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

		
		auto photo = chat->getFriendPhoto();
		if (photo != nullptr) {
			try {
				std::string path = photo->getPhotoPath();
				if (path.empty()) {
					throw std::runtime_error("Empty photo path");
				}

				std::ifstream file(path, std::ios::binary);
				if (!file) {
					throw std::runtime_error("Failed to open photo file");
				}

				file.seekg(0, std::ios::end);
				size_t fileSize = file.tellg();
				file.seekg(0, std::ios::beg);

				std::string fileData(fileSize, '\0');
				file.read(&fileData[0], fileSize);
				file.close();

				size_t delimiterPos = fileData.find('\n');
				if (delimiterPos == std::string::npos) {
					throw std::runtime_error("Invalid photo file format");
				}

				std::string encryptedKey = fileData.substr(0, delimiterPos);
				std::string encryptedData = fileData.substr(delimiterPos + 1);

				auto aesKey = utility::RSADecryptKey(chatsWidget->getClient()->getPrivateKey(), encryptedKey);

				std::string decryptedData = utility::AESDecrypt(aesKey, encryptedData);

				QByteArray imageData(decryptedData.data(), decryptedData.size());
				QPixmap avatarPixmap;
				if (!avatarPixmap.loadFromData(imageData)) {
					throw std::runtime_error("Failed to load decrypted avatar");
				}


				QMetaObject::invokeMethod(messagingArea,
					"setAvatar",
					Qt::QueuedConnection,
					Q_ARG(QPixmap, avatarPixmap));

			}
			catch (const std::exception& e) {
				qWarning() << "Avatar load error:" << e.what();
			}
		}

		return true;
	}

	return false;
}

void WorkerQt::blockProfileEditing() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsListComponent = chatsWidget->getChatsList();
	QMetaObject::invokeMethod(chatsListComponent,
		"disableProfileButton",
		Qt::QueuedConnection);
}

void WorkerQt::activateProfileEditing() {
	ChatsWidget* chatsWidget = m_main_window->getChatsWidget();
	ChatsListComponent* chatsListComponent = chatsWidget->getChatsList();
	QMetaObject::invokeMethod(chatsListComponent,
		"activateProfileButton",
		Qt::QueuedConnection);
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