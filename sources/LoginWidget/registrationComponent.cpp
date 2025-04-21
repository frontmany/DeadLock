#include "registrationComponent.h"
#include "mainWindow.h"
#include "loginWidget.h"

StyleRegistrationComponent::StyleRegistrationComponent() {
    DarkButtonStyle = R"(
    QPushButton {
        background-color: rgb(21, 119, 232);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";


    LightButtonStyle = R"(
    QPushButton {
        background-color: rgb(21, 119, 232);    
        color: white;                
        border: none;      
        border-radius: 5px;          
        padding: 5px 10px;           
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);     
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";


    LightLineEditStyle = R"(
    QLineEdit {
        background-color: #ffffff;    
        color: black;                 
        border: none;       
        border-radius: 5px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: 2px solid rgb(237, 237, 237);         
    }
)";

    LightLineEditStyleRedBorder = R"(
    QLineEdit {
        background-color: #ffffff;
        color: black;
        border: 1px solid #e0e0e0;
        border-radius: 5px;
        padding: 5px;
    }
    QLineEdit:focus {
        border: 2px solid #ff4444;  /* ярко-красный при фокусе */
        background-color: #fff8f8;   /* —легка красноватый фон */
    }
    QLineEdit:hover {
        border: 1px solid #c0c0c0;
    }
    QLineEdit:disabled {
        background-color: #f5f5f5;
        color: #a0a0a0;
        border: 1px solid #eaeaea;
    }
    QLineEdit:invalid {
        border: 2px solid #ff9999;  /* ћ€гкий красный дл€ невалидных */
    }
)";

    DarkLineEditStyle = R"(
    QLineEdit {
        background-color: #333;    
        color: white;               
        border: none;     
        border-radius: 5px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid #888;    
    }
)";

    DarkLineEditStyleRedBorder = R"(
    QLineEdit {
        background-color: #333;
        color: white;
        border: none;
        border-radius: 5px;
        padding: 5px;
    }
    QLineEdit:focus {
        border: 2px solid red;
    }
    QLineEdit:hover {
        border: 1px solid #555;
    }
    QLineEdit:disabled {
        background-color: #222;
        color: #777;
    }
)";

}

RegistrationComponent::RegistrationComponent(QWidget* parent, LoginWidget* loginWidget)
    : QWidget(parent) {

    style = new StyleRegistrationComponent;
    m_backgroundColor = QColor(30, 30, 30, 200);

    m_loginEdit = new QLineEdit(this);
    m_loginEdit->setPlaceholderText("Login");
    m_loginEdit->setMaximumSize(500, 40);
    connect(m_loginEdit, &QLineEdit::textChanged,
        this, &RegistrationComponent::resetLoginEditStyle);


    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password");
    m_passwordEdit->setMaximumSize(500, 40);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    connect(m_passwordEdit, &QLineEdit::textChanged,
        this, &RegistrationComponent::resetPasswordEditsStyles);


    m_password2Edit = new QLineEdit(this);
    m_password2Edit->setPlaceholderText("Confirm password");
    m_password2Edit->setMaximumSize(500, 40);
    m_password2Edit->setEchoMode(QLineEdit::Password);
    connect(m_password2Edit, &QLineEdit::textChanged,
        this, &RegistrationComponent::resetPasswordEditsStyles);

    m_nameEdit = new QLineEdit(this);
    m_nameEdit->setPlaceholderText("Name");
    m_nameEdit->setMaximumSize(500, 40);
    connect(m_nameEdit, &QLineEdit::textChanged,
        this, &RegistrationComponent::resetNameEditStyle);


    QRegularExpressionValidator* validator = new QRegularExpressionValidator(
        QRegularExpression("^[a-zA-Z0-9_!@#$%^&*()-+=;:'\",.<>?/\\\\|`~\\[\\]{} ]*$"),
        this
    );

    m_loginEdit->setValidator(validator);
    m_passwordEdit->setValidator(validator);
    m_password2Edit->setValidator(validator);

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



void RegistrationComponent::setTheme(Theme& theme) {
    m_theme = &theme;
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
    /*
    QRegularExpression loginRegex("^[a-zA-Z0-9_]{4,20}$");

    if (!loginRegex.match(login).hasMatch()) {
        setRedBorderToLoginEdit();
        return;
    }
     */

    QString name = m_nameEdit->text().trimmed();
    /*
    if (name.length() < 2 || name.length() > 50) {
        setRedBorderToNameEdit();
        return;
    }
   */
    QString password = m_passwordEdit->text();
    QString password2 = m_password2Edit->text();
    /*
    if (password.length() < 8) {
        setRedBorderToPasswordEdits();
        return;
    }

    if (password != password2) {
        setRedBorderToPasswordEdits();
        return;
    }

    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).+$");
    if (!passwordRegex.match(password).hasMatch()) {
        setRedBorderToPasswordEdits();
        return;
    }
    */
    emit sendRegistrationData(login, password, name);
}

void RegistrationComponent::setRedBorderToLoginEdit() {
    if (*m_theme == DARK) {
        m_loginEdit->setStyleSheet(style->DarkLineEditStyleRedBorder);
    }
    if (*m_theme == LIGHT){
        m_loginEdit->setStyleSheet(style->LightLineEditStyleRedBorder);
    }
}

void RegistrationComponent::setRedBorderToNameEdit() {
    if (*m_theme == DARK) {
        m_nameEdit->setStyleSheet(style->DarkLineEditStyleRedBorder);
    }
    if (*m_theme == LIGHT) {
        m_nameEdit->setStyleSheet(style->LightLineEditStyleRedBorder);
    }
}

void RegistrationComponent::setRedBorderToPasswordEdits() {
    if (*m_theme == DARK) {
        m_passwordEdit->setStyleSheet(style->DarkLineEditStyleRedBorder);
        m_password2Edit->setStyleSheet(style->DarkLineEditStyleRedBorder);
    }
    if (*m_theme == LIGHT) {
        m_passwordEdit->setStyleSheet(style->LightLineEditStyleRedBorder);
        m_password2Edit->setStyleSheet(style->LightLineEditStyleRedBorder);
    }
}


void RegistrationComponent::resetLoginEditStyle() {
    if (*m_theme == DARK) {
        m_loginEdit->setStyleSheet(style->DarkLineEditStyle);
    }
    if (*m_theme == LIGHT) {
        m_loginEdit->setStyleSheet(style->LightLineEditStyle);
    }
}

void RegistrationComponent::resetNameEditStyle() {
    if (*m_theme == DARK) {
        m_nameEdit->setStyleSheet(style->DarkLineEditStyle);
    }
    if (*m_theme == LIGHT) {
        m_nameEdit->setStyleSheet(style->LightLineEditStyle);
    }
}

void RegistrationComponent::resetPasswordEditsStyles() {
    if (*m_theme == DARK) {
        m_passwordEdit->setStyleSheet(style->DarkLineEditStyle);
        m_password2Edit->setStyleSheet(style->DarkLineEditStyle);
    }
    if (*m_theme == LIGHT) {
        m_passwordEdit->setStyleSheet(style->LightLineEditStyle);
        m_password2Edit->setStyleSheet(style->LightLineEditStyle);
    }
}