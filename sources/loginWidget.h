#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>


class AuthorizationComponent;
class RegistrationComponent;

struct StyleLoginWidget {
    QString buttonStyleBlue = R"(
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

    QString buttonStyleGray = R"(
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

};

enum Theme {
    DARK,
    LIGHT
};

class LoginWidget : public QWidget {
    Q_OBJECT

public:
    explicit LoginWidget(QWidget* parent);
    void setTheme(Theme theme);

private slots:
    void onAuthorizeButtonClicked();
    void onRegisterButtonClicked();
    void switchToAuthorize();
    void switchToRegister();


private:
    void paintEvent(QPaintEvent* event) override;
    void setBackGround(Theme theme);

private:
    StyleLoginWidget*       style;
    QPixmap                 m_background;


    QVBoxLayout*            m_mainVLayout;
    QHBoxLayout*            m_switchersHLayout;
    QHBoxLayout*            m_FormsHLayout;

    AuthorizationComponent*     m_authorizationWidget;
    RegistrationComponent*     m_registrationWidget;

    QPushButton*            m_switchToAuthorizeButton;
    QPushButton*            m_switchToRegisterButton;
};
