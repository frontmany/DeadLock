#include "theme.h"
#include "greetWidget.h"
#include "mainwindow.h"
#include "client.h"
#include "utility.h"
#include "avatar.h"
#include "chatsWidget.h"
#include "packetsBuilder.h"
#include "configManager.h"
#include "chatsListComponent.h"

StyleGreetWidget::StyleGreetWidget() {
    DarkButtonSkipStyle = R"(
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

    LightButtonSkipStyle = R"(
        QPushButton {
            background-color: transparent;     
            color: rgb(229, 229, 229);              
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

    DarkButtonStyle = R"(
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

    LightButtonStyle = R"(
        QPushButton {
            background-color: rgb(26, 133, 255);   
            color: white;             
            border: none;   
            border-radius: 28px;       
            padding: 5px 10px;        
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
            background-color: rgb(77, 77, 77); 
            height: 8px; 
            border-radius: 4px;
        }

        QSlider::handle:horizontal {
            background-color: white;
            width: 16px;
            height: 16px; 
            border-radius: 8px;
            margin: -4px 0;
        }

        QSlider::add-page:horizontal {
            background-color: rgb(77, 77, 77);
            border-radius: 4px;
        }

        QSlider::sub-page:horizontal {
            background-color: rgb(21, 119, 232);
            border-radius: 4px;
        }

        QSlider::groove:vertical {
            background-color: rgb(77, 77, 77);
            width: 8px;
            border-radius: 4px;
        }

        QSlider::handle:vertical {
            background-color: white;
            width: 16px;
            height: 16px;
            border-radius: 8px; 
            margin: 0 -4px;
        }

        QSlider::add-page:vertical {
            background-color: rgb(77, 77, 77);
            border-radius: 4px;
        }

        QSlider::sub-page:vertical {
            background-color: rgb(21, 119, 232);
            border-radius: 4px;
        }
        )";


    LightSliderStyle = R"(
        QSlider::groove:horizontal {
            background-color: rgb(230, 230, 230);
            height: 8px;
            border-radius: 4px;
        }

        QSlider::handle:horizontal {
            background-color: white;
            width: 16px;
            height: 16px;
            border-radius: 8px;
            margin: -4px 0;
        }

        QSlider::add-page:horizontal {
            background-color: rgb(230, 230, 230);
            border-radius: 4px;
        }

        QSlider::sub-page:horizontal {
            background-color: rgb(21, 119, 232);
            border-radius: 4px;
        }

        QSlider::groove:vertical {
            background-color: rgb(230, 230, 230);
            width: 8px;
            border-radius: 4px;
        }

        QSlider::handle:vertical {
            background-color: white;
            width: 16px;
            height: 16px;
            border-radius: 8px;
            margin: 0 -4px;
        }

        QSlider::add-page:vertical {
            background-color: rgb(230, 230, 230);
            border-radius: 4px;
        }

        QSlider::sub-page:vertical {
            background-color: rgb(21, 119, 232);
            border-radius: 4px; 
        }
        )";


    DarkHintStyle = R"(
    QLabel {
        background-color: rgb(90, 90, 90);
        color: rgb(240, 240, 240);
        border-radius: 8px;
        padding: 12px;
        font-weight: 650;
        font-family: 'Segoe UI';
        font-size: 14px;
        margin: 5px;
    }
)";

    LightHintStyle = R"(
    QLabel {
        background-color: rgb(245, 245, 245);
        color: rgb(60, 60, 60);
        border-radius: 8px;
        padding: 12px;
        font-weight: 650;
        font-family: 'Segoe UI';
        font-size: 14px;
        margin: 5px;
    }
)";

    WhiteLabelWelcomeStyle = "font-size: 42px; font-weight: bold; color: white;";
};

using namespace utility;

void GreetWidget::setTheme(Theme theme) {
    if (m_theme == Theme::DARK) {
        m_hintLabel->setStyleSheet(m_style->DarkHintStyle);
        m_welcomeLabel->setStyleSheet(m_style->WhiteLabelWelcomeStyle);
        m_skipButton->setStyleSheet(m_style->DarkButtonSkipStyle);

        m_selectImageButton->setStyleSheet(m_style->DarkButtonStyle);
        m_continueButton->setStyleSheet(m_style->DarkButtonStyle);

        m_cropXSlider->setStyleSheet(m_style->DarkSliderStyle);
        m_cropYSlider->setStyleSheet(m_style->DarkSliderStyle);
    }
    else {
        m_hintLabel->setStyleSheet(m_style->LightHintStyle);
        m_welcomeLabel->setStyleSheet(m_style->WhiteLabelWelcomeStyle);
        m_skipButton->setStyleSheet(m_style->LightButtonSkipStyle);

        m_selectImageButton->setStyleSheet(m_style->LightButtonStyle);
        m_continueButton->setStyleSheet(m_style->LightButtonStyle);

        m_cropXSlider->setStyleSheet(m_style->LightSliderStyle);
        m_cropYSlider->setStyleSheet(m_style->LightSliderStyle);
    }
}

