#include "authorizationComponent.h"
#include "loginWidget.h"
#include "mainWindow.h"


#include <QDebug>


StyleAuthorizationComponent::StyleAuthorizationComponent() {
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

    LightButtonStyle = R"(
    QPushButton {
        background-color: rgb(26, 133, 255);
        color: white;                
        border: none;      
        border-radius: 5px;          
        padding: 5px 10px;           
    }
    QPushButton:hover {
        background-color: rgb(21, 119, 232);     
    }
    QPushButton:pressed {
        background-color: rgb(21, 119, 232);      
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
        border: 2px solid #ff4444;
        border-radius: 5px;
        padding: 5px;
    }
    QLineEdit:focus {
        border: 2px solid #ff4444;  /* ярко-красный при фокусе */
        background-color: #fff8f8;   /* —легка красноватый фон */
    }
    QLineEdit:hover {
        border: 2px solid #ff4444;
    }
)";


    DarkLineEditStyleRedBorder = R"(
    QLineEdit {
        background-color: #333;
        color: white;
        border: 2px solid rgb(255, 66, 66);
        border-radius: 5px;
        padding: 5px;
    }
    QLineEdit:focus {
        border: 2px solid rgb(255, 66, 66);
    }
    QLineEdit:hover {
        border: 2px solid rgb(255, 66, 66);
    }
)";
}

AuthorizationComponent::AuthorizationComponent(QWidget* parent, LoginWidget* loginWidget)
    : QWidget(parent)
{
   style = new StyleAuthorizationComponent;
   m_backgroundColor = QColor(30, 30, 30, 200);

   m_loginEdit = new QLineEdit(this);
   m_loginEdit->setPlaceholderText("Login"); 
   m_loginEdit->setMaximumSize(500, 40);
   connect(m_loginEdit, &QLineEdit::textChanged,
       this, &AuthorizationComponent::resetLoginEditStyle);

    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setPlaceholderText("Password"); 
    m_passwordEdit->setMaximumSize(500, 40);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    connect(m_passwordEdit, &QLineEdit::textChanged,
        this, &AuthorizationComponent::resetPasswordEditStyle);


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

    
    QHBoxLayout* errorLabelHLayout = new QHBoxLayout;
    errorLabelHLayout->setAlignment(Qt::AlignLeft);
    m_error_label = new QLabel;
    m_error_label->setStyleSheet("color: rgb(252, 81, 81);");
    m_error_label->setFixedHeight(30);
    m_error_label->hide();
    m_error_label->setWordWrap(true);
    errorLabelHLayout->addSpacing(25);
    errorLabelHLayout->addWidget(m_error_label);

    layout->addLayout(m_loginEditHla);
    layout->addLayout(m_passwordEditHla);
    layout->addLayout(errorLabelHLayout);
    layout->addLayout(m_loginButtonHla);
    setLayout(layout);
    setFixedSize(450, 300); 
}

void AuthorizationComponent::setErrorMessageToLabel(const QString& errorText) {
    m_error_label->setText(errorText);
    m_error_label->show();
}

void AuthorizationComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backgroundColor);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 10, 10);

    QWidget::paintEvent(event);
}


void AuthorizationComponent::setTheme(Theme& theme) {
    m_theme = &theme;
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

    QRegularExpression loginRegex("^[a-zA-Z0-9_]{4,20}$");

    if (!loginRegex.match(login).hasMatch()) {
        setRedBorderToLoginEdit();
        setErrorMessageToLabel("Invalid username. Must be 4-20 characters (letters, numbers, or _)");
        return;
    }

    QString password = m_passwordEdit->text();

    if (password.isEmpty()) {
        setRedBorderToPasswordEdit();
        setErrorMessageToLabel("Password cannot be empty");
        return;
    }

    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).+$");
    if (!passwordRegex.match(password).hasMatch()) {
        setRedBorderToPasswordEdit();
        setErrorMessageToLabel("Invalid password. Must contain uppercase, lowercase and a number");
        return;
    }

    emit sendLoginData(login, password);
}

void AuthorizationComponent::setRedBorderToLoginEdit() {
    m_error_label->show();
    if (*m_theme == DARK) {
        m_loginEdit->setStyleSheet(style->DarkLineEditStyleRedBorder);
    }
    if (*m_theme == LIGHT) {
        m_loginEdit->setStyleSheet(style->LightLineEditStyleRedBorder);
    }
}


void AuthorizationComponent::setRedBorderToPasswordEdit() {
    m_error_label->show();
    if (*m_theme == DARK) {
        m_passwordEdit->setStyleSheet(style->DarkLineEditStyleRedBorder);
    }
    if (*m_theme == LIGHT) {
        m_passwordEdit->setStyleSheet(style->LightLineEditStyleRedBorder);
    }
}

void AuthorizationComponent::resetLoginEditStyle() {
    m_error_label->hide();
    if (*m_theme == DARK) {
        m_loginEdit->setStyleSheet(style->DarkLineEditStyle);
    }
    if (*m_theme == LIGHT) {
        m_loginEdit->setStyleSheet(style->LightLineEditStyle);
    }
}

void AuthorizationComponent::resetPasswordEditStyle() {
    m_error_label->hide();
    if (*m_theme == DARK) {
        m_passwordEdit->setStyleSheet(style->DarkLineEditStyle);
    }
    if (*m_theme == LIGHT) {
        m_passwordEdit->setStyleSheet(style->LightLineEditStyle);
    }
}