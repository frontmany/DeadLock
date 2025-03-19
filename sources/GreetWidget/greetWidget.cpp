#include "greetWidget.h"
#include "mainwindow.h"
#include "client.h"
#include "utility.h"
#include <QWheelEvent>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainterPath>

GreetWidget::GreetWidget(QWidget* parent, MainWindow* mw, Client* client, Theme theme, std::string login)
    : QWidget(parent), m_client(client), style(new StyleGreetWidget()),
    m_cropX(0), m_cropY(0), m_cropWidth(100), m_cropHeight(100) {

    setBackGround(theme);

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setAlignment(Qt::AlignCenter);
    QString s = "Welcome " + QString::fromStdString(login) + "!";
    m_welcomeLabel = new QLabel(s, this);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_welcomeLabel->setStyleSheet("font-size: 42px; font-weight: bold;");
    labelLayout->addSpacing(35);
    labelLayout->addWidget(m_welcomeLabel);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setFixedSize(500, 500);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setPixmap(QPixmap(":/resources/GreetWidget/loadPhoto.png").scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_selectImageButton = new QPushButton("Choose a photo", this);
    m_selectImageButton->setMinimumSize(200, 60);
    m_selectImageButton->setMaximumSize(200, 60);
    m_selectImageButton->setStyleSheet(style->DarkButtonStyle);
    connect(m_selectImageButton, &QPushButton::clicked, this, &GreetWidget::openImagePicker);

    m_continueButton = new QPushButton("Continue", this);
    m_continueButton->setStyleSheet(style->DarkButtonStyle);
    m_continueButton->setEnabled(false);
    m_continueButton->setMinimumSize(200, 60);
    m_continueButton->setMaximumSize(350, 60);
    connect(m_continueButton, &QPushButton::clicked, this, [this]() {
        saveCroppedImage();
        //TODO

        });

    QHBoxLayout* sliderXLayout = new QHBoxLayout();
    sliderXLayout->setAlignment(Qt::AlignCenter);
    m_cropXSlider = new QSlider(Qt::Horizontal, this);
    m_cropXSlider->setStyleSheet(style->DarkSliderStyle);
    m_cropXSlider->setFixedSize(400, 20);
    m_cropXSlider->hide();
    m_cropXSlider->setRange(0, 500);
    m_cropXSlider->setValue(m_cropX);
    connect(m_cropXSlider, &QSlider::valueChanged, this, &GreetWidget::adjustCropArea);
    sliderXLayout->addSpacing(40);
    sliderXLayout->addWidget(m_cropXSlider);

    m_cropYSlider = new QSlider(Qt::Vertical, this);
    m_cropYSlider->setRange(0, 330);

    m_cropYSlider->setStyleSheet(style->DarkSliderStyle);
    m_cropYSlider->setInvertedAppearance(true);
    m_cropYSlider->setValue(m_cropY);
   
    m_cropYSlider->setFixedSize(20, 330);
    connect(m_cropYSlider, &QSlider::valueChanged, this, &GreetWidget::adjustCropArea);
    m_cropYSlider->hide();
    m_buttonsHLayout = new QHBoxLayout();
    m_buttonsHLayout->setAlignment(Qt::AlignCenter);
    m_buttonsHLayout->addWidget(m_selectImageButton);
    m_buttonsHLayout->addSpacing(60);
    m_buttonsHLayout->addWidget(m_continueButton);

    
    QHBoxLayout* imageAndSliderLayout = new QHBoxLayout();
    imageAndSliderLayout->setAlignment(Qt::AlignCenter);
    imageAndSliderLayout->addWidget(m_cropYSlider); 
    imageAndSliderLayout->addSpacing(20);
    imageAndSliderLayout->addWidget(m_imageLabel); 

    QVBoxLayout* compLayout = new QVBoxLayout();
    compLayout->setAlignment(Qt::AlignTop);
    compLayout->addLayout(imageAndSliderLayout);
    compLayout->addLayout(sliderXLayout);

    QWidget* widgetContainer = new QWidget;
    widgetContainer->setLayout(compLayout);
    widgetContainer->setFixedHeight(550);

    // Добавление виджетов в основной layout
    m_mainVLayout->addSpacing(85);
    m_mainVLayout->addLayout(labelLayout);
    m_mainVLayout->addWidget(widgetContainer);
    m_mainVLayout->addSpacing(50);
    m_mainVLayout->addLayout(m_buttonsHLayout);
    m_mainVLayout->addSpacing(150);

    setLayout(m_mainVLayout);
    setMouseTracking(true);
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
    QString imagePath = QFileDialog::getOpenFileName(this, "Выберите фото", "", "Images (*.png *.jpg *.jpeg)");
    if (!imagePath.isEmpty()) {
        // Загружаем изображение
        m_selectedImage.load(imagePath);

        // Масштабируем изображение до 500x500 с сохранением пропорций
        m_selectedImage = m_selectedImage.scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // Получаем размеры масштабированного изображения
        int imageWidth = m_selectedImage.width();
        int imageHeight = m_selectedImage.height();

        // Если изображение меньше 500x500, изменяем размер QLabel
        if (imageWidth < 500 || imageHeight < 500) {
            m_imageLabel->setFixedSize(imageWidth, imageHeight);
            m_cropXSlider->setFixedSize(imageWidth, 30);
            m_cropYSlider->setFixedSize(30, imageHeight);
        }
        else {
            // Иначе устанавливаем размер 500x500
            m_imageLabel->setFixedSize(500, 500);
        }

        // Устанавливаем масштабированное изображение в QLabel
        m_imageLabel->setPixmap(m_selectedImage);

        // Инициализация маски
        m_cropSize = qMin(imageWidth, imageHeight) / 2; // Начальный размер маски (половина меньшей стороны)
        m_cropX = (imageWidth - m_cropSize) / 2; // Центрируем маску по X
        m_cropY = (imageHeight - m_cropSize) / 2; // Центрируем маску по Y

        // Обновляем слайдеры
        m_cropXSlider->setRange(0, imageWidth - m_cropSize);
        m_cropYSlider->setRange(0, imageHeight - m_cropSize);
        m_cropXSlider->setValue(m_cropX);
        m_cropYSlider->setValue(m_cropY);

        // Активируем кнопку continue
        m_continueButton->setEnabled(true);

        // Показываем маску сразу после загрузки изображения
        cropImageToCircle();

        // Показываем слайдеры
        m_cropXSlider->show();
        m_cropYSlider->show();
    }
}

void GreetWidget::setName(std::string name) {
    QString s = "Welcome " + QString::fromStdString(name) + "!";
    m_welcomeLabel->setText(s);
}

void GreetWidget::cropImageToCircle() {
    if (m_selectedImage.isNull()) return;

    // Размеры изображения
    QSize imageSize = m_imageLabel->size();

    // Размытие изображения
    QPixmap blurredImage = m_selectedImage.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QGraphicsBlurEffect* blurEffect = new QGraphicsBlurEffect;
    blurEffect->setBlurRadius(10); // Устанавливаем радиус размытия
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

    // Затемнение размытого изображения (делаем его серым)
    QPixmap darkenedBlurredImage = QPixmap::fromImage(blurredImageResult);
    QPainter darkenPainter(&darkenedBlurredImage);
    darkenPainter.setCompositionMode(QPainter::CompositionMode_Multiply); // Режим наложения
    darkenPainter.fillRect(darkenedBlurredImage.rect(), QColor(100, 100, 100, 150)); // Серый цвет с прозрачностью
    darkenPainter.end();

    // Создание круглой маски с белой рамкой
    QPixmap circularMask(m_cropSize, m_cropSize); // Используем m_cropSize для ширины и высоты
    circularMask.fill(Qt::transparent);

    QPainter circularPainter(&circularMask);
    circularPainter.setRenderHint(QPainter::Antialiasing);

    // Рисуем белую рамку
    circularPainter.setPen(QPen(Qt::NoPen));
    circularPainter.setBrush(Qt::NoBrush); // Без заливки
    circularPainter.drawEllipse(1, 1, m_cropSize - 2, m_cropSize - 2); // Учитываем толщину рамки
    circularPainter.end();

    // Создание фона с размытым и затемненным изображением
    QPixmap background(imageSize);
    background.fill(Qt::transparent);

    QPainter backgroundPainter(&background);
    backgroundPainter.setRenderHint(QPainter::Antialiasing);

    // Рисуем размытое и затемненное изображение
    backgroundPainter.drawPixmap(0, 0, darkenedBlurredImage);

    // Рисуем исходное изображение внутри маски
    QPixmap originalImage = m_selectedImage.scaled(imageSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Создаем круглую область для исходного изображения
    QPixmap circularArea(m_cropSize, m_cropSize); // Используем m_cropSize для ширины и высоты
    circularArea.fill(Qt::transparent);

    QPainter circularAreaPainter(&circularArea);
    circularAreaPainter.setRenderHint(QPainter::Antialiasing);
    circularAreaPainter.setBrush(Qt::white); // Заливка белым цветом для маски
    circularAreaPainter.setPen(Qt::NoPen); // Без контура
    circularAreaPainter.drawEllipse(0, 0, m_cropSize, m_cropSize);
    circularAreaPainter.end();

    // Наложение круглой области на исходное изображение
    QPixmap maskedOriginalImage = originalImage.copy(m_cropX, m_cropY, m_cropSize, m_cropSize);
    maskedOriginalImage.setMask(circularArea.createMaskFromColor(Qt::transparent));

    // Рисуем исходное изображение внутри маски
    backgroundPainter.drawPixmap(m_cropX, m_cropY, maskedOriginalImage);

    // Рисуем белую рамку поверх
    backgroundPainter.drawPixmap(m_cropX, m_cropY, circularMask);

    backgroundPainter.end();

    // Устанавливаем изображение с размытием, затемнением, круглой маской и белой рамкой
    m_imageLabel->setPixmap(background);
}

void GreetWidget::adjustCropArea(int value) {
    // Получаем размеры изображения
    int imageWidth = m_imageLabel->width();
    int imageHeight = m_imageLabel->height();

    // Ограничиваем размер маски, чтобы она не превышала размеры изображения
    m_cropSize = qMin(m_cropSize, qMin(imageWidth, imageHeight));

    // Ограничиваем перемещение маски в пределах изображения
    int maxX = imageWidth - m_cropSize;
    int maxY = imageHeight - m_cropSize;

    // Ограничиваем значения m_cropX и m_cropY
    m_cropX = qBound(0, m_cropXSlider->value(), maxX);
    m_cropY = qBound(0, m_cropYSlider->value(), maxY);

    if (!m_selectedImage.isNull()) {
        cropImageToCircle(); // Перерисовываем изображение с контуром маски
    }
}

void GreetWidget::wheelEvent(QWheelEvent* event) {
    if (m_selectedImage.isNull()) return;

    // Изменение размера маски с помощью колесика мыши
    int delta = event->angleDelta().y(); // Получаем значение прокрутки
    int newSize = m_cropSize + (delta > 0 ? 10 : -10); // Увеличиваем или уменьшаем размер

    // Ограничиваем минимальный и максимальный размер маски
    newSize = qBound(50, newSize, 500);

    // Обновляем размер маски
    m_cropSize = newSize;

    // Ограничиваем положение маски, чтобы она не выходила за пределы изображения
    adjustCropArea(0); // Пересчитываем положение маски

    // Перерисовываем изображение
    cropImageToCircle();
}

void GreetWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);

    QPainter painter(this);

    // 1. Рисуем фоновое изображение на весь экран
    if (!m_background.isNull()) {
        painter.drawPixmap(rect(), m_background);
    }

    // 2. Рисуем размытый скруглённый прямоугольник
    QPainterPath roundedRectPath;
    int cornerRadius = 0; // Радиус скругления углов
    QRect rect(0, 0, width(), height()); // Прямоугольник с отступами 50 пикселей от краёв

    // Создаём скруглённый прямоугольник
    roundedRectPath.addRoundedRect(rect, cornerRadius, cornerRadius);

    // Применяем размытие
    QGraphicsBlurEffect blurEffect;
    blurEffect.setBlurRadius(10); // Уровень размытия
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Рисуем скруглённый прямоугольник с размытием
    painter.save();
    painter.setClipPath(roundedRectPath);
    painter.setOpacity(0.7); // Прозрачность
    painter.fillPath(roundedRectPath, QColor(26, 26, 26, 200)); // Цвет прямоугольника
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

void GreetWidget::saveCroppedImage() {
    if (m_selectedImage.isNull()) return;

    // Создаем круглую маску
    QPixmap circularMask(m_cropSize, m_cropSize);
    circularMask.fill(Qt::transparent);

    QPainter maskPainter(&circularMask);
    maskPainter.setRenderHint(QPainter::Antialiasing);
    maskPainter.setBrush(Qt::white); // Заливка белым цветом для маски
    maskPainter.setPen(Qt::NoPen); // Без контура
    maskPainter.drawEllipse(0, 0, m_cropSize, m_cropSize);
    maskPainter.end();

    // Обрезаем изображение по маске
    QPixmap croppedImage = m_selectedImage.copy(m_cropX, m_cropY, m_cropSize, m_cropSize);
    croppedImage.setMask(circularMask.createMaskFromColor(Qt::transparent));

    // Получаем путь для сохранения
    QString saveDir = Utility::getSaveDir();
    if (saveDir.isEmpty()) {
        qWarning() << "Не удалось получить директорию для сохранения.";
        return;
    }

    // Генерируем имя файла
    QString fileName = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".png";
    QString filePath = QDir(saveDir).filePath(fileName);

    // Сохраняем обрезанное изображение
    if (croppedImage.save(filePath, "PNG")) {
        qDebug() << "Изображение успешно сохранено:" << filePath;
    }
    else {
        qWarning() << "Не удалось сохранить изображение.";
    }
}