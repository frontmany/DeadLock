#include "authorizationComponent.h"
#include "loginWidget.h"
#include"mainWindow.h"


#include <QDebug>


AuthorizationComponent::AuthorizationComponent(QWidget* parent, LoginWidget* loginWidget)
    : QWidget(parent) {

    style = new StyleAuthorizationComponent;
    m_backgroundColor = QColor(30, 30, 30, 200);

   m_loginEdit = new QLineEdit(this);
   m_loginEdit->setPlaceholderText("Login"); 
   m_loginEdit->setMaximumSize(500, 40);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password"); 
    m_passwordEdit->setMaximumSize(500, 40);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_loginButton = new QPushButton("Login", this);
    m_loginButton->setMaximumSize(500, 40);
    connect(m_loginButton, &QPushButton::clicked, this, &AuthorizationComponent::SlotToSendLoginData);
    connect(this, &AuthorizationComponent::sendLoginData, loginWidget, &LoginWidget::onAuthorizeButtonClicked);

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_loginButtonHla = new QHBoxLayout;
    m_loginEditHla = new QHBoxLayout;
    m_passwordEditHla = new QHBoxLayout;


    m_loginEditHla->addSpacing(25);
    m_loginEditHla->addWidget(m_loginEdit);
    m_loginEditHla->addSpacing(25);

    m_passwordEditHla->addSpacing(25);
    m_passwordEditHla->addWidget(m_passwordEdit);
    m_passwordEditHla->addSpacing(25);

    m_loginButtonHla->addSpacing(25);
    m_loginButtonHla->addWidget(m_loginButton);
    m_loginButtonHla->addSpacing(25);

    
    layout->addLayout(m_loginEditHla);
    layout->addLayout(m_passwordEditHla);
    layout->addLayout(m_loginButtonHla);
    setLayout(layout);
    setFixedSize(450, 300); 
}


void AuthorizationComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);

    QWidget::paintEvent(event);
}


void AuthorizationComponent::setTheme(Theme theme) {
    if (theme == DARK) {
       m_loginEdit->setStyleSheet(style->DarkLineEditStyle);
        m_passwordEdit->setStyleSheet(style->DarkLineEditStyle);
        m_loginButton->setStyleSheet(style->DarkButtonStyle);
        m_backgroundColor = QColor(30, 30, 30, 200);
        update();
    }
    else {
       m_loginEdit->setStyleSheet(style->LightLineEditStyle);
        m_passwordEdit->setStyleSheet(style->LightLineEditStyle);
        m_loginButton->setStyleSheet(style->LightButtonStyle);
        m_backgroundColor = QColor(204, 204, 204, 200); 
        update();
    }
}

void AuthorizationComponent::SlotToSendLoginData() {
    QString login = m_loginEdit->text();
    QString password = m_passwordEdit->text();
    emit sendLoginData(login, password);
}