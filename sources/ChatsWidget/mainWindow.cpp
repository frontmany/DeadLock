#include "theme.h"
#include "chatsListComponent.h"
#include "registrationComponent.h"
#include "authorizationComponent.h"
#include "notificationWidget.h"
#include "configManager.h"
#include "overlayWidget.h"
#include "loginWidget.h"
#include "chatsWidget.h"
#include "greetWidget.h"
#include "mainWindow.h"
#include "workerQt.h"
#include "client.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_worker_Qt(nullptr), m_client(nullptr),
    m_greetWidget(nullptr), m_loginWidget(nullptr), m_chatsWidget(nullptr) , m_config_manager(nullptr)
{
    setAttribute(Qt::WA_DeleteOnClose);
    m_theme = utility::isDarkMode() ? DARK : LIGHT;

    setWindowTitle("Deadlock");
    setWindowIcon(QIcon(":/resources/GreetWidget/Deadlock.ico"));
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event->type() == QEvent::WindowStateChange) {
        QWindowStateChangeEvent* stateEvent = static_cast<QWindowStateChangeEvent*>(event);

        if (windowState() & Qt::WindowMinimized) {
            onWindowMinimized(); 
        }
        else {
            onWindowMaximized();
        }
    }

    QMainWindow::changeEvent(event);
}

void MainWindow::onWindowMinimized()
{
    if (m_chatsWidget) {
        if (!m_chatsWidget->getChatsList()->getIsHidden()) {
            m_client->broadcastMyStatus(utility::getCurrentFullDateAndTime());
        }
    }
}

void MainWindow::onWindowMaximized()
{
    if (m_chatsWidget) {
        if (!m_chatsWidget->getChatsList()->getIsHidden()) {
            m_client->broadcastMyStatus("online");
        }
    }
}

void MainWindow::setWorkerUIonClient() {
    m_worker_Qt = new WorkerQt(this);
    m_client->setWorkerUI(m_worker_Qt);
}

MainWindow::~MainWindow() 
{
}

void MainWindow::setupGreetWidget() {
    m_greetWidget = new GreetWidget(this, this, m_client, m_config_manager, m_theme, m_config_manager->getMyLogin(), m_client->getPublicKey(), m_client->getPrivateKey(), m_chatsWidget);
    m_greetWidget->setWelcomeLabelText(m_config_manager->getMyName());
    m_greetWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);
    m_greetWidget->startWelcomeAnimation();
    setCentralWidget(m_greetWidget);
}

void MainWindow::setupLoginWidget() {
    m_loginWidget = new LoginWidget(this, this, m_client, m_config_manager);
    m_loginWidget->setTheme(m_theme);
    setCentralWidget(m_loginWidget);
}

void MainWindow::setupChatsWidget() {
    m_chatsWidget = new ChatsWidget(this, this, m_client, m_config_manager, m_theme);
    m_chatsWidget->setTheme(m_theme);
    m_chatsWidget->restoreMessagingAreaComponents();
    m_chatsWidget->restoreChatComponents();

    setCentralWidget(m_chatsWidget);
    if (m_client->getIsFirstAuthentication()) {
        m_chatsWidget->getChatsList()->openHiddenButtonHintDialog();
    }

    m_client->setIsUIReadyToUpdate(true);
}

void MainWindow::updateRegistrationUIRedBorder() {
    if (m_loginWidget) {
        m_loginWidget->setButtonsDisabled(false);
        m_loginWidget->hideProgressLabel();
        RegistrationComponent* registrationComponent = m_loginWidget->getRegistrationComponent();
        registrationComponent->setErrorMessageToLabel("Registration Failed");
    }
}

void MainWindow::updateAuthorizationUIRedBorder() {
    if (m_loginWidget) {
        m_loginWidget->setButtonsDisabled(false);
        m_loginWidget->hideProgressLabel();
        auto authorizationComponent = m_loginWidget->getAuthorizationComponent();
        authorizationComponent->setErrorMessageToLabel("Authorization Failed");
    }
    else {
        showDoubleConnectionErrorDialog();
    }
}

ChatsWidget* MainWindow::getChatsWidget() {
    return m_chatsWidget;
}