void GreetWidget::setWelcomeLabelText(const std::string& text) {
    QString s = "Welcome " + QString::fromStdString(text) + "!";
    m_welcomeLabel->setText(s);
}

GreetWidget::GreetWidget(QWidget* parent, MainWindow* mw, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme, std::string login, CryptoPP::RSA::PublicKey publicKey, CryptoPP::RSA::PrivateKey privateKey, ChatsWidget* cv)
    : QWidget(parent), m_client(client), m_style(new StyleGreetWidget()),
    m_cropX(0), m_cropY(0), m_cropWidth(100), m_cropHeight(100), m_mainWindow(mw), m_chatsWidget(cv), m_config_manager(configManager), m_theme(theme), m_login(login), m_public_key(publicKey), m_private_key(privateKey) {

    setBackGround(m_theme);

    qreal scale = getDeviceScaleFactor();
    m_style->DarkButtonStyle = R"(
    QPushButton {
        background-color: rgb(21, 119, 232);   
        color: white;             
        border: none;   
        border-radius: )" + QString::number(28 / scale) + R"(px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
    )";

    m_style->LightButtonStyle = R"(
    QPushButton {
        background-color: rgb(26, 133, 255);   
        color: white;             
        border: none;   
        border-radius: )" + QString::number(28 / scale) + R"(px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(21, 119, 232);   
    }
    QPushButton:pressed {
        background-color: rgb(21, 119, 232);      
    }
    )";

    spacer = new QSpacerItem(80, 10);
    m_sender = new PacketsBuilder();

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignCenter);
    m_mainVLayout->setContentsMargins(getScaledSize(20), getScaledSize(20), getScaledSize(20), getScaledSize(20));

    QString welcomeStr = "Welcome " + QString::fromStdString(login) + "!";
    m_welcomeLabel = new QLabel(welcomeStr, this);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont welcomeFont = m_welcomeLabel->font();
    welcomeFont.setPixelSize(getScaledSize(16));
    m_welcomeLabel->setFont(welcomeFont);

    m_greetLabelLayout = new QHBoxLayout();
    m_greetLabelLayout->setContentsMargins(0, getScaledSize(20), 0, getScaledSize(10));
    m_greetLabelLayout->setAlignment(Qt::AlignCenter);
    m_greetLabelLayout->addSpacing(getScaledSize(35));
    m_greetLabelLayout->addWidget(m_welcomeLabel);

    m_skipButton = new QPushButton("skip", this);
    m_skipButton->setMinimumSize(getScaledSize(100), getScaledSize(60));
    m_skipButton->setMaximumSize(getScaledSize(350), getScaledSize(60));
    connect(m_skipButton, &QPushButton::clicked, this, [this]() {
        m_mainWindow->setupChatsWidget();
        m_hintLabel->hide();
        if (m_spacerAdded) {
            m_buttonsHLayout->removeItem(spacer);
            m_spacerAdded = false;
        }
    });

    m_imageLabel = new QLabel(this);
    m_imageLabel->setFixedSize(getScaledSize(500), getScaledSize(500));
    m_imageLabel->setAlignment(Qt::AlignCenter);

    QPixmap pixmap;
    if (m_theme == DARK){
        pixmap = QPixmap(":/resources/GreetWidget/loadPhotoDark.png");
    }
    else {
        pixmap = QPixmap(":/resources/GreetWidget/loadPhoto.png");
    }
    pixmap.setDevicePixelRatio(devicePixelRatioF());
    m_imageLabel->setPixmap(pixmap.scaled(
        m_imageLabel->size() * devicePixelRatioF(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    ));

    m_selectImageButton = new QPushButton("Choose a photo", this);
    m_selectImageButton->setMinimumSize(getScaledSize(200), getScaledSize(60));
    m_selectImageButton->setMaximumSize(getScaledSize(200), getScaledSize(60));
    connect(m_selectImageButton, &QPushButton::clicked, this, &GreetWidget::openImagePicker);

    m_continueButton = new QPushButton("Continue", this);
    m_continueButton->setEnabled(false);
    m_continueButton->setMinimumSize(getScaledSize(200), getScaledSize(60));
    m_continueButton->setMaximumSize(getScaledSize(350), getScaledSize(60));

    connect(m_continueButton, &QPushButton::clicked, this, [this]() {
        int res = saveImage();

        if (res == 1) {
            QDialog* errorDialog = new QDialog(this);
            errorDialog->setWindowFlags(Qt::Dialog);
            errorDialog->setFixedHeight(85);
            errorDialog->setFixedWidth(140);
            errorDialog->setWindowTitle("We are sorry :( ");
            errorDialog->setFixedSize(300, 150);

            QVBoxLayout* layout = new QVBoxLayout(errorDialog);


            QLabel* textLabel = new QLabel("Couldn't save image. Try to choose a smaller photo or reduce the circle.", errorDialog);
            textLabel->setWordWrap(true);
            textLabel->setFixedHeight(40);
            textLabel->setStyleSheet(
                "QLabel {"
                "   font-family: 'Segoe UI';"
                "   font-size: 12pt;"
                "   color: white;"
                "   background: transparent;"
                "}"
            );
            textLabel->setAlignment(Qt::AlignCenter);

            QPushButton* okButton = new QPushButton("OK", errorDialog);
            okButton->setFixedHeight(30);
            okButton->setFixedWidth(140);

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

            okButton->setStyleSheet(buttonStyleGray);
            connect(okButton, &QPushButton::clicked, errorDialog, &QDialog::accept);

            layout->addWidget(textLabel);

            QHBoxLayout* hla = new QHBoxLayout;
            hla->addWidget(okButton);
            hla->setAlignment(Qt::AlignCenter);

            layout->addLayout(hla);

            errorDialog->exec();
        }
        else {
            Avatar* avatar = new Avatar(m_client->getAvatarsKey(), m_filePath);
            m_config_manager->setAvatar(avatar);
            m_config_manager->setIsHasAvatar(true);
            m_client->updateMyAvatar(avatar);
            m_mainWindow->setupChatsWidget();
        }
    });

    m_cropXSlider = new QSlider(Qt::Horizontal, this);
    m_cropXSlider->setFixedSize(getScaledSize(400), getScaledSize(20));
    m_cropXSlider->hide();
    m_cropXSlider->setRange(0, 500);
    m_cropXSlider->setValue(m_cropX);
    connect(m_cropXSlider, &QSlider::valueChanged, this, &GreetWidget::adjustCropArea);

    m_sliderXLayout = new QHBoxLayout();
    m_sliderXLayout->setAlignment(Qt::AlignCenter);
    m_sliderXLayout->addSpacing(getScaledSize(50));
    m_sliderXLayout->addWidget(m_cropXSlider);

    m_cropYSlider = new QSlider(Qt::Vertical, this);
    m_cropYSlider->setFixedSize(getScaledSize(20), getScaledSize(400));
    m_cropYSlider->hide();
    m_cropYSlider->setRange(0, 500);
    m_cropYSlider->setValue(m_cropY);
    m_cropYSlider->setInvertedAppearance(true);
    connect(m_cropYSlider, &QSlider::valueChanged, this, &GreetWidget::adjustCropArea);

    m_hintLabel = new QLabel("Use mouse\nwheel", this);
    m_hintLabel->setAlignment(Qt::AlignCenter);
    m_hintLabel->setFixedSize(utility::getScaledSize(120), 90);
    m_hintLabel->setWordWrap(true);
    m_hintLabel->hide();

    m_buttonsHLayout = new QHBoxLayout();
    m_buttonsHLayout->setAlignment(Qt::AlignCenter);
    m_buttonsHLayout->addWidget(m_hintLabel);
    m_buttonsHLayout->addSpacing(utility::getScaledSize(20));
    m_buttonsHLayout->addWidget(m_selectImageButton);
    m_buttonsHLayout->addSpacing(getScaledSize(35));
    m_buttonsHLayout->addWidget(m_continueButton);
    m_buttonsHLayout->addWidget(m_skipButton);
    m_buttonsHLayout->addSpacing(-80);

    m_imageAndYSliderLayout = new QHBoxLayout();
    m_imageAndYSliderLayout->setAlignment(Qt::AlignCenter);
    m_imageAndYSliderLayout->addWidget(m_cropYSlider);
    m_imageAndYSliderLayout->addSpacing(getScaledSize(20));
    m_imageAndYSliderLayout->addWidget(m_imageLabel);

    m_bothSlidersVLayout = new QVBoxLayout();
    m_bothSlidersVLayout->setAlignment(Qt::AlignTop);
    m_bothSlidersVLayout->addLayout(m_imageAndYSliderLayout);
    m_bothSlidersVLayout->addSpacing(getScaledSize(70));
    m_bothSlidersVLayout->addLayout(m_sliderXLayout);

    m_photoAndSlidersWidgetContainer = new QWidget;
    m_photoAndSlidersWidgetContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    m_photoAndSlidersWidgetContainer->setLayout(m_bothSlidersVLayout);
    m_photoAndSlidersWidgetContainer->setFixedHeight(getScaledSize(575));

    m_mainVLayout->addLayout(m_greetLabelLayout);
    m_mainVLayout->addWidget(m_photoAndSlidersWidgetContainer);
    m_mainVLayout->addLayout(m_buttonsHLayout);

    m_container = new QWidget;
    m_container->setLayout(m_mainVLayout);
    m_container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    m_containerLayout = new QHBoxLayout;
    m_containerLayout->setAlignment(Qt::AlignCenter);
    m_containerLayout->setContentsMargins(0, 0, 0, 0);
    m_containerLayout->addWidget(m_container);

    setLayout(m_containerLayout);
    setMouseTracking(true);

    setTheme(m_theme);
}

