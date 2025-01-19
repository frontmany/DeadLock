#include "loginWidget.h"
#include "authorizationComponent.h"
#include "registrationComponent.h"

LoginWidget::LoginWidget(QWidget* parent)
    : QWidget(parent){


    style = new StyleLoginWidget;

    m_switchToRegisterButton = new QPushButton("register", this);
    m_switchToAuthorizeButton = new QPushButton("login", this);

    m_switchersHLayout = new QHBoxLayout;
    m_switchersHLayout->addSpacing(-300);
    m_switchersHLayout->setAlignment(Qt::AlignCenter);
    m_switchersHLayout->addWidget(m_switchToAuthorizeButton);
    m_switchersHLayout->addWidget(m_switchToRegisterButton);



    m_FormsHLayout = new QHBoxLayout;
    m_FormsHLayout->setAlignment(Qt::AlignCenter);

    m_authorizationWidget = new AuthorizationComponent(this);
    m_registrationWidget = new RegistrationComponent(this);
    m_FormsHLayout->addWidget(m_authorizationWidget);
    m_FormsHLayout->addWidget(m_registrationWidget);


    
    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignCenter);
    m_mainVLayout->addLayout(m_switchersHLayout);
    m_registrationWidget->hide();
    m_mainVLayout->addLayout(m_FormsHLayout);

    
    connect(m_switchToRegisterButton, &QPushButton::clicked, this, &LoginWidget::switchToRegister);
    connect(m_switchToAuthorizeButton, &QPushButton::clicked, this, &LoginWidget::switchToAuthorize);
}

void LoginWidget::onAuthorizeButtonClicked() {
    // Логика авторизации

}

void LoginWidget::onRegisterButtonClicked() {
    // Логика регистрации 
}

void LoginWidget::switchToAuthorize() {
    m_registrationWidget->hide();
    m_authorizationWidget->show();
}

void LoginWidget::switchToRegister() {
    m_authorizationWidget->hide();
    m_registrationWidget->show();
}

void LoginWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.drawPixmap(this->rect(), m_background); 
    QWidget::paintEvent(event);
}

void LoginWidget::setTheme(Theme theme) {
    m_switchToAuthorizeButton->setStyleSheet(style->buttonStyleBlue);
    m_switchToRegisterButton->setStyleSheet(style->buttonStyleGray);
    m_authorizationWidget->setTheme(theme);
    m_registrationWidget->setTheme(theme);
    setBackGround(theme);
    update();
}

void LoginWidget::setBackGround(Theme theme) {
    if (theme == DARK) {
        if (m_background.load(":/resources/LoginWidget/darkLoginBackground.jpg")) {
        }
    }
    else {
        if (m_background.load(":/resources/LoginWidget/lightLoginBackground.jpg")) { 
        }
    }
}