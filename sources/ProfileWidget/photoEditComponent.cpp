#include "theme.h"
#include "photoEditComponent.h"
#include "fieldsEditComponent.h"
#include "profileEditorWidget.h"
#include "client.h"
#include "utility.h"
#include "photo.h"
#include "chatsWidget.h"
#include "configManager.h"
#include "chatsListComponent.h"

StylePhotoEditComponent::StylePhotoEditComponent() {
    ButtonSkipStyleBothTheme = R"(
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

    DarkButtonStyleBlue = R"(
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

    LightButtonStyleBlue = R"(
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

    DarkSliderStyle = R"(
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

    LightSliderStyle = R"(
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


PhotoEditComponent::PhotoEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme)
    : QWidget(parent), m_profile_editor_widget(profileEditorWidget), m_client(client), m_theme(theme),
    m_cropX(0), m_cropY(0), m_cropWidth(100), m_cropHeight(100), m_config_manager(configManager) {
    m_style = new StylePhotoEditComponent;

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setContentsMargins(utility::getScaledSize(20), utility::getScaledSize(20), utility::getScaledSize(20), utility::getScaledSize(20));
    m_mainVLayout->setAlignment(Qt::AlignTop);


    m_imageLabel = new QLabel(this);
    QPixmap pixmap;
    if (m_theme == DARK) {
        pixmap = QPixmap(":/resources/GreetWidget/loadPhotoDark.png");
    }
    else {
        pixmap = QPixmap(":/resources/GreetWidget/loadPhoto.png");
    }
    m_imageLabel->setPixmap(pixmap.scaled(utility::getScaledSize(450), utility::getScaledSize(450), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    m_cancelButton = new QPushButton("cancel", this);
    m_cancelButton->setMinimumSize(utility::getScaledSize(100), utility::getScaledSize(60));
    m_cancelButton->setMaximumSize(utility::getScaledSize(150), utility::getScaledSize(60));
    connect(m_cancelButton, &QPushButton::clicked, [this]() {
        m_profile_editor_widget->setFieldsEditor();
        QPixmap pixmap;
        if (m_theme == DARK) {
            pixmap = QPixmap(":/resources/GreetWidget/loadPhotoDark.png");
        }
        else {
            pixmap = QPixmap(":/resources/GreetWidget/loadPhoto.png");
        }
        m_imageLabel->setPixmap(pixmap.scaled(utility::getScaledSize(450), utility::getScaledSize(450), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_imageLabel->setFixedSize(utility::getScaledSize(450), utility::getScaledSize(450));
        m_cropXSlider->hide();
        m_cropYSlider->hide();
    });


    m_selectImageButton = new QPushButton("Choose a photo", this);
    m_selectImageButton->setMinimumSize(utility::getScaledSize(200), utility::getScaledSize(60));
    m_selectImageButton->setMaximumSize(utility::getScaledSize(250), utility::getScaledSize(60));
    connect(m_selectImageButton, &QPushButton::clicked, this, &PhotoEditComponent::openImagePicker);

    m_continueButton = new QPushButton("Save", this);
    m_continueButton->setEnabled(false);
    m_continueButton->setMinimumSize(utility::getScaledSize(200), utility::getScaledSize(60));
    m_continueButton->setMaximumSize(utility::getScaledSize(250), utility::getScaledSize(60));
    connect(m_continueButton, &QPushButton::clicked, [this]() {
        saveCroppedImage();
        Photo* photo = new Photo(m_config_manager->getClient()->getPrivateKey(), m_filePath);
        m_config_manager->setIsHasPhoto(true);
        m_config_manager->setPhoto(photo);
        m_client->updateMyPhoto(*photo);

        m_profile_editor_widget->setFieldsEditor();
        m_profile_editor_widget->updateAvatar(*photo);
        m_profile_editor_widget->setFieldsEditor();
    });

    m_cropXSlider = new QSlider(Qt::Horizontal, this);
    m_cropXSlider->setFixedSize(utility::getScaledSize(200), utility::getScaledSize(20));
    m_cropXSlider->hide();
    m_cropXSlider->setRange(0, 500);
    m_cropXSlider->setValue(m_cropX);
    connect(m_cropXSlider, &QSlider::valueChanged, this, &PhotoEditComponent::adjustCropArea);

    m_sliderXLayout = new QHBoxLayout();
    m_sliderXLayout->setAlignment(Qt::AlignCenter);
    m_sliderXLayout->addSpacing(50);
    m_sliderXLayout->addWidget(m_cropXSlider);


    m_cropYSlider = new QSlider(Qt::Vertical, this);
    m_cropYSlider->setRange(0, 330);
    m_cropYSlider->setInvertedAppearance(true);
    m_cropYSlider->setValue(m_cropY);
    m_cropYSlider->setFixedSize(utility::getScaledSize(20), utility::getScaledSize(200));
    m_cropYSlider->hide();
    connect(m_cropYSlider, &QSlider::valueChanged, this, &PhotoEditComponent::adjustCropArea);

    m_buttonsHLayout = new QHBoxLayout();
    m_buttonsHLayout->setAlignment(Qt::AlignCenter);
    m_buttonsHLayout->addWidget(m_selectImageButton);
    m_buttonsHLayout->addSpacing(30);
    m_buttonsHLayout->addWidget(m_continueButton);
    m_buttonsHLayout->addWidget(m_cancelButton);
    m_buttonsHLayout->addSpacing(-120);

    m_imageAndYSliderLayout = new QHBoxLayout();
    m_imageAndYSliderLayout->setAlignment(Qt::AlignCenter);
    m_imageAndYSliderLayout->addWidget(m_cropYSlider);
    m_imageAndYSliderLayout->addSpacing(utility::getScaledSize(20));
    m_imageAndYSliderLayout->addWidget(m_imageLabel);

    m_bothSlidersVLayout = new QVBoxLayout();
    m_bothSlidersVLayout->setAlignment(Qt::AlignTop);
    m_bothSlidersVLayout->addLayout(m_imageAndYSliderLayout);
    m_bothSlidersVLayout->addSpacing(40);
    m_bothSlidersVLayout->addLayout(m_sliderXLayout);

    m_photoAndSlidersWidgetContainer = new QWidget;
    m_photoAndSlidersWidgetContainer->setMaximumSize(utility::getScaledSize(2000), utility::getScaledSize(600));
    m_photoAndSlidersWidgetContainer->setLayout(m_bothSlidersVLayout);

    m_mainVLayout->addWidget(m_photoAndSlidersWidgetContainer);
    m_mainVLayout->addSpacing(utility::getScaledSize(20));
    m_mainVLayout->addLayout(m_buttonsHLayout);
    m_mainVLayout->addSpacing(30);
    setLayout(m_mainVLayout);
    setMouseTracking(true);

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setTheme(m_theme);
} 

void PhotoEditComponent::setTheme(Theme theme) {
    m_theme = theme;

    QPixmap pixmap;
    if (m_theme == DARK) {
        pixmap = QPixmap(":/resources/GreetWidget/loadPhotoDark.png");
    }
    else {
        pixmap = QPixmap(":/resources/GreetWidget/loadPhoto.png");
    }
    m_imageLabel->setPixmap(pixmap.scaled(utility::getScaledSize(450), utility::getScaledSize(450), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    if (theme == Theme::DARK) {
        m_cancelButton->setStyleSheet(m_style->ButtonSkipStyleBothTheme);
        m_selectImageButton->setStyleSheet(m_style->DarkButtonStyleBlue);
        m_continueButton->setStyleSheet(m_style->DarkButtonStyleBlue);
        m_cropXSlider->setStyleSheet(m_style->DarkSliderStyle);
        m_cropYSlider->setStyleSheet(m_style->DarkSliderStyle);

    }
    if (theme == Theme::LIGHT) {
        m_cancelButton->setStyleSheet(m_style->ButtonSkipStyleBothTheme);
        m_selectImageButton->setStyleSheet(m_style->LightButtonStyleBlue);
        m_continueButton->setStyleSheet(m_style->LightButtonStyleBlue);
        m_cropXSlider->setStyleSheet(m_style->LightSliderStyle);
        m_cropYSlider->setStyleSheet(m_style->LightSliderStyle);
    }
}

void PhotoEditComponent::openImagePicker() {
    QString imagePath = QFileDialog::getOpenFileName(this, "Choose Photo", "", "Images (*.png *.jpg *.jpeg)");
    if (!imagePath.isEmpty()) {
        m_selectedImage.load(imagePath);

        int paramScale = utility::getScaledSize(500);
        m_selectedImage = m_selectedImage.scaled(paramScale, paramScale, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        int imageWidth = m_selectedImage.width();
        int imageHeight = m_selectedImage.height();

        if (imageWidth < paramScale || imageHeight < paramScale) {
            m_imageLabel->setFixedSize(imageWidth, imageHeight);
            m_cropXSlider->setFixedSize(imageWidth, 30);
            m_cropYSlider->setFixedSize(30, imageHeight);
        }
        else {
            m_imageLabel->setFixedSize(utility::getScaledSize(400), utility::getScaledSize(400));
        }

        m_imageLabel->setPixmap(m_selectedImage);

        m_cropSize = qMin(imageWidth, imageHeight) / 2; 
        m_cropX = (imageWidth - m_cropSize) / 2;
        m_cropY = (imageHeight - m_cropSize) / 2; 

        m_cropXSlider->setRange(0, imageWidth - m_cropSize);
        m_cropYSlider->setRange(0, imageHeight - m_cropSize);
        m_cropXSlider->setValue(m_cropX);
        m_cropYSlider->setValue(m_cropY);

        m_continueButton->setEnabled(true);
        cropImageToCircle();

        m_cropXSlider->show();
        m_cropYSlider->show();
    }
}

void PhotoEditComponent::cropImageToCircle() {
    if (m_selectedImage.isNull()) return;
    QSize imageSize = m_imageLabel->size();

    QPixmap blurredImage = m_selectedImage.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QGraphicsBlurEffect* blurEffect = new QGraphicsBlurEffect;
    blurEffect->setBlurRadius(10);
    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(blurredImage);
    item.setGraphicsEffect(blurEffect);
    scene.addItem(&item);
    QImage blurredImageResult(blurredImage.size(), QImage::Format_ARGB32);
    blurredImageResult.fill(Qt::transparent);
    QPainter painter(&blurredImageResult);
    scene.render(&painter);
    painter.end();

    QPixmap darkenedBlurredImage = QPixmap::fromImage(blurredImageResult);
    QPainter darkenPainter(&darkenedBlurredImage);
    darkenPainter.setCompositionMode(QPainter::CompositionMode_Multiply);
    darkenPainter.fillRect(darkenedBlurredImage.rect(), QColor(100, 100, 100, 150)); 
    darkenPainter.end();

    QPixmap circularMask(m_cropSize, m_cropSize);
    circularMask.fill(Qt::transparent);

    QPainter circularPainter(&circularMask);
    circularPainter.setRenderHint(QPainter::Antialiasing);

    circularPainter.setPen(QPen(Qt::NoPen));
    circularPainter.setBrush(Qt::NoBrush);
    circularPainter.drawEllipse(1, 1, m_cropSize - 2, m_cropSize - 2);
    circularPainter.end();

    QPixmap background(imageSize);
    background.fill(Qt::transparent);

    QPainter backgroundPainter(&background);
    backgroundPainter.setRenderHint(QPainter::Antialiasing);

    backgroundPainter.drawPixmap(0, 0, darkenedBlurredImage);

    QPixmap originalImage = m_selectedImage.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    QPixmap circularArea(m_cropSize, m_cropSize);
    circularArea.fill(Qt::transparent);

    QPainter circularAreaPainter(&circularArea);
    circularAreaPainter.setRenderHint(QPainter::Antialiasing);
    circularAreaPainter.setBrush(Qt::white);
    circularAreaPainter.setPen(Qt::NoPen); 
    circularAreaPainter.drawEllipse(0, 0, m_cropSize, m_cropSize);
    circularAreaPainter.end();

    QPixmap maskedOriginalImage = originalImage.copy(m_cropX, m_cropY, m_cropSize, m_cropSize);
    maskedOriginalImage.setMask(circularArea.createMaskFromColor(Qt::transparent));

    backgroundPainter.drawPixmap(m_cropX, m_cropY, maskedOriginalImage);
    backgroundPainter.drawPixmap(m_cropX, m_cropY, circularMask);

    backgroundPainter.end();

    m_imageLabel->setPixmap(background);
}

void PhotoEditComponent::adjustCropArea() {
    int imageWidth = m_imageLabel->width();
    int imageHeight = m_imageLabel->height();

    m_cropSize = qMin(m_cropSize, qMin(imageWidth, imageHeight));

    int maxX = imageWidth - m_cropSize;
    int maxY = imageHeight - m_cropSize;

    m_cropX = qBound(0, m_cropXSlider->value(), maxX);
    m_cropY = qBound(0, m_cropYSlider->value(), maxY);

    if (!m_selectedImage.isNull()) {
        cropImageToCircle();
    }
}

void PhotoEditComponent::wheelEvent(QWheelEvent* event) {
    if (m_selectedImage.isNull()) return;

    int delta = event->angleDelta().y(); 
    int newSize = m_cropSize + (delta > 0 ? 10 : -10);

    newSize = qBound(50, newSize, 500);
    m_cropSize = newSize;

    adjustCropArea();
    cropImageToCircle();
}

void PhotoEditComponent::saveCroppedImage() {
    if (m_selectedImage.isNull()) {
        qWarning() << "Нет изображения для сохранения";
        return;
    }

    QPixmap circularMask(m_cropSize, m_cropSize);
    circularMask.fill(Qt::transparent);
    QPainter maskPainter(&circularMask);
    maskPainter.setRenderHint(QPainter::Antialiasing);
    maskPainter.setBrush(Qt::white);
    maskPainter.setPen(Qt::NoPen);
    maskPainter.drawEllipse(0, 0, m_cropSize, m_cropSize);
    maskPainter.end();

    QPixmap croppedImage = m_selectedImage.copy(m_cropX, m_cropY, m_cropSize, m_cropSize);
    croppedImage.setMask(circularMask.createMaskFromColor(Qt::transparent));

    QImage image = croppedImage.toImage();

    while (image.sizeInBytes() > 58 * 1024 && image.width() > 10 && image.height() > 10) {
        image = image.scaled(image.width() * 0.9, image.height() * 0.9,
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);

    if (!image.save(&buffer, "PNG")) {
        return;
    }

    if (imageData.size() > 58 * 1024) {
        return;
    }


    std::string fileName = m_config_manager->getMyLoginHash() + "myMainPhoto.dph";


    try {
        CryptoPP::SecByteBlock aesKey;
        utility::generateAESKey(aesKey);

        std::string encryptedImage = utility::AESEncrypt(aesKey,
            std::string(imageData.constData(), imageData.size()));

        std::string encryptedKey = utility::RSAEncryptKey(m_client->getPublicKey(), aesKey);

        std::string finalData = encryptedKey + "\n" + encryptedImage;


        std::string path = utility::getConfigsAndPhotosDirectory() + "/" + fileName;
        std::ofstream outFile(path, std::ios::binary);
        if (!outFile) {
            return;
        }

        outFile.write(finalData.data(), finalData.size());
        m_filePath = path;
    }
    catch (const std::exception& e) {
        return;
    }
    catch (...) {
        return;
    }
}
