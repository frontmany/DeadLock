#include "authorizationComponent.h"


#include <QDebug>


AuthorizationComponent::AuthorizationComponent(QWidget* parent)
    : QWidget(parent) {

    style = new StyleAuthorizationComponent;
    m_backgroundColor = QColor(30, 30, 30, 200);

    m_usernameEditAuthorize = new QLineEdit(this);
    m_usernameEditAuthorize->setPlaceholderText("Login"); 
    m_usernameEditAuthorize->setMaximumSize(500, 40);

    m_passwordEditAuthorize = new QLineEdit(this);
    m_passwordEditAuthorize->setPlaceholderText("Password"); 
    m_passwordEditAuthorize->setMaximumSize(500, 40);
    m_passwordEditAuthorize->setEchoMode(QLineEdit::Password);

    m_loginButton = new QPushButton("Login", this);
    m_loginButton->setMaximumSize(500, 40);
    connect(m_loginButton, &QPushButton::clicked, this, &AuthorizationComponent::onAuthorizeButtonClicked);

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_loginButtonHla = new QHBoxLayout;
    m_usernameEditHla = new QHBoxLayout;
    m_passwordEditHla = new QHBoxLayout;


    m_usernameEditHla->addSpacing(25);
    m_usernameEditHla->addWidget(m_usernameEditAuthorize);
    m_usernameEditHla->addSpacing(25);

    m_passwordEditHla->addSpacing(25);
    m_passwordEditHla->addWidget(m_passwordEditAuthorize);
    m_passwordEditHla->addSpacing(25);

    m_loginButtonHla->addSpacing(25);
    m_loginButtonHla->addWidget(m_loginButton);
    m_loginButtonHla->addSpacing(25);

    
    layout->addLayout(m_usernameEditHla);
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


void AuthorizationComponent::onAuthorizeButtonClicked() {
    QString username = m_usernameEditAuthorize->text();
    QString password = m_passwordEditAuthorize->text();

    qDebug() << "Username:" << username << "Password:" << password;
}


void AuthorizationComponent::setTheme(Theme theme) {
    if (theme == DARK) {
        m_usernameEditAuthorize->setStyleSheet(style->DarkLineEditStyle);
        m_passwordEditAuthorize->setStyleSheet(style->DarkLineEditStyle);
        m_loginButton->setStyleSheet(style->DarkButtonStyle);
        m_backgroundColor = QColor(30, 30, 30, 200);
        update();
    }
    else {
        m_usernameEditAuthorize->setStyleSheet(style->LightLineEditStyle);
        m_passwordEditAuthorize->setStyleSheet(style->LightLineEditStyle);
        m_loginButton->setStyleSheet(style->LightButtonStyle);
        m_backgroundColor = QColor(204, 204, 204, 200); 
        update();
    }
}

