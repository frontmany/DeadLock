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

class ProfileEditorWidget;
class Client;
class ConfigManager;
enum Theme;

struct StylePhotoEditComponent {
    StylePhotoEditComponent();
    QString ButtonSkipStyleBothTheme;
    QString DarkButtonStyleBlue;
    QString LightSliderStyle;
    QString LightButtonStyleBlue;
    QString ConfigManager;
    QString DarkSliderStyle;
    QString DarkHintStyle;
    QString LightHintStyle;
};

class PhotoEditComponent : public QWidget {
    Q_OBJECT

public:
    explicit PhotoEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme);
    void setTheme(Theme theme);

    QSlider* m_cropXSlider;
    QSlider* m_cropYSlider;

protected:
    void wheelEvent(QWheelEvent* event) override;
    int saveImage();

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

    std::string     m_filePath;
    QPixmap         m_selectedImage;

    std::shared_ptr<ConfigManager> m_config_manager;
    QLabel* m_hintLabel;
    QSpacerItem* spacer;
    QLabel* m_imageLabel;
    QPushButton* m_selectImageButton;
    QPushButton* m_cancelButton;
    QPushButton* m_continueButton;

    bool m_spacerAdded = false;
    int m_cropX;
    int m_cropY;
    int m_cropSize;
    int m_cropWidth;
    int m_cropHeight;
};