#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>

#include "loginWidget.h"

struct StyleAuthorizationComponent {

    QString DarkButtonStyle = R"(
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

    QString DarkLineEditStyle = R"(
    QLineEdit {
        background-color: #333;    
        color: white;               
        border: 1px solid #666;     
        border-radius: 5px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 1px solid #888;     
    }
)";

    QString LightButtonStyle = R"(
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

    QString LightLineEditStyle = R"(
    QLineEdit {
        background-color: #ffffff;    
        color: black;                 
        border: 1px solid #ccc;       
        border-radius: 5px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: 1px solid #888;        
    }
)";



};

class LoginWidget;

class AuthorizationComponent : public QWidget {
    Q_OBJECT

public:
    explicit AuthorizationComponent(QWidget* parent, LoginWidget* loginWidget);
    void setTheme(Theme theme);


protected:
    void paintEvent(QPaintEvent* event) override;
    

private slots:
    void SlotToSendLoginData();

signals:
    void sendLoginData(QString& login, QString& password);


private:
    StyleAuthorizationComponent*    style;
    QColor                      m_backgroundColor;


    QHBoxLayout*                m_loginButtonHla;
    QHBoxLayout*                m_loginEditHla;
    QHBoxLayout*                m_passwordEditHla;

    QPushButton*                m_loginButton;
    QLineEdit*                  m_loginEdit;
    QLineEdit*                  m_passwordEdit;
};