#include <filesystem>

#include "chatsListComponent.h"
#include "registrationComponent.h"
#include "authorizationComponent.h"
#include "loginWidget.h"
#include "chatsWidget.h"
#include "greetWidget.h"
#include "mainWindow.h"
#include "mainWindow.h"
#include "workerQt.h"
#include "utility.h"
#include "client.h"




MainWindow::MainWindow(QWidget* parent, Client* client)
    : QMainWindow(parent), m_worker_Qt(nullptr), m_client(client),
    m_greetWidget(nullptr), m_loginWidget(nullptr), m_chatsWidget(nullptr) 
{
    m_theme = utility::isDarkMode() ? DARK : LIGHT;

    setWindowTitle("Deadlock");
    setWindowIcon(QIcon(":/resources/GreetWidget/Ilock.ico"));

    m_worker_Qt = new WorkerQt(this);
    m_client->setWorkerUI(m_worker_Qt);
}

MainWindow::~MainWindow() {
    qDebug() << "window closing";
    m_client->broadcastMyStatus(utility::getCurrentDateTime());
    std::cout << "saving\n";

    if (m_client->isNeedToSaveConfig()) {
        m_client->save();
    }

    delete m_chatsWidget;
    delete m_worker_Qt;
}

void MainWindow::setupGreetWidget() {
    m_greetWidget = new GreetWidget(this, this, m_client, m_theme, "", m_chatsWidget);
    m_greetWidget->setName(m_client->getMyName());
    m_greetWidget->setLogin(m_client->getMyLogin());
    m_greetWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_greetWidget->startWelcomeAnimation();
    setCentralWidget(m_greetWidget);
}

void MainWindow::setupLoginWidget() {
    m_loginWidget = new LoginWidget(this, this, m_client);
    m_loginWidget->setTheme(m_theme);
    setCentralWidget(m_loginWidget);
}

void MainWindow::setupChatsWidget() {
    m_chatsWidget = new ChatsWidget(this, this, m_client, m_theme);
    m_chatsWidget->setTheme(m_theme);
    m_chatsWidget->restoreMessagingAreaComponents();
    m_chatsWidget->restoreChatComponents();

    m_client->setIsUIReadyToUpdate(true);

    setCentralWidget(m_chatsWidget);
}

LoginWidget* MainWindow::getLoginWidget() {
    return m_loginWidget;
}

void MainWindow::updateRegistrationUIRedBorder() {
    RegistrationComponent* registrationComponent = m_loginWidget->getRegistrationComponent();
    registrationComponent->setRedBorderToLoginEdit();
    registrationComponent->setRedBorderToNameEdit();
    registrationComponent->setRedBorderToPasswordEdits();
}

void MainWindow::updateAuthorizationUIRedBorder() {
    if (auto authorizationComponent = m_loginWidget->getAuthorizationComponent()) {
        authorizationComponent->setRedBorderToLoginEdit();
        authorizationComponent->setRedBorderToPasswordEdit();
    }
    
}

ChatsWidget* MainWindow::getChatsWidget() {
    return m_chatsWidget;
}