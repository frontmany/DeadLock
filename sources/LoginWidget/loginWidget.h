#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>


struct StyleLoginWidget {
    StyleLoginWidget();

    QString buttonStyleBlue;
    QString buttonStyleGray;
};


class AuthorizationComponent;
class RegistrationComponent;
class Client;
class MainWindow;
enum Theme;


class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget* parent, MainWindow* mw, Client* client);
    void setTheme(Theme& theme);

    AuthorizationComponent* getAuthorizationComponent();
    RegistrationComponent* getRegistrationComponent();

private slots:
    void switchToAuthorize();
    void switchToRegister();

public slots:
    void onAuthorizeButtonClicked(QString& login, QString& password);
    void onRegisterButtonClicked(QString& login, QString& password, QString& name);


private:
    void paintEvent(QPaintEvent* event) override;
    void setBackGround(Theme theme);
    void swapSwitchStyles();
    enum SwithcState{AUTHORIZATION, REGISTRATION};

private:
    StyleLoginWidget*       style;
    QPixmap                 m_background;
    SwithcState             m_switchState;
    Client*                 m_client;

    QVBoxLayout*            m_mainVLayout;
    QHBoxLayout*            m_switchersHLayout;
    QHBoxLayout*            m_FormsHLayout;

    AuthorizationComponent* m_authorizationWidget;
    RegistrationComponent*  m_registrationWidget;

    QPushButton*            m_switchToAuthorizeButton;
    QPushButton*            m_switchToRegisterButton;
};
