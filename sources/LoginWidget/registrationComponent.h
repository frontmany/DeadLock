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

struct StyleRegistrationComponent {

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
        border: none;     
        border-radius: 5px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid #888;    
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
        border: none;       
        border-radius: 5px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: 2px solid rgb(237, 237, 237);         
    }
)";



};

class LoginWidget;


class RegistrationComponent : public QWidget {
    Q_OBJECT

public:
    explicit RegistrationComponent(QWidget* parent, LoginWidget* loginWidget);
    void setTheme(Theme theme);


protected:
    void paintEvent(QPaintEvent* event) override;


private slots:
    void slotToSendRegistrationData();

signals:
    void sendRegistrationData(QString& login, QString& data, QString& name);



private:
    StyleRegistrationComponent* style;
    QColor                      m_backgroundColor;


    QHBoxLayout* m_registerButtonHla;
    QHBoxLayout* m_loginEditHla;
    QHBoxLayout* m_passwordEditHla;
    QHBoxLayout* m_password2EditHla;
    QHBoxLayout* m_nameEditHla;

    QPushButton* m_registerButton;
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
    QLineEdit* m_password2Edit;
    QLineEdit* m_nameEdit;
};