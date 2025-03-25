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

struct StyleGreetWidget {
    QString buttonStyleBlue = R"(
            QPushButton {
                background-color: transparent; 
                color: rgb(21, 119, 232);   
                border: none;                  
                padding: 5px 10px;            
                font-family: 'Arial';          
                font-size: 20px;               
            }
            QPushButton:hover {
                color: rgb(26, 133, 255);       
            }
            QPushButton:pressed {
                color: rgb(26, 133, 255);                  
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
enum Theme;
class SendStringsGenerator;

class GreetWidget : public QWidget {
    Q_OBJECT

public:
    explicit GreetWidget(QWidget* parent, MainWindow* mw, Client* client, Theme theme, std::string login, ChatsWidget* cv);
    void startWelcomeAnimation();
    void setBackGround(Theme theme);
    void setName(const std::string& name);
    void setLogin(const std::string& login);

protected:
    void wheelEvent(QWheelEvent* event) override; // Переопределение метода для обработки колесика мыши
    void paintEvent(QPaintEvent* event) override;

private slots:
    void openImagePicker();
    void cropImageToCircle();
    void adjustCropArea(int value);

private:
    void saveCroppedImage();

private:
    MainWindow*        m_mainWindow;
    StyleGreetWidget* style;
    QPixmap           m_background;
    Client*           m_client;
    ChatsWidget*      m_chatsWidget;
    SendStringsGenerator* m_sender;


    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_buttonsHLayout;

    std::string m_login;

    QLabel* m_welcomeLabel;
    QLabel* m_imageLabel;
    QPushButton* m_selectImageButton;
    QPushButton* m_skipButton;
    QPushButton* m_continueButton;
    QString m_filePath;

    QSlider* m_cropXSlider;
    QSlider* m_cropYSlider;

    int m_cropX;
    int m_cropY;
    int m_cropWidth;
    int m_cropSize;
    int m_cropHeight;

    QPixmap m_selectedImage;
};