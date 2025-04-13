#include "photoEditComponent.h"
#include "mainwindow.h"
#include "fieldsEditComponent.h"
#include "profileEditorWidget.h"
#include "client.h"
#include "utility.h"
#include "photo.h"
#include "chatsWidget.h"
#include "chatsListComponent.h"


PhotoEditComponent::PhotoEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, Theme theme)
    : QWidget(parent), m_profile_editor_widget(profileEditorWidget), m_client(client), m_theme(theme),
    m_cropX(0), m_cropY(0), m_cropWidth(100), m_cropHeight(100) {
    m_style = new StylePhotoEditComponent;
    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);
    
    m_cancelButton = new QPushButton("cancel", this);
    m_cancelButton->setMinimumSize(100, 60);
    m_cancelButton->setMaximumSize(150, 60);
    connect(m_cancelButton, &QPushButton::clicked, [this]() {
        m_profile_editor_widget->setFieldsEditor();


        m_imageLabel->setPixmap(QPixmap(":/resources/GreetWidget/loadPhoto.png").scaled(450, 450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        

        m_imageLabel->setFixedSize(450, 450);
        m_cropXSlider->hide();
        m_cropYSlider->hide();
        setFixedHeight(650);
        });

    m_imageLabel = new QLabel(this);
    m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_imageLabel->setAlignment(Qt::AlignCenter);


    m_imageLabel->setPixmap(QPixmap(":/resources/GreetWidget/loadPhoto.png").scaled(450, 450, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    


    m_selectImageButton = new QPushButton("Choose a photo", this);
    m_selectImageButton->setMinimumSize(200, 60);
    m_selectImageButton->setMaximumSize(250, 60);
    connect(m_selectImageButton, &QPushButton::clicked, this, &PhotoEditComponent::openImagePicker);

    m_continueButton = new QPushButton("Save", this);
    m_continueButton->setEnabled(false);
    m_continueButton->setMinimumSize(200, 60);
    m_continueButton->setMaximumSize(250, 60);
    connect(m_continueButton, &QPushButton::clicked, [this]() {
        saveCroppedImage();
        Photo* photo = new Photo(m_filePath.toStdString());


        m_profile_editor_widget->setFieldsEditor();
        m_profile_editor_widget->updateAvatar(*photo);

 
        m_client->setPhoto(photo);
        m_client->setIsHasPhoto(true);

        m_client->updateMyPhoto(*photo);
        m_profile_editor_widget->setFieldsEditor();
        });

    m_cropXSlider = new QSlider(Qt::Horizontal, this);
    m_cropXSlider->setFixedSize(200, 20);
    m_cropXSlider->hide();
    m_cropXSlider->setRange(0, 500);
    m_cropXSlider->setValue(m_cropX);
    connect(m_cropXSlider, &QSlider::valueChanged, this, &PhotoEditComponent::adjustCropArea);

    m_sliderXLayout = new QHBoxLayout();
    m_sliderXLayout->setAlignment(Qt::AlignCenter);
    m_sliderXLayout->addSpacing(40);
    m_sliderXLayout->addWidget(m_cropXSlider);


    m_cropYSlider = new QSlider(Qt::Vertical, this);
    m_cropYSlider->setRange(0, 330);
    m_cropYSlider->setInvertedAppearance(true);
    m_cropYSlider->setValue(m_cropY);
    m_cropYSlider->setFixedSize(20, 200);
    m_cropYSlider->hide();
    connect(m_cropYSlider, &QSlider::valueChanged, this, &PhotoEditComponent::adjustCropArea);

    m_buttonsHLayout = new QHBoxLayout();
    m_buttonsHLayout->setAlignment(Qt::AlignCenter);
    m_buttonsHLayout->addWidget(m_selectImageButton);
    m_buttonsHLayout->addSpacing(30);
    m_buttonsHLayout->addWidget(m_continueButton);
    m_buttonsHLayout->addWidget(m_cancelButton);
    m_buttonsHLayout->addSpacing(-110);

    m_imageAndYSliderLayout = new QHBoxLayout();
    m_imageAndYSliderLayout->setAlignment(Qt::AlignCenter);
    m_imageAndYSliderLayout->addWidget(m_cropYSlider);
    m_imageAndYSliderLayout->addSpacing(20);
    m_imageAndYSliderLayout->addWidget(m_imageLabel);

    m_bothSlidersVLayout = new QVBoxLayout();
    m_bothSlidersVLayout->setAlignment(Qt::AlignTop);
    m_bothSlidersVLayout->addLayout(m_imageAndYSliderLayout);
    m_bothSlidersVLayout->addSpacing(20);
    m_bothSlidersVLayout->addLayout(m_sliderXLayout);

    m_photoAndSlidersWidgetContainer = new QWidget;
    m_photoAndSlidersWidgetContainer->setMaximumSize(2000, 600);
    m_photoAndSlidersWidgetContainer->setLayout(m_bothSlidersVLayout);

    m_mainVLayout->addWidget(m_photoAndSlidersWidgetContainer);
    m_mainVLayout->addSpacing(20);
    m_mainVLayout->addLayout(m_buttonsHLayout);
    m_mainVLayout->addSpacing(30);
    setLayout(m_mainVLayout);
    setMouseTracking(true);

    setTheme(m_theme);
} 

void PhotoEditComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (theme == Theme::DARK) {
        m_cancelButton->setStyleSheet(m_style->buttonSkipStyleBothTheme);
        m_selectImageButton->setStyleSheet(m_style->DarkButtonStyleBlue);
        m_continueButton->setStyleSheet(m_style->DarkButtonStyleBlue);
        m_cropXSlider->setStyleSheet(m_style->DarkSliderStyle);
        m_cropYSlider->setStyleSheet(m_style->DarkSliderStyle);

    }
    if (theme == Theme::LIGHT) {
        m_cancelButton->setStyleSheet(m_style->buttonSkipStyleBothTheme);
        m_selectImageButton->setStyleSheet(m_style->LightButtonStyleBlue);
        m_continueButton->setStyleSheet(m_style->LightButtonStyleBlue);
        m_cropXSlider->setStyleSheet(m_style->LightSliderStyle);
        m_cropYSlider->setStyleSheet(m_style->LightSliderStyle);
    }
}

void PhotoEditComponent::openImagePicker() {
    setFixedHeight(700);
    m_profile_editor_widget->onImagePicker();

    QString imagePath = QFileDialog::getOpenFileName(this, "Выберите фото", "", "Images (*.png *.jpg *.jpeg)");
    if (!imagePath.isEmpty()) {
        m_selectedImage.load(imagePath);
        m_selectedImage = m_selectedImage.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int imageWidth = m_selectedImage.width();
        int imageHeight = m_selectedImage.height();

        if (imageWidth < 500 || imageHeight < 500) {
            m_imageLabel->setFixedSize(imageWidth, imageHeight);
            m_cropXSlider->setFixedSize(imageWidth, 30);
            m_cropYSlider->setFixedSize(30, imageHeight);
        }
        else {
            m_imageLabel->setFixedSize(400, 400);
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
        qWarning() << "No image to save";
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

    QString saveDir = QString::fromStdString(utility::getSaveDir());
    if (saveDir.isEmpty()) {
        qWarning() << "Couldn't get the directory to save.";
        return;
    }

    QString fileName = QString::fromStdString(m_client->getMyLogin()) + "myMainPhoto.png";
    m_filePath = QDir(saveDir).filePath(fileName);

    QImage image = croppedImage.toImage();

    // Уменьшаем размер изображения, если оно слишком большое
    while (image.sizeInBytes() > 58 * 1024 && image.width() > 10 && image.height() > 10) {
        image = image.scaled(image.width() * 0.9, image.height() * 0.9,
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);


    int quality = 60;
    if (!image.save(&buffer, "PNG", quality)) {
        qWarning() << "Error saving the image to the buffer";
        return;
    }

    // Проверяем, удалось ли сжать
    if (imageData.size() > 58 * 1024) {
        qWarning() << "Не удалось сжать изображение до 64 КБ. Фактический размер:"
            << imageData.size() / 1024 << "КБ";
        return;
    }

    // Сохраняем в файл
    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(imageData);
        qDebug() << "The image was saved successfully:" << m_filePath
            << "Size" << imageData.size() / 1024 << "КБ"
            << "Quality:" << quality + 5; // +5 потому что последняя итерация уменьшила quality
        file.close();
        return;
    }
    else {
        qWarning() << "Error when opening a file for writing:" << file.errorString();
        return;
    }
}
