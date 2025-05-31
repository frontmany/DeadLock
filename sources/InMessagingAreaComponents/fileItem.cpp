#include <filesystem>

#include "fileItem.h"
#include "filesComponent.h"


StyleFileItem::StyleFileItem() {
    darkThemeStyle = R"(
        QWidget {
            background-color: rgb(60, 60, 60);
            border-radius: 5px;
        }
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;
            color: #FFFFFF;
            font-size: 12px;
            padding: 2px;
        }
    )";

    lightThemeStyle = R"(
        QWidget {
            background-color: rgb(230, 230, 230);
            border-radius: 5px;
        }
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;
            color: #000000;
            font-size: 12px;
            padding: 2px;
        }
    )";

    highlightedDarkStyle = R"(
        QWidget {
            background-color: rgb(80, 80, 120);
            border-radius: 5px;
        }
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;
            color: #FFFFFF;
            font-size: 12px;
            padding: 2px;
        }
    )";

    highlightedLightStyle = R"(
        QWidget {
            background-color: rgb(200, 220, 255);
            border-radius: 5px;
        }
        QLabel {
            font-family: "Segoe UI";
            background-color: transparent;
            color: #000000;
            font-size: 12px;
            padding: 2px;
        }
    )";

    iconButtonStyle = R"(
        QPushButton {
            background: transparent;
            border: none;
            padding: 2px;
        }
    )";
}


FileItem::FileItem(QWidget* parent, FilesComponent* filesComponent, fileWrapper& fileWrapper, Theme theme)
    : QWidget(parent),
    m_files_component(filesComponent),
    m_file_wrapper(fileWrapper),
    m_theme(theme),
    m_isHovered(false),
    m_style(new StyleFileItem())
{
    initUI();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMouseTracking(true);

    connect(this, &FileItem::clicked, this, [this, filesComponent]() {
        if (m_file_wrapper.isPresent) {
            QString filePath = QString::fromStdString(m_file_wrapper.file.filePath);
            QUrl fileUrl = QUrl::fromLocalFile(filePath);
            if (!QDesktopServices::openUrl(fileUrl)) {
                qWarning() << "Не удалось открыть файл:" << filePath;
            }
        }
        else {
            if (!m_isLoading) {
                startLoadingAnimation();
                setAttribute(Qt::WA_TransparentForMouseEvents, true);
                filesComponent->onFileClicked(m_file_wrapper);
            }
            
        }
    });
}

void FileItem::updateFileInfo(const fileWrapper& wrapper) {
    m_file_wrapper = wrapper;
}

void FileItem::startLoadingAnimation() {
    if (!m_loadingAnimation) {
        m_loadingAnimation = new QVariantAnimation(this);
        m_loadingAnimation->setDuration(1000);
        m_loadingAnimation->setLoopCount(-1);
        m_loadingAnimation->setStartValue(0);
        m_loadingAnimation->setEndValue(360); 

        connect(m_loadingAnimation, &QVariantAnimation::valueChanged, this, [this](const QVariant& value) {
            m_rotationAngle = value.toInt();
            update(); 
            });
    }

    if (m_loadingAnimation->state() != QAbstractAnimation::Running) {
        m_loadingAnimation->start();
    }

    m_isLoading = true;
    update();
}

void FileItem::stopLoadingAnimation() {
    if (m_loadingAnimation && m_loadingAnimation->state() == QAbstractAnimation::Running) {
        m_loadingAnimation->stop();
    }
    m_isLoading = false;
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    update();
}


void FileItem::initUI() {
    m_style = new StyleFileItem;

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 5, 5, 5);

    m_iconBtn = new QPushButton(this);
    m_iconBtn->setIconSize(QSize(32, 32));
    m_iconBtn->setFlat(true);
    m_iconBtn->installEventFilter(this);

    m_nameLabel = new QLabel(std::filesystem::path(m_file_wrapper.file.filePath).filename().string().c_str(), this);
    m_nameLabel->setWordWrap(true);
    m_nameLabel->installEventFilter(this);

    m_sizeLabel = new QLabel(QString("%1 KB").arg(m_file_wrapper.file.fileSize / 1024), this);
    m_sizeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_sizeLabel->installEventFilter(this);

    layout->addSpacing(8);
    layout->addWidget(m_iconBtn);
    layout->addWidget(m_nameLabel, 1);
    layout->addSpacing(50);
    layout->addWidget(m_sizeLabel);
    setTheme(m_theme);
}

void FileItem::setTheme(Theme theme) {
    m_theme = theme;

    if (m_theme == DARK) {
        if (m_file_wrapper.isPresent) {
            m_iconBtn->setIcon(QIcon(":/resources/ChatsWidget/fileDark.png"));
        }
        else {
            m_iconBtn->setIcon(QIcon(":/resources/ChatsWidget/fileDarkNeedToLoad.png"));
        }

        setStyleSheet(m_style->darkThemeStyle);
    }
    else {
        if (m_file_wrapper.isPresent) {
            m_iconBtn->setIcon(QIcon(":/resources/ChatsWidget/fileLight.png"));
        }
        else {
            m_iconBtn->setIcon(QIcon(":/resources/ChatsWidget/fileLightNeedToLoad.png"));
        }

        setStyleSheet(m_style->lightThemeStyle);        
    }

    m_iconBtn->setStyleSheet(m_style->iconButtonStyle);
    update();
}

void FileItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_isLoading) {
        painter.save();
        QPen pen(Qt::white, 2);
        pen.setColor(QColor(61, 139, 255));
        painter.setPen(pen);

        int size = qMin(width(), height()) / 3;
        QRect loadingRect(width() - size - 60, height() - size - 15, size, size);

        static int arcDirection = 1; 
        static int currentArcLength = 20;

        currentArcLength += arcDirection * 2; 

        if (currentArcLength >= 300) {
            currentArcLength = 300;
            arcDirection = -1;
        }
        else if (currentArcLength <= 20) {
            currentArcLength = 20;
            arcDirection = 1;
        }

        painter.translate(loadingRect.center());
        painter.rotate(m_rotationAngle);
        painter.translate(-loadingRect.center());

        painter.drawArc(loadingRect, 0, currentArcLength * 16);

        painter.restore();
    }

    if (m_isHovered) {
        QPainterPath path;
        path.addRoundedRect(rect(), 0, 0);

        QColor bgColor;

        if (m_theme == Theme::DARK) {
            if (m_isHovered) {
                bgColor = QColor(132, 132, 132);
            }
            else {
                bgColor = QColor(112, 112, 112);
            }
        }
        else {
            if (m_isHovered) {
                bgColor = QColor(230, 230, 230);
            }
            else {
                bgColor = QColor(162, 162, 162);
            }
        }

        painter.fillPath(path, bgColor);
    }

    QWidget::paintEvent(event);
}

void FileItem::enterEvent(QEnterEvent* event) {
    m_isHovered = true;
    update();
    QWidget::enterEvent(event);
}

void FileItem::leaveEvent(QEvent* event) {
    m_isHovered = false;
    update();
    QWidget::leaveEvent(event);
}

bool FileItem::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::MouseButtonPress) {
        emit clicked();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}