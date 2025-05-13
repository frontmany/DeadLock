#include "loginWidget.h"
#include "authorizationComponent.h"
#include "registrationComponent.h"
#include "mainwindow.h"
#include "client.h"
#include "utility.h"
#include <QGraphicsBlurEffect>
#include <QPainterPath>

StyleLoginWidget::StyleLoginWidget() {
    buttonStyleBlue = R"(
        QPushButton {
            background-color: transparent; 
            color: rgb(21, 119, 232);   
            border: none;                  
            padding: 5px 10px;            
            font-family: 'Arial';          
            font-size: 14px;               
        }
        QPushButton:hover {
            color: rgb(26, 133, 255);       
        }
        QPushButton:pressed {
            color: rgb(26, 133, 255);                  
        }
    )";

    buttonStyleGray = R"(
        QPushButton {
            background-color: transparent;     
            color: rgb(153, 150, 150);              
            border: none;        
            border-radius: 5px;             
            padding: 5px 10px;              
            font-family: 'Arial';            
            font-size: 14px;                 
        }
        QPushButton:hover {
            color: rgb(153, 150, 150);      
        }
        QPushButton:pressed {
            color: rgb(153, 150, 150);      
        }
    )";
}


LoginWidget::LoginWidget(QWidget* parent, MainWindow* mw, Client* client)
    : QWidget(parent), m_client(client){

    m_switchState = AUTHORIZATION;
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

    m_authorizationWidget = new AuthorizationComponent(this, this);
    m_registrationWidget = new RegistrationComponent(this, this);
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

void LoginWidget::onAuthorizeButtonClicked(QString& login, QString& password) {
    m_client->authorizeClient(login.toStdString(), utility::hashPassword(password.toStdString()));
    m_client->setMyLogin(login.toStdString());
}

void LoginWidget::onRegisterButtonClicked(QString& login, QString& password, QString& name) {
    m_client->registerClient(login.toStdString(), utility::hashPassword(password.toStdString()), name.toStdString());
     m_client->setMyLogin(login.toStdString());
    m_client->setMyName(name.toStdString());
}

void LoginWidget::switchToAuthorize() {
    m_registrationWidget->hide();
    m_authorizationWidget->show();
    m_switchState = AUTHORIZATION;
    swapSwitchStyles();
}

void LoginWidget::switchToRegister() {
    m_authorizationWidget->hide();
    m_registrationWidget->show();
    m_switchState = REGISTRATION;
    swapSwitchStyles();
}

void LoginWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    QPainterPath roundedRectPath;
    int cornerRadius = 0; 
    QRect rect(0, 0, width(), height()); 

    roundedRectPath.addRoundedRect(rect, cornerRadius, cornerRadius);

    QGraphicsBlurEffect blurEffect;
    blurEffect.setBlurRadius(10); 
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter.save();
    painter.setClipPath(roundedRectPath);
    painter.setOpacity(0.7); 
    painter.fillPath(roundedRectPath, QColor(26, 26, 26, 200)); 
    painter.restore();

    
    painter.drawPixmap(this->rect(), m_background); 
    QWidget::paintEvent(event);
}

void LoginWidget::setTheme(Theme& theme) {
    swapSwitchStyles();
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

void LoginWidget::swapSwitchStyles() {
    if (m_switchState == AUTHORIZATION) {
        m_switchToAuthorizeButton->setStyleSheet(style->buttonStyleBlue);
        m_switchToRegisterButton->setStyleSheet(style->buttonStyleGray);
    }
    if (m_switchState == REGISTRATION) {
        m_switchToAuthorizeButton->setStyleSheet(style->buttonStyleGray);
        m_switchToRegisterButton->setStyleSheet(style->buttonStyleBlue);
    }
}


AuthorizationComponent* LoginWidget::getAuthorizationComponent() {
    return m_authorizationWidget;
}

RegistrationComponent* LoginWidget::getRegistrationComponent() {
    return m_registrationWidget;
}