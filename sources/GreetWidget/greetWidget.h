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
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QBuffer>
#include <QApplication>
#include <QStyle>



struct StyleGreetWidget {
    QString buttonStyleGray = R"(
            QPushButton {
                background-color: rgb(145, 145, 145); 
                color: rgb(255, 255, 255);   
                border: none;                  
                padding: 5px 10px;   
                border-radius: 5px;           
                font-family: 'Arial';          
                font-size: 20px;               
            }
            QPushButton:hover {
                color: rgb(255, 255, 255);     
                background-color: rgb( 173, 173, 173 ); 
            }
            QPushButton:pressed {
                background-color: rgb( 173, 173, 173 ); 
                color: rgb(255, 255, 255);                  
            }
        )";

    QString buttonSkipStyle = R"(
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

    QString DarkButtonStyle = R"(
    QPushButton {
        background-color: rgb(21, 119, 232);   
        color: white;             
        border: none;   
        border-radius: 28px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
    )";

    QString DarkSliderStyle = R"(
QSlider::groove:horizontal {
    background-color: rgb(77, 77, 77); /* Тёмно-серый цвет полосы */
    height: 8px; /* Высота полосы */
    border-radius: 4px; /* Закругление углов полосы */
}

QSlider::handle:horizontal {
    background-color: white; /* Цвет ручки */
    width: 16px; /* Ширина ручки */
    height: 16px; /* Высота ручки */
    border-radius: 8px; /* Круглая форма ручки */
    margin: -4px 0; /* Смещение ручки относительно полосы */
}

QSlider::add-page:horizontal {
    background-color: rgb(77, 77, 77); /* Тёмно-серый цвет заполненной части */
    border-radius: 4px; /* Закругление углов */
}

QSlider::sub-page:horizontal {
    background-color: rgb(21, 119, 232); /* Синий цвет незаполненной части */
    border-radius: 4px; /* Закругление углов */
}

QSlider::groove:vertical {
    background-color: rgb(77, 77, 77); /* Тёмно-серый цвет полосы */
    width: 8px; /* Ширина полосы */
    border-radius: 4px; /* Закругление углов полосы */
}

QSlider::handle:vertical {
    background-color: white; /* Цвет ручки */
    width: 16px; /* Ширина ручки */
    height: 16px; /* Высота ручки */
    border-radius: 8px; /* Круглая форма ручки */
    margin: 0 -4px; /* Смещение ручки относительно полосы */
}

QSlider::add-page:vertical {
    background-color: rgb(77, 77, 77); /* Тёмно-серый цвет заполненной части */
    border-radius: 4px; /* Закругление углов */
}

QSlider::sub-page:vertical {
    background-color: rgb(21, 119, 232); /* Синий цвет незаполненной части */
    border-radius: 4px; /* Закругление углов */
}
)";

};

class Client;
class MainWindow;
class ChatsWidget;
enum  Theme;
class PacketsBuilder;

class GreetWidget : public QWidget {
    Q_OBJECT

public:
    explicit GreetWidget(QWidget* parent, MainWindow* mw, Client* client, Theme theme, std::string login, ChatsWidget* cv);
    void startWelcomeAnimation();
    void setBackGround(Theme theme);
    void setName(const std::string& name);
    void setLogin(const std::string& login);

protected:
    void wheelEvent(QWheelEvent* event) override; 
    void paintEvent(QPaintEvent* event) override;

private slots:
    void openImagePicker();
    void cropImageToCircle();
    void adjustCropArea(int value);

private:
    int saveCroppedImage();

private:
    MainWindow*             m_mainWindow;
    StyleGreetWidget*       m_style;
    Client*                 m_client;
    ChatsWidget*            m_chatsWidget;
    PacketsBuilder*   m_sender;

    QVBoxLayout* m_mainVLayout;
    QVBoxLayout* m_containerVLayout;
    QHBoxLayout* m_buttonsHLayout;
    QHBoxLayout* m_greetLabelLayout;
    QHBoxLayout* m_sliderXLayout;
    QHBoxLayout* m_imageAndYSliderLayout;
    QVBoxLayout* m_bothSlidersVLayout;
    QWidget*     m_photoAndSlidersWidgetContainer;

    QPixmap         m_background;
    std::string     m_login;
    QString         m_filePath;
    QPixmap         m_selectedImage;

    QLabel*         m_welcomeLabel;
    QLabel*         m_imageLabel;
    QPushButton*    m_selectImageButton;
    QPushButton*    m_skipButton;
    QPushButton*    m_continueButton;
    QSlider*        m_cropXSlider;
    QSlider*        m_cropYSlider;
    
    QVBoxLayout* m_containerLayout;
    QWidget*        m_container;

    int m_cropX;
    int m_cropY;
    int m_cropWidth;
    int m_cropSize;
    int m_cropHeight;


};