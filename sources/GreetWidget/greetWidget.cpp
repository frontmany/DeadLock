#include "greetWidget.h"
#include "mainwindow.h"
#include "client.h"
#include "utility.h"
#include "chatsWidget.h"
#include "chatsListComponent.h"
#include <QWheelEvent>
#include <QPainter>
#include <QGraphicsBlurEffect>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QPainterPath>
#include <QBuffer>
#include <QApplication>
#include <QStyle>

GreetWidget::GreetWidget(QWidget* parent, MainWindow* mw, Client* client, Theme theme, std::string login, ChatsWidget* cv)
    : QWidget(parent), m_client(client), m_style(new StyleGreetWidget()),
    m_cropX(0), m_cropY(0), m_cropWidth(100), m_cropHeight(100), m_mainWindow(mw), m_chatsWidget(cv) {

    setBackGround(theme);

    m_sender = new SendStringsGenerator;

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setAlignment(Qt::AlignCenter);

    QString welcomeStr = "Welcome " + QString::fromStdString(login) + "!";
    m_welcomeLabel = new QLabel(welcomeStr, this);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    m_welcomeLabel->setStyleSheet("font-size: 42px; font-weight: bold;");

    m_greetLabelLayout = new QHBoxLayout();
    m_greetLabelLayout->setAlignment(Qt::AlignCenter);
    m_greetLabelLayout->addSpacing(35);
    m_greetLabelLayout->addWidget(m_welcomeLabel);

    m_skipButton = new QPushButton("skip", this);
    m_skipButton->setStyleSheet(m_style->buttonSkipStyle);
    m_skipButton->setMinimumSize(100, 60);
    m_skipButton->setMaximumSize(350, 60);
    connect(m_skipButton, &QPushButton::clicked, this, [this]() {
        m_mainWindow->setupChatsWidget();
        });

    m_imageLabel = new QLabel(this);
    m_imageLabel->setFixedSize(500, 500);
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setPixmap(QPixmap(":/resources/GreetWidget/loadPhoto.png").scaled(500, 500, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_selectImageButton = new QPushButton("Choose a photo", this);
    m_selectImageButton->setMinimumSize(200, 60);
    m_selectImageButton->setMaximumSize(200, 60);
    m_selectImageButton->setStyleSheet(m_style->DarkButtonStyle);
    connect(m_selectImageButton, &QPushButton::clicked, this, &GreetWidget::openImagePicker);

    m_continueButton = new QPushButton("Continue", this);
    m_continueButton->setStyleSheet(m_style->DarkButtonStyle);
    m_continueButton->setEnabled(false);
    m_continueButton->setMinimumSize(200, 60);
    m_continueButton->setMaximumSize(350, 60);
    connect(m_continueButton, &QPushButton::clicked, this, [this]() {
        int res = saveCroppedImage();
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

            // Добавляем кнопку
            QPushButton* okButton = new QPushButton("OK", errorDialog);
            okButton->setFixedHeight(30);
            okButton->setFixedWidth(140);
            okButton->setStyleSheet(m_style->buttonStyleGray);
            connect(okButton, &QPushButton::clicked, errorDialog, &QDialog::accept);

            // Компоновка
            layout->addWidget(textLabel);

            QHBoxLayout* hla = new QHBoxLayout;
            hla->addWidget(okButton);
            hla->setAlignment(Qt::AlignCenter);

            layout->addLayout(hla);

            errorDialog->exec();  // Показываем как модальное окно
        }
        else {
            m_mainWindow->setupChatsWidget();
            Photo photo(m_filePath.toStdString());
            m_client->setPhoto(photo);
            auto chatsList = m_chatsWidget->getChatsList();
            chatsList->SetAvatar(photo);

            auto& map = m_client->getMyChatsMap();
            std::vector<std::string> logins;
            logins.reserve(map.size());
            for (auto [login, Chat] : map) {
                logins.emplace_back(login);
            }
            for (auto login : m_client->getVecToSendStatusTmp()) {
                logins.emplace_back(login);
            }
            m_client->setIsHasPhoto(true);
            m_client->sendPacket(m_sender->get_updateMyInfo_QueryStr(m_client->getMyLogin(), m_client->getMyName(), m_client->getPassword(), true, photo, logins));
        }
       
        });

    m_cropXSlider = new QSlider(Qt::Horizontal, this);
    m_cropXSlider->setStyleSheet(m_style->DarkSliderStyle);
    m_cropXSlider->setFixedSize(400, 20);
    m_cropXSlider->hide();
    m_cropXSlider->setRange(0, 500);
    m_cropXSlider->setValue(m_cropX);
    connect(m_cropXSlider, &QSlider::valueChanged, this, &GreetWidget::adjustCropArea);

    m_sliderXLayout = new QHBoxLayout();
    m_sliderXLayout->setAlignment(Qt::AlignCenter);
    m_sliderXLayout->addSpacing(40);
    m_sliderXLayout->addWidget(m_cropXSlider);


    m_cropYSlider = new QSlider(Qt::Vertical, this);
    m_cropYSlider->setRange(0, 330);
    m_cropYSlider->setStyleSheet(m_style->DarkSliderStyle);
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
    m_buttonsHLayout->addWidget(m_skipButton);
    m_buttonsHLayout->addSpacing(-140);
    
    m_imageAndYSliderLayout = new QHBoxLayout();
    m_imageAndYSliderLayout->setAlignment(Qt::AlignCenter);
    m_imageAndYSliderLayout->addWidget(m_cropYSlider);
    m_imageAndYSliderLayout->addSpacing(20);
    m_imageAndYSliderLayout->addWidget(m_imageLabel);

    m_bothSlidersVLayout = new QVBoxLayout();
    m_bothSlidersVLayout->setAlignment(Qt::AlignTop);
    m_bothSlidersVLayout->addLayout(m_imageAndYSliderLayout);
    m_bothSlidersVLayout->addLayout(m_sliderXLayout);

    m_photoAndSlidersWidgetContainer = new QWidget;
    m_photoAndSlidersWidgetContainer->setLayout(m_bothSlidersVLayout);
    m_photoAndSlidersWidgetContainer->setFixedHeight(550);

    // Добавление виджетов в основной layout
    m_mainVLayout->addSpacing(85);
    m_mainVLayout->addLayout(m_greetLabelLayout);
    m_mainVLayout->addSpacing(25);
    m_mainVLayout->addWidget(m_photoAndSlidersWidgetContainer);
    m_mainVLayout->addSpacing(25);
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

void GreetWidget::setName(const std::string& name) {
    QString s = "Welcome " + QString::fromStdString(name) + "!";
    m_welcomeLabel->setText(s);
}

void GreetWidget::setLogin(const std::string& login) {
    m_login = login;
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

int GreetWidget::saveCroppedImage() {
    if (m_selectedImage.isNull()) {
        qWarning() << "Нет изображения для сохранения";
        return 1;
    }

    // Создаем круглую маску
    QPixmap circularMask(m_cropSize, m_cropSize);
    circularMask.fill(Qt::transparent);
    QPainter maskPainter(&circularMask);
    maskPainter.setRenderHint(QPainter::Antialiasing);
    maskPainter.setBrush(Qt::white);
    maskPainter.setPen(Qt::NoPen);
    maskPainter.drawEllipse(0, 0, m_cropSize, m_cropSize);
    maskPainter.end();

    // Обрезаем изображение по маске
    QPixmap croppedImage = m_selectedImage.copy(m_cropX, m_cropY, m_cropSize, m_cropSize);
    croppedImage.setMask(circularMask.createMaskFromColor(Qt::transparent));

    // Получаем путь для сохранения
    QString saveDir = Utility::getSaveDir();
    if (saveDir.isEmpty()) {
        qWarning() << "Не удалось получить директорию для сохранения.";
        return 1;
    }

    QString fileName = QString::fromStdString(m_login) + "myMainPhoto.png";
    m_filePath = QDir(saveDir).filePath(fileName);

    // Конвертируем в QImage для обработки
    QImage image = croppedImage.toImage();

    // Уменьшаем размер изображения, если оно слишком большое
    while (image.sizeInBytes() > 64 * 1024 && image.width() > 10 && image.height() > 10) {
        image = image.scaled(image.width() * 0.9, image.height() * 0.9,
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // Сохраняем изображение с заданным качеством
    QByteArray imageData;
    QBuffer buffer(&imageData);
    buffer.open(QIODevice::WriteOnly);

    // Устанавливаем начальное качество
    int quality = 90; // Начальное качество 90
    if (!image.save(&buffer, "PNG", quality)) {
        qWarning() << "Ошибка при сохранении изображения в буфер";
        return 1;
    }

    // Если размер больше 64 КБ, уменьшаем качество
    while (imageData.size() > 64 * 1024 && quality > 0) {
        imageData = QByteArray();
        if (!image.save(&buffer, "PNG", quality)) {
            qWarning() << "Ошибка при сохранении изображения в буфер";
            return 1;
        }
        quality -= 5; // Уменьшаем качество на 5
    }

    // Проверяем, удалось ли сжать
    if (imageData.size() > 64 * 1024) {
        qWarning() << "Не удалось сжать изображение до 64 КБ. Фактический размер:"
            << imageData.size() / 1024 << "КБ";
        return 1;
    }

    // Сохраняем в файл
    QFile file(m_filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(imageData);
        qDebug() << "Изображение успешно сохранено:" << m_filePath
            << "Размер:" << imageData.size() / 1024 << "КБ"
            << "Качество:" << quality + 5; // +5 потому что последняя итерация уменьшила quality
        file.close();
        return 0;
    }
    else {
        qWarning() << "Ошибка при открытии файла для записи:" << file.errorString();
        return 1;
    }
}

