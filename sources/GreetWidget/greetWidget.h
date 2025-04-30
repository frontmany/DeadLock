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
    StyleGreetWidget();

    QString buttonStyleGray;

    QString DarkbuttonSkipStyle;
    QString LightButtonSkipStyle;

    QString DarkButtonStyle;
    QString LightButtonStyle;

    QString DarkSliderStyle;
    QString LightSliderStyle;

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
    void setTheme(Theme theme);

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
    
    QVBoxLayout*    m_containerLayout;
    QWidget*        m_container;

    Theme           m_theme;

    int m_cropX;
    int m_cropY;
    int m_cropWidth;
    int m_cropSize;
    int m_cropHeight;


};