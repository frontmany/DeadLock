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
#include <QGraphicsOpacityEffect>
#include <QWheelEvent>

struct StyleFieldsEditComponent {
    QString buttonStyleBlueDark = R"(
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

    QString buttonToChoosePhotoStyleDark = "QPushButton {"
        "background-color: #2b2b2b;"
        "color: white;"
        "border-radius: 10px;"
        "padding: 8px;"
        "}"
        "QPushButton:hover {"
        "background-color: #3a3a3a;"
        "}";

    QString buttonCancelStyleDark = R"(
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
                color: rgb(26, 133, 255);      
            }
            QPushButton:pressed {
                color: rgb(26, 133, 255);      
            }
        )";

    QString DarkSliderStyle = R"(
QSlider::groove:horizontal {
    background-color: rgb(77, 77, 77); /* Ҹ���-����� ���� ������ */
    height: 8px; /* ������ ������ */
    border-radius: 4px; /* ����������� ����� ������ */
}

QSlider::handle:horizontal {
    background-color: white; /* ���� ����� */
    width: 16px; /* ������ ����� */
    height: 16px; /* ������ ����� */
    border-radius: 8px; /* ������� ����� ����� */
    margin: -4px 0; /* �������� ����� ������������ ������ */
}

QSlider::add-page:horizontal {
    background-color: rgb(77, 77, 77); /* Ҹ���-����� ���� ����������� ����� */
    border-radius: 4px; /* ����������� ����� */
}

QSlider::sub-page:horizontal {
    background-color: rgb(21, 119, 232); /* ����� ���� ������������� ����� */
    border-radius: 4px; /* ����������� ����� */
}

QSlider::groove:vertical {
    background-color: rgb(77, 77, 77); /* Ҹ���-����� ���� ������ */
    width: 8px; /* ������ ������ */
    border-radius: 4px; /* ����������� ����� ������ */
}

QSlider::handle:vertical {
    background-color: white; /* ���� ����� */
    width: 16px; /* ������ ����� */
    height: 16px; /* ������ ����� */
    border-radius: 8px; /* ������� ����� ����� */
    margin: 0 -4px; /* �������� ����� ������������ ������ */
}

QSlider::add-page:vertical {
    background-color: rgb(77, 77, 77); /* Ҹ���-����� ���� ����������� ����� */
    border-radius: 4px; /* ����������� ����� */
}

QSlider::sub-page:vertical {
    background-color: rgb(21, 119, 232); /* ����� ���� ������������� ����� */
    border-radius: 4px; /* ����������� ����� */
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

    QString DarkLineEditStyleRedBorder = R"(
    QLineEdit {
        background-color: #333;    
        color: white;               
        border: none;     
        border: color rgb(255, 64, 64);     
        border-radius: 5px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 2px solid rgb(255, 64, 64);    
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
    void setRedBorderOnLoginEdit();
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