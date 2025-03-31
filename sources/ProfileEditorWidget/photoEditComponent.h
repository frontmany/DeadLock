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

struct StylePhotoEditComponent {

    QString buttonSkipStyleBothTheme = R"(
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

    QString LightSliderStyle = R"(
QSlider::groove:horizontal {
    background-color: rgb(168, 168, 168); /* Тёмно-серый цвет полосы */
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
    background-color: rgb(168, 168, 168); /* Тёмно-серый цвет заполненной части */
    border-radius: 4px; /* Закругление углов */
}

QSlider::sub-page:horizontal {
    background-color: rgb(21, 119, 232); /* Синий цвет незаполненной части */
    border-radius: 4px; /* Закругление углов */
}

QSlider::groove:vertical {
    background-color: rgb(168, 168, 168); /* Тёмно-серый цвет полосы */
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
    background-color: rgb(168, 168, 168); /* Тёмно-серый цвет заполненной части */
    border-radius: 4px; /* Закругление углов */
}

QSlider::sub-page:vertical {
    background-color: rgb(21, 119, 232); /* Синий цвет незаполненной части */
    border-radius: 4px; /* Закругление углов */
}
)";

};


class ProfileEditorWidget;
class Client;
enum Theme;

class PhotoEditComponent : public QWidget {
    Q_OBJECT

public:
    explicit PhotoEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, Theme theme);
    void setTheme(Theme theme);

    QSlider* m_cropXSlider;
    QSlider* m_cropYSlider;

protected:
    void wheelEvent(QWheelEvent* event) override;
    void saveCroppedImage();

private slots:
    void openImagePicker();
    void cropImageToCircle();
    void adjustCropArea();

private:
    ProfileEditorWidget* m_profile_editor_widget;
    Client* m_client;

    StylePhotoEditComponent* m_style;
    Theme m_theme;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_buttonsHLayout;
    QHBoxLayout* m_sliderXLayout;
    QHBoxLayout* m_imageAndYSliderLayout;
    QVBoxLayout* m_bothSlidersVLayout;
    QWidget*     m_photoAndSlidersWidgetContainer;

    QString         m_filePath;
    QPixmap         m_selectedImage;

    QLabel* m_imageLabel;
    QPushButton* m_selectImageButton;
    QPushButton* m_cancelButton;
    QPushButton* m_continueButton;

    int m_cropX;
    int m_cropY;
    int m_cropSize;
    int m_cropWidth;
    int m_cropHeight;


};