void MainWindow::closeEvent(QCloseEvent* event) {
    qDebug() << "close triggered!";

    if (m_client->getIsPassedAuthentication()) {
        if (!m_chatsWidget->getChatsList()->getIsHidden()) {
            m_client->broadcastMyStatus(utility::getCurrentFullDateAndTime());
        }
    }
    else {
        QTimer::singleShot(300, []() { 
            QCoreApplication::quit(); 
        });
        return;
    }

    if (m_client->getIsAbleToClose()) {
        safeShutdown();
        event->accept();
        return;
    }

    event->ignore();

    auto window = windowHandle();
    if (window) {
        window->setFlag(Qt::WindowCloseButtonHint, false);
        window->show();
    }

    QTimer* closeCheckTimer = new QTimer(this);
    closeCheckTimer->setInterval(500);

    auto quitRequested = std::make_shared<bool>(false);

    auto cleanup = [this, closeCheckTimer, window, quitRequested]() {
        if (*quitRequested) return;
        *quitRequested = true;

        closeCheckTimer->stop();
        closeCheckTimer->deleteLater();

        if (window) {
            window->setFlag(Qt::WindowCloseButtonHint, true);
            window->show();
        }
    };

    auto tryClose = [this, cleanup, quitRequested]() {
        if (*quitRequested) return;
        if (m_client->getIsAbleToClose()) {
            safeShutdown();
            cleanup();
            QTimer::singleShot(300, []() { QCoreApplication::quit(); });
        }
    };

    connect(closeCheckTimer, &QTimer::timeout, this, tryClose);

    QTimer::singleShot(10'000, this, tryClose);
    closeCheckTimer->start();
}

void MainWindow::safeShutdown() {
    m_config_manager->save(
        m_client->getPublicKey(),
        m_client->getPrivateKey(),
        m_client->getSpecialServerKey(),
        m_client->getMyHashChatsMap(),
        m_client->getIsHidden(),
        m_client->getDatabase()
    );
}

void MainWindow::showAlreadyRunningDialog()
{
    OverlayWidget* overlay = new OverlayWidget(this);
    overlay->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    overlay->setAttribute(Qt::WA_TranslucentBackground);
    overlay->showMaximized();

    QDialog* dialog = new QDialog(overlay);
    dialog->setWindowTitle(tr("Application Already Running"));
    dialog->setMinimumWidth(400);
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    dialog->setAttribute(Qt::WA_TranslucentBackground);

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(20);
    shadowEffect->setXOffset(0);
    shadowEffect->setYOffset(0);
    shadowEffect->setColor(QColor(0, 0, 0, 120));

    QWidget* mainWidget = new QWidget(dialog);
    mainWidget->setGraphicsEffect(shadowEffect);
    mainWidget->setObjectName("mainWidget");

    QString mainWidgetStyle;
    if (m_theme == Theme::DARK) {
        mainWidgetStyle =
            "QWidget#mainWidget {"
            "   background-color: rgb(21, 21, 21);"
            "   border-radius: 12px;"
            "   border: 1px solid rgb(20, 20, 20);"
            "}";
    }
    else {
        mainWidgetStyle =
            "QWidget#mainWidget {"
            "   background-color: rgb(229, 228, 226);"
            "   border-radius: 12px;"
            "}";
    }
    mainWidget->setStyleSheet(mainWidgetStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    mainLayout->addWidget(mainWidget);

    QVBoxLayout* contentLayout = new QVBoxLayout(mainWidget);
    contentLayout->setContentsMargins(16, 16, 16, 16);
    contentLayout->setSpacing(20);

    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(QIcon(":/resources/ChatsWidget/warning.png").pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* messageLabel = new QLabel(tr("Application is already running!\nPlease close the existing instance."));
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);

    QString textStyle = (m_theme == Theme::DARK)
        ? "color: white; font-size: 14px;"
        : "color: black; font-size: 14px;";
    messageLabel->setStyleSheet(textStyle);

    QPushButton* closeButton = new QPushButton(tr("Close Application"));
    closeButton->setMinimumHeight(40);

    if (m_theme == Theme::DARK) {
        closeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: rgb(45, 45, 45);"
            "   color: white;"
            "   border-radius: 6px;"
            "   padding: 8px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgb(55, 55, 55);"
            "}"
        );
    }
    else {
        closeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: rgb(220, 220, 220);"
            "   color: black;"
            "   border-radius: 6px;"
            "   padding: 8px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgb(200, 200, 200);"
            "}"
        );
    }

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(messageLabel);
    contentLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, []() {
        QApplication::quit();
    });

    QObject::connect(dialog, &QDialog::finished, overlay, &QWidget::deleteLater);

    dialog->exec();
}