void GreetWidget::startWelcomeAnimation() {
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(this);
    m_welcomeLabel->setGraphicsEffect(effect);
    effect->setOpacity(0);

    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(3000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GreetWidget::openImagePicker() {
    QString imagePath = QFileDialog::getOpenFileName(this, "Choose Photo", "", "Images (*.png *.jpg *.jpeg)");
    if (!imagePath.isEmpty()) {
        m_selectedImage.load(imagePath);

        int paramScale = getScaledSize(500);
        m_selectedImage = m_selectedImage.scaled(paramScale, paramScale, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        int imageWidth = m_selectedImage.width();
        int imageHeight = m_selectedImage.height();

        if (imageWidth < paramScale || imageHeight < paramScale) {
            m_imageLabel->setFixedSize(imageWidth, imageHeight);
            m_cropXSlider->setFixedSize(imageWidth, getScaledSize(30));
            m_cropYSlider->setFixedSize(getScaledSize(30), imageHeight);
        }
        else {
            m_imageLabel->setFixedSize(getScaledSize(500), getScaledSize(500));
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

        
        if (m_spacerAdded) {
            m_buttonsHLayout->removeItem(spacer);
            m_spacerAdded = false;
        }
        m_buttonsHLayout->addSpacerItem(spacer);
        m_spacerAdded = true;

        m_cropXSlider->show();
        m_cropYSlider->show();
        m_hintLabel->show();
    }

    m_imageLabel->update();
    update();
}

void GreetWidget::setLogin(const std::string& login) {
    m_login = login;
}

void GreetWidget::cropImageToCircle() {
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

void GreetWidget::adjustCropArea(int value) {
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

void GreetWidget::wheelEvent(QWheelEvent* event) {
    if (m_selectedImage.isNull()) return;

    int delta = event->angleDelta().y();
    int newSize = m_cropSize + (delta > 0 ? 10 : -10);

    newSize = qBound(50, newSize, 500);
    m_cropSize = newSize;

    adjustCropArea(0); 
    cropImageToCircle();
}

void GreetWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);

    if (!m_background.isNull()) {
        painter.drawPixmap(rect(), m_background);
    }

    QPainterPath roundedRectPath;
    int cornerRadius = 0;
    QRect rect(0, 0, width(), height()); 

    roundedRectPath.addRoundedRect(rect, cornerRadius, cornerRadius);

    QGraphicsBlurEffect blurEffect;
    blurEffect.setBlurRadius(10);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter.save();
    painter.setClipPath(roundedRectPath);
    painter.setOpacity(0.7);

    if (m_theme == Theme::DARK) {
        painter.fillPath(roundedRectPath, QColor(26, 26, 26, 200)); 

    }
    else {
        painter.fillPath(roundedRectPath, QColor(150, 150, 150, 200));

    }
    painter.restore();
}

void GreetWidget::setBackGround(Theme theme) {
    if (theme == DARK) {
        if (m_background.load(":/resources/LoginWidget/darkLoginBackground.jpg")) {
        }
    }
    else {
        if (m_background.load(":/resources/LoginWidget/lightLoginBackground.jpg")) {
        }
    }
}
int GreetWidget::saveImage() {
    if (m_selectedImage.isNull()) {
        qWarning() << "There is no image to save";
        return 1;
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
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);

    if (!image.save(&buffer, "PNG")) {
        qWarning() << "Couldn't save image to buffer";
        return 1;
    }

    std::string fileName = m_config_manager->getMyLoginHash() + ".dph";
    m_filePath = utility::getConfigsAndPhotosDirectory() + "/" + fileName;

    CryptoPP::SecByteBlock key = m_client->getAvatarsKey();

    std::string encryptedImage;
    try {
        encryptedImage = utility::AESEncrypt(key, std::string(imageData.constData(), imageData.size()));
    }
    catch (const std::exception& e) {
        qWarning() << "Image encryption error:" << e.what();
        return 1;
    }

    std::ofstream outFile(m_filePath, std::ios::binary);
    if (!outFile) {
        qWarning() << "Couldn't open the file for writing:" << m_filePath.c_str();
        return 1;
    }

    outFile.write(encryptedImage.data(), encryptedImage.size());
    if (!outFile) {
        qWarning() << "Error writing to a file";
        return 1;
    }

    return 0;
}

