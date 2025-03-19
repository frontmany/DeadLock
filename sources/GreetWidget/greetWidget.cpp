#include "greetWidget.h"
#include "mainwindow.h"
#include "client.h"
#include <QWheelEvent>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QPainterPath>

GreetWidget::GreetWidget(QWidget* parent, MainWindow* mw, Client* client, Theme theme)
    : QWidget(parent), m_client(client), style(new StyleGreetWidget()),
    m_cropX(0), m_cropY(0), m_cropWidth(100), m_cropHeight(100) {

    setBackGround(theme);

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignTop);

    QHBoxLayout* labelLayout = new QHBoxLayout();
    labelLayout->setAlignment(Qt::AlignCenter);
    m_welcomeLabel = new QLabel("Добро пожаловать!", this);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_welcomeLabel->setStyleSheet("font-size: 32px; font-weight: bold;");
    labelLayout->addSpacing(35);
    labelLayout->addWidget(m_welcomeLabel);

    m_imageLabel = new QLabel(this);
    m_imageLabel->setFixedSize(500, 500);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setPixmap(QPixmap(":/resources/GreetWidget/loadPhoto.png").scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_selectImageButton = new QPushButton("Выбрать фото", this);
    m_selectImageButton->setMinimumSize(200, 60);
    m_selectImageButton->setMaximumSize(200, 60);
    m_selectImageButton->setStyleSheet(style->DarkButtonStyle);
    connect(m_selectImageButton, &QPushButton::clicked, this, &GreetWidget::openImagePicker);

    m_continueButton = new QPushButton("Continue", this);
    m_continueButton->setStyleSheet(style->DarkButtonStyle);
    m_continueButton->setEnabled(false);
    m_continueButton->setMinimumSize(200, 60);
    m_continueButton->setMaximumSize(350, 60);
    connect(m_continueButton, &QPushButton::clicked, this, []() {
        // Действие при нажатии на кнопку continue
        });

    QHBoxLayout* sliderXLayout = new QHBoxLayout();
    sliderXLayout->setAlignment(Qt::AlignCenter);
    m_cropXSlider = new QSlider(Qt::Horizontal, this);
    m_cropXSlider->setStyleSheet(style->DarkSliderStyle);
    m_cropXSlider->setFixedSize(400, 20);
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

        // Активируем кнопку continue
        m_continueButton->setEnabled(true);

        // Обрезаем изображение до круга (если это необходимо)
        cropImageToCircle();
    }
}

void GreetWidget::cropImageToCircle() {
    if (m_selectedImage.isNull()) return;

    // Создание круглой маски (только контур)
    QPixmap circularImage(m_cropWidth, m_cropHeight);
    circularImage.fill(Qt::transparent);

    QPainter circularPainter(&circularImage);
    circularPainter.setRenderHint(QPainter::Antialiasing);
    circularPainter.setPen(QPen(Qt::white, 3)); // Красный контур толщиной 2 пикселя
    circularPainter.setBrush(Qt::NoBrush); // Без заливки
    circularPainter.drawEllipse(0, 0, m_cropWidth, m_cropHeight);
    circularPainter.end(); // Завершаем работу с circularPainter

    // Создание фона с изображением
    QPixmap background(m_imageLabel->size());
    background.fill(Qt::transparent);

    QPainter backgroundPainter(&background);
    backgroundPainter.setRenderHint(QPainter::Antialiasing);
    backgroundPainter.drawPixmap(0, 0, m_selectedImage.scaled(m_imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // Наложение контура маски на изображение с учетом m_cropX и m_cropY
    int maskX = m_cropX; // Позиция маски по X
    int maskY = m_cropY; // Позиция маски по Y
    backgroundPainter.drawPixmap(maskX, maskY, circularImage);
    backgroundPainter.end(); // Завершаем работу с backgroundPainter

    m_imageLabel->setPixmap(background); // Устанавливаем изображение с контуром маски
}

void GreetWidget::adjustCropArea(int value) {
    // Получаем размеры изображения
    int imageWidth = m_imageLabel->width();
    int imageHeight = m_imageLabel->height();

    // Ограничиваем размер маски, чтобы она не превышала размеры изображения
    m_cropWidth = qMin(m_cropWidth, imageWidth);
    m_cropHeight = qMin(m_cropHeight, imageHeight);

    // Ограничиваем перемещение маски в пределах изображения
    int maxX = imageWidth - m_cropWidth;
    int maxY = imageHeight - m_cropHeight;

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
    int newSize = m_cropWidth + (delta > 0 ? 10 : -10); // Увеличиваем или уменьшаем размер

    // Ограничиваем минимальный и максимальный размер маски
    newSize = qBound(50, newSize, 500);

    // Обновляем размер маски
    m_cropWidth = newSize;
    m_cropHeight = newSize;

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