void MainWindow::showConnectionErrorDialog() {
    m_client->stopClient();

    OverlayWidget* overlay = new OverlayWidget(this);
    overlay->show();

    QDialog* errorDialog = new QDialog(overlay);
    errorDialog->setWindowTitle(tr("Connection Error"));
    errorDialog->setFixedSize(500, 350);
    errorDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    errorDialog->setAttribute(Qt::WA_TranslucentBackground);

    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    errorDialog->move(
        screenGeometry.center() - errorDialog->rect().center()
    );

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(20);
    shadowEffect->setXOffset(0);
    shadowEffect->setYOffset(0);
    shadowEffect->setColor(QColor(0, 0, 0, 120));

    QWidget* mainWidget = new QWidget(errorDialog);
    mainWidget->setGraphicsEffect(shadowEffect);
    mainWidget->setObjectName("mainWidget");

    QString mainWidgetStyle;
    if (m_theme == Theme::DARK) {
        mainWidgetStyle =
            "QWidget#mainWidget {"
            "   background-color: rgb(21, 21, 21);"
            "   border-radius: 12px;"
            "   border: 1px solid rgb(20, 20, 20);"
            "}";
    }
    else {
        mainWidgetStyle =
            "QWidget#mainWidget {"
            "   background-color: rgb(229, 228, 226);"
            "   border-radius: 12px;"
            "}";
    }
    mainWidget->setStyleSheet(mainWidgetStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(errorDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(mainWidget);

    QVBoxLayout* contentLayout = new QVBoxLayout(mainWidget);
    contentLayout->setContentsMargins(16, 16, 16, 16);
    contentLayout->setSpacing(20);

    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(QIcon(":/resources/ChatsWidget/networkError.png").pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* errorLabel = new QLabel(tr("Network connection problem\nWe can't establish connection right now"));
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setWordWrap(true);

    QString textStyle = (m_theme == Theme::DARK)
        ? "color: white; font-size: 14px;"
        : "color: black; font-size: 14px;";
    errorLabel->setStyleSheet(textStyle);

    QPushButton* closeButton = new QPushButton(tr("Close Application"));
    closeButton->setMinimumHeight(40);

    if (m_theme == Theme::DARK) {
        closeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: rgb(45, 45, 45);"
            "   color: white;"
            "   border-radius: 6px;"
            "   padding: 8px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgb(55, 55, 55);"
            "}"
        );
    }
    else {
        closeButton->setStyleSheet(
            "QPushButton {"
            "   background-color: rgb(220, 220, 220);"
            "   color: black;"
            "   border-radius: 6px;"
            "   padding: 8px;"
            "}"
            "QPushButton:hover {"
            "   background-color: rgb(200, 200, 200);"
            "}"
        );
    }

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(errorLabel);
    contentLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, []() {
        QApplication::quit();
    });

    QObject::connect(errorDialog, &QDialog::finished, overlay, &QWidget::deleteLater);

    errorDialog->exec();
}

void MainWindow::showDoubleConnectionErrorDialog() {
    OverlayWidget* overlay = new OverlayWidget(this);
    overlay->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    overlay->setAttribute(Qt::WA_TranslucentBackground);
    overlay->showMaximized();

    QDialog* errorDialog = new QDialog(overlay);
    errorDialog->setWindowTitle(tr("Double Connection Error"));
    errorDialog->setFixedSize(500, 350); 
    errorDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    errorDialog->setAttribute(Qt::WA_TranslucentBackground);

    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    errorDialog->move(
        screenGeometry.center() - errorDialog->rect().center()
    );

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(15);
    shadowEffect->setXOffset(0);
    shadowEffect->setYOffset(0);
    shadowEffect->setColor(QColor(0, 0, 0, 80));

    QWidget* mainWidget = new QWidget(errorDialog);
    mainWidget->setGraphicsEffect(shadowEffect);
    mainWidget->setObjectName("mainWidget");

    QString mainWidgetStyle = m_theme == Theme::DARK ?
        "QWidget#mainWidget {"
        "   background-color: rgb(21, 21, 21);"
        "   border-radius: 12px;"
        "   border: 1px solid rgb(40, 40, 40);"
        "}" :
        "QWidget#mainWidget {"
        "   background-color: rgb(229, 228, 226);"
        "   border-radius: 12px;"
        "   border: 1px solid rgb(200, 200, 200);"
        "}";
    mainWidget->setStyleSheet(mainWidgetStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(errorDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20); 
    mainLayout->addWidget(mainWidget);

    QVBoxLayout* contentLayout = new QVBoxLayout(mainWidget);
    contentLayout->setContentsMargins(16, 16, 16, 16);
    contentLayout->setSpacing(20);
    contentLayout->setAlignment(Qt::AlignCenter);

    QLabel* iconLabel = new QLabel();
    iconLabel->setPixmap(QIcon(":/resources/ChatsWidget/warning.png").pixmap(64, 64));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* errorLabel = new QLabel(tr("Double connection detected!\nOnly one active connection is allowed."));
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setWordWrap(true);
    errorLabel->setStyleSheet(
        m_theme == Theme::DARK ?
        "color: white; font-size: 14px;" :
        "color: black; font-size: 14px;"
    );

    QPushButton* closeButton = new QPushButton(tr("Close Application"));
    closeButton->setFixedHeight(40);
    closeButton->setStyleSheet(
        m_theme == Theme::DARK ?
        "QPushButton {"
        "   background-color: rgb(45, 45, 45);"
        "   color: white;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(55, 55, 55);"
        "}" :
    "QPushButton {"
        "   background-color: rgb(220, 220, 220);"
        "   color: black;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(200, 200, 200);"
        "}"
        );

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(errorLabel);
    contentLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, this, []() {
        QApplication::quit();
    });

    QObject::connect(errorDialog, &QDialog::finished, overlay, &QWidget::deleteLater);

    errorDialog->exec();
}

void MainWindow::showConfigLoadErrorDialog() {
    OverlayWidget* overlay = new OverlayWidget(this);
    overlay->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    overlay->setAttribute(Qt::WA_TranslucentBackground);
    overlay->showMaximized();

    QDialog* errorDialog = new QDialog(overlay);
    errorDialog->setWindowTitle(tr("Configuration Error"));
    errorDialog->setFixedSize(380, 450);
    errorDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    errorDialog->setAttribute(Qt::WA_TranslucentBackground);

    QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
    errorDialog->move(
        screenGeometry.center() - errorDialog->rect().center()
    );

    QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect();
    shadowEffect->setBlurRadius(15);
    shadowEffect->setXOffset(0);
    shadowEffect->setYOffset(0);
    shadowEffect->setColor(QColor(0, 0, 0, 80));

    QWidget* mainWidget = new QWidget(errorDialog);
    mainWidget->setGraphicsEffect(shadowEffect);
    mainWidget->setObjectName("mainWidget");

    QString mainWidgetStyle = m_theme == Theme::DARK ?
        "QWidget#mainWidget {"
        "   background-color: rgb(21, 21, 21);"
        "   border-radius: 12px;"
        "   border: 1px solid rgb(40, 40, 40);"
        "}" :
        "QWidget#mainWidget {"
        "   background-color: rgb(229, 228, 226);"
        "   border-radius: 12px;"
        "   border: 1px solid rgb(200, 200, 200);"
        "}";
    mainWidget->setStyleSheet(mainWidgetStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(errorDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->addWidget(mainWidget);

    QVBoxLayout* contentLayout = new QVBoxLayout(mainWidget);
    contentLayout->setContentsMargins(16, 16, 16, 16);
    contentLayout->setSpacing(20);
    contentLayout->setAlignment(Qt::AlignCenter);

    QLabel* iconLabel = new QLabel();
    QPixmap errorPixmap(":/resources/ChatsWidget/configLoadingError.png");


    if (errorPixmap.isNull()) {
        errorPixmap = QIcon(":/resources/ChatsWidget/error.png").pixmap(64, 64);
        qWarning() << "Failed to load configLoadingError.png, using fallback icon";
    }
    iconLabel->setPixmap(errorPixmap.scaled(128, 128, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    iconLabel->setAlignment(Qt::AlignCenter);

    QLabel* errorLabel = new QLabel(
        "The configuration file could not be loaded.\n"
            "Current chat history is unavailable.\n\n"
            "Possible causes:\n"
            "- The configuration file is corrupted\n"
            "- File not found\n"
            "\n"
            "Application will start with default settings."
    );

    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setWordWrap(true);
    errorLabel->setStyleSheet(
        m_theme == Theme::DARK ?
        "color: white; font-size: 14px;" :
        "color: black; font-size: 14px;"
    );

    QPushButton* closeButton = new QPushButton(tr("Close"));
    closeButton->setFixedHeight(40);
    closeButton->setStyleSheet(
        m_theme == Theme::DARK ?
        "QPushButton {"
        "   background-color: rgb(45, 45, 45);"
        "   color: white;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(55, 55, 55);"
        "}" :
    "QPushButton {"
        "   background-color: rgb(220, 220, 220);"
        "   color: black;"
        "   border-radius: 6px;"
        "   padding: 8px;"
        "}"
        "QPushButton:hover {"
        "   background-color: rgb(200, 200, 200);"
        "}"
        );

    contentLayout->addWidget(iconLabel);
    contentLayout->addWidget(errorLabel);
    contentLayout->addWidget(closeButton);

    connect(closeButton, &QPushButton::clicked, errorDialog, &QDialog::reject);
    QObject::connect(errorDialog, &QDialog::finished, overlay, &QWidget::deleteLater);

    errorDialog->exec();
}