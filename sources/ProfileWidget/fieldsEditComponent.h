#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QRegularExpressionValidator>
#include <QGraphicsOpacityEffect>
#include <QWheelEvent>

struct StyleFieldsEditComponent {

    QString DarkLabelStyle = R"(
    QLabel {
        background-color: transparent;   
        color: white;             
    }
)";

    QString LightLabelStyle = R"(
    QLabel {
        background-color: transparent;   
        color: rgb(38, 38, 38);             
    }
)";

    QString DarkButtonStyleBlue = R"(
    QPushButton {
        background-color: rgb(21, 119, 232);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        font-family: "Segoe UI";  
        font-size: 14px;          
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";

    QString LightButtonStyleBlue = R"(
    QPushButton {
        background-color: rgb(26, 133, 255);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        font-family: "Segoe UI";  
        font-size: 14px;          
    }
    QPushButton:hover {
        background-color: rgb(21, 119, 232);   
    }
    QPushButton:pressed {
        background-color: rgb(21, 119, 232);      
    }
)";

    QString buttonStyleGrayDark = R"(
            QPushButton {
        background-color: rgb(115, 115, 115);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(135, 135, 135);   
    }
    QPushButton:pressed {
        background-color: rgb(115, 115, 115);      
    }
        )";

    QString buttonStyleGrayLight = R"(
            QPushButton {
        background-color: rgb(173, 173, 173);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(158, 158, 158);   
    }
    QPushButton:pressed {
        background-color: rgb(158, 158, 158);      
    }
        )";

    QString buttonToChoosePhotoStyleDark = "QPushButton {"
        "background-color: #2b2b2b;"
        "color: white;"
        "border-radius: 10px;"
        "padding: 8px;"
        "}"
        "QPushButton:hover {"
        "background-color: #3a3a3a;"
        "}";

    QString buttonToChoosePhotoStyleLight = "QPushButton {"
        "background-color: rgb(201, 201, 201);"
        "color: rgb(48, 48, 48);"
        "border-radius: 10px;"
        "padding: 8px;"
        "}"
        "QPushButton:hover {"
        "background-color: rgb(163, 163, 163);"
        "}";

    QString DarkLineEditStyle = R"(
    QLineEdit {
        background-color: #333;    
        color: white;     
        font-size: 12px;           
        border: none;     
        border-radius: 5px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: none;     
    }
)";

    QString LightLineEditStyle = R"(
    QLineEdit {
        background-color: #ffffff;    
        color: rgb(51, 51, 51);      
        font-size: 12px;            
        border: none;       
        border-radius: 5px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: none;     
    }
)";

    QString DarkLineEditDisabledStyle = R"(
    QLineEdit {
        background-color: rgb(89, 89, 89);    
        color: rgb(140, 140, 140);               
        border: none;     
        border-radius: 5px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid #888;    
    }
)";

    QString LightLineEditDisabledStyle = R"(
    QLineEdit {
        background-color: rgb(207, 207, 207);    
        color: rgb(143, 143, 143);               
        border: none;     
        border-radius: 5px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid #888;    
    }
)";

};



class EditDialogComponent;
class ProfileEditorWidget;
class Client;
enum  Theme;



class FieldsEditComponent : public QWidget {
    Q_OBJECT

public:
    FieldsEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget,  Client* client, Theme theme);
    void setTheme(Theme theme);
    void updateAvatar();


private:
    StyleFieldsEditComponent*  m_style;
    Theme                      m_theme;

    QVBoxLayout*    m_mainVLayout;
    QHBoxLayout*    m_buttonsHLayout;

    QLabel*         m_avatar_label;
    QPushButton*    m_change_photo_button;

    QLabel*         m_login_label;
    QLineEdit*      m_login_edit;

    QLabel*         m_name_label;
    QLineEdit*      m_name_edit;

    QLabel*         m_password_label;
    QLineEdit*      m_password_edit;

    QPushButton*    m_save_button;
    QPushButton*    m_cancel_button;


    ProfileEditorWidget* m_profile_editor_widget;
    Client* m_client;
};