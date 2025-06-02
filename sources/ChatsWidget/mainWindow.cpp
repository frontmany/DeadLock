#include "theme.h"
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

    if (m_client->isAutoLogin()) {
        m_client->save();
    }

    delete m_chatsWidget;
    delete m_worker_Qt;
}

void MainWindow::showConnectionErrorDialog() {
    class OverlayWidget : public QWidget {
    public:
        using QWidget::QWidget;
    protected:
        void paintEvent(QPaintEvent*) override {
            setGeometry(QApplication::primaryScreen()->geometry());
            QPainter painter(this);
            painter.fillRect(rect(), QColor(25, 25, 25, 160));
        }
    };

    OverlayWidget* overlay = new OverlayWidget(nullptr);
    overlay->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    overlay->setAttribute(Qt::WA_TranslucentBackground);
    overlay->showMaximized();

    QDialog* errorDialog = new QDialog(overlay);
    errorDialog->setWindowTitle(tr("Connection Error"));
    errorDialog->setMinimumWidth(400);
    errorDialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    errorDialog->setAttribute(Qt::WA_TranslucentBackground);

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
            "   background-color: rgb(240, 240, 240);"
            "   border-radius: 12px;"
            "   border: 1px solid rgb(230, 230, 230);"
            "}";
    }
    mainWidget->setStyleSheet(mainWidgetStyle);

    QVBoxLayout* mainLayout = new QVBoxLayout(errorDialog);
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
void MainWindow::setupGreetWidget() {
    m_greetWidget = new GreetWidget(this, this, m_client, m_theme, "", m_chatsWidget);
    m_greetWidget->setWelcomeLabelText(m_client->getMyName());
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
    registrationComponent->setErrorMessageToLabel("Registration Failed");
    
}

void MainWindow::updateAuthorizationUIRedBorder() {
    auto authorizationComponent = m_loginWidget->getAuthorizationComponent();
    authorizationComponent->setErrorMessageToLabel("Authorization Failed");
}

ChatsWidget* MainWindow::getChatsWidget() {
    return m_chatsWidget;
}