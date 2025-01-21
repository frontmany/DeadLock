#include "registrationComponent.h"
#include "mainWindow.h"


#include <QDebug>


RegistrationComponent::RegistrationComponent(QWidget* parent, LoginWidget* loginWidget)
    : QWidget(parent) {

    style = new StyleRegistrationComponent;
    m_backgroundColor = QColor(30, 30, 30, 200);

    m_loginEdit = new QLineEdit(this);
    m_loginEdit->setPlaceholderText("Login");
    m_loginEdit->setMaximumSize(500, 40);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setMaximumSize(500, 40);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    m_password2Edit = new QLineEdit(this);
    m_password2Edit->setPlaceholderText("Confirm password");
    m_password2Edit->setMaximumSize(500, 40);
    m_password2Edit->setEchoMode(QLineEdit::Password);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Name");
    m_nameEdit->setMaximumSize(500, 40);


    m_registerButton = new QPushButton("Register", this);
    m_registerButton->setMaximumSize(500, 40);
    connect(m_registerButton, &QPushButton::clicked, this, &RegistrationComponent::slotToSendRegistrationData);
    connect(this, &RegistrationComponent::sendRegistrationData, loginWidget, &LoginWidget::onRegisterButtonClicked);

    QVBoxLayout* layout = new QVBoxLayout(this);
    m_registerButtonHla = new QHBoxLayout;
    m_loginEditHla = new QHBoxLayout;
    m_nameEditHla = new QHBoxLayout;
    m_passwordEditHla = new QHBoxLayout;
    m_password2EditHla = new QHBoxLayout;


    m_loginEditHla->addSpacing(25);
    m_loginEditHla->addWidget(m_loginEdit);
    m_loginEditHla->addSpacing(25);

    m_nameEditHla->addSpacing(25);
    m_nameEditHla->addWidget(m_nameEdit);
    m_nameEditHla->addSpacing(25);

    m_passwordEditHla->addSpacing(25);
    m_passwordEditHla->addWidget(m_passwordEdit);
    m_passwordEditHla->addSpacing(25);

    m_password2EditHla->addSpacing(25);
    m_password2EditHla->addWidget(m_password2Edit);
    m_password2EditHla->addSpacing(25);

    m_registerButtonHla->addSpacing(25);
    m_registerButtonHla->addWidget(m_registerButton);
    m_registerButtonHla->addSpacing(25);


    layout->addLayout(m_loginEditHla);
    layout->addLayout(m_nameEditHla);
    layout->addLayout(m_passwordEditHla);
    layout->addLayout(m_password2EditHla);
    layout->addLayout(m_registerButtonHla);
    setLayout(layout);
    setFixedSize(550, 450);
}


void RegistrationComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);

    QWidget::paintEvent(event);
}



void RegistrationComponent::setTheme(Theme theme) {
    if (theme == DARK) {
        m_loginEdit->setStyleSheet(style->DarkLineEditStyle);
        m_nameEdit->setStyleSheet(style->DarkLineEditStyle);
        m_passwordEdit->setStyleSheet(style->DarkLineEditStyle);
        m_password2Edit->setStyleSheet(style->DarkLineEditStyle);
        m_registerButton->setStyleSheet(style->DarkButtonStyle);
        m_backgroundColor = QColor(30, 30, 30, 200);
        update();
    }
    else {
        m_loginEdit->setStyleSheet(style->LightLineEditStyle);
        m_nameEdit->setStyleSheet(style->LightLineEditStyle);
        m_passwordEdit->setStyleSheet(style->LightLineEditStyle);
        m_password2Edit->setStyleSheet(style->LightLineEditStyle);
        m_registerButton->setStyleSheet(style->LightButtonStyle);
        m_backgroundColor = QColor(204, 204, 204, 200);
        update();
    }
}

void RegistrationComponent::slotToSendRegistrationData() {
    QString login = m_loginEdit->text();
    QString password = m_passwordEdit->text();
    QString name = m_nameEdit->text();
    emit sendRegistrationData(login, password, name);
}