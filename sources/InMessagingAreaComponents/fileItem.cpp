#include <filesystem>

#include "fileItem.h"
#include "filesComponent.h"


StyleFileItem::StyleFileItem() {
    darkThemeStyle = R"(
        QWidget {
            background-color: transparent;
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
            background-color: transparent;
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
            background-color: transparent;
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
            background-color: transparent;
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

FileItem::FileItem(QWidget* parent, FilesComponent* filesComponent, fileWrapper& fileWrapper, Theme theme, bool isSent)
    : QWidget(parent),
    m_files_component(filesComponent),
    m_file_wrapper(fileWrapper),
    m_theme(theme),
    m_isHovered(false),
    m_isSent(isSent)
{
    m_style = new StyleFileItem;

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMouseTracking(true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 5, 5, 5);

    m_iconBtn = new QPushButton(this);
    m_iconBtn->setIconSize(QSize(32, 32));
    m_iconBtn->setFlat(true);
    m_iconBtn->installEventFilter(this);

    m_nameLabel = new QLabel(m_file_wrapper.file.fileName.c_str(), this);
    m_nameLabel->setWordWrap(true);
    m_nameLabel->installEventFilter(this);

    m_loadingAnimationLabel = new QLabel(this);
    m_loadingMovie = new QMovie(":/resources/ChatsWidget/loading.gif");
    m_loadingAnimationLabel->setMovie(m_loadingMovie);
    m_loadingAnimationLabel->setFixedSize(32, 32);
    m_loadingAnimationLabel->setVisible(false);

    m_progressLabel = new QLabel(this);
    m_progressLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_progressLabel->setVisible(false);

    m_sizeLabel = new QLabel(formatFileSize(QString::fromStdString(m_file_wrapper.file.fileSize)), this);
    m_sizeLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_sizeLabel->installEventFilter(this);


    layout->addSpacing(8);
    layout->addWidget(m_iconBtn);
    layout->addWidget(m_nameLabel, 1);
    layout->addWidget(m_loadingAnimationLabel);
    layout->addWidget(m_progressLabel);
    layout->addSpacing(50);
    layout->addWidget(m_sizeLabel);

    setTheme(m_theme);

    connect(this, &FileItem::clicked, [this, filesComponent]() {
        if (m_file_wrapper.isPresent) {
            QString filePath = QString::fromStdString(m_file_wrapper.file.filePath);
            QUrl fileUrl = QUrl::fromLocalFile(filePath);
            if (!QDesktopServices::openUrl(fileUrl)) {
                qWarning() << "Не удалось открыть файл:" << filePath;
            }
        }
        else {
            if (!m_isLoading) {
                startProgressAnimation();
                setAttribute(Qt::WA_TransparentForMouseEvents, true);
                filesComponent->onFileClicked(m_file_wrapper);
            }
        }
        });
}

QString FileItem::formatFileSize(const QString& sizeStr) {
    bool ok = false;
    qint64 size = sizeStr.toLongLong(&ok);
    if (!ok || size < 0)
        return "Unknown size";

    static const char* units[] = { "B", "KB", "MB", "GB", "TB" };
    int unitIndex = 0;
    double displaySize = static_cast<double>(size);

    while (displaySize >= 1024.0 && unitIndex < 4) {
        displaySize /= 1024.0;
        ++unitIndex;
    }

    return QString("%1 %2").arg(QString::number(displaySize, 'f', 2)).arg(units[unitIndex]);
}


FileItem::~FileItem() {
    delete m_style;
    if (m_loadingMovie) {
        m_loadingMovie->stop();
        delete m_loadingMovie;
    }
}

void FileItem::setProgress(int percent) {
    if (!m_isAnimationStarted) {
        startProgressAnimation();
    }

    percent = qBound(0, percent, 100);
    if (m_progress != percent) {
        m_progress = percent;
        updateProgressLabel();
    }

    if (percent >= 100) {
        stopProgressAnimation();
    }
}

void FileItem::updateProgressLabel() {
    m_progressLabel->setText(QString("%1%").arg(m_progress));
}

void FileItem::setDownloadState(bool inProgress) {
    if (m_isLoading != inProgress) {
        m_isLoading = inProgress;

        m_loadingAnimationLabel->setVisible(inProgress);
        m_progressLabel->setVisible(inProgress);

        if (inProgress) {
            m_loadingMovie->start();
        }
        else {
            m_loadingMovie->stop();
        }

        update();
    }
}

void FileItem::startProgressAnimation() {
    m_isAnimationStarted = true;
    m_progress = 0;
    setDownloadState(true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    updateProgressLabel();
}

void FileItem::stopProgressAnimation() {
    m_file_wrapper.isSending = false;
    m_isAnimationStarted = false;
    setDownloadState(false);
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
}

void FileItem::updateFileInfo(const fileWrapper& wrapper) {
    m_file_wrapper = wrapper;
}

void FileItem::setRetryStyle(bool isNeedToRetry) {
    m_isNeedToRetry = isNeedToRetry;
    update();
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
        m_progressLabel->setStyleSheet("color: white; font-family: 'Segoe UI'; font-weight: bold;");
    }
    else {
        if (m_file_wrapper.isPresent) {
            m_iconBtn->setIcon(QIcon(":/resources/ChatsWidget/fileLight.png"));
        }
        else {
            m_iconBtn->setIcon(QIcon(":/resources/ChatsWidget/fileLightNeedToLoad.png"));
        }
        setStyleSheet(m_style->lightThemeStyle);
        m_progressLabel->setStyleSheet("color: black; font-family: 'Segoe UI'; font-weight: bold;");
    }

    m_iconBtn->setStyleSheet(m_style->iconButtonStyle);
    update();
}

void FileItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 0, 0);

    QColor bgColor;

    if (m_theme == Theme::DARK) {
        if (m_isHovered) {
            if (m_isNeedToRetry) {
                bgColor = QColor(255, 138, 138);
            }
            else {
                bgColor = QColor(143, 142, 140);
            }
        }
        else {
            if (m_isNeedToRetry) {
                bgColor = QColor(255, 117, 117);
            }
            else {
                if (m_isSent) {
                    bgColor = QColor(132, 132, 130);
                }
                else {
                    bgColor = QColor(85, 85, 85);
                }
            }
        }
    }
    else {
        if (m_isHovered) {
            if (m_isNeedToRetry) {
                bgColor = QColor(255, 224, 224);
            }
            else {
                if (m_isSent) {
                    bgColor = QColor(250, 253, 255);
                }
                else {
                    bgColor = QColor(250, 250, 250);
                }
            }
        }
        else {
            if (m_isNeedToRetry) {
                bgColor = QColor(255, 212, 212);
            }
            else {
                if (m_isSent) {
                    bgColor = QColor(240, 248, 255);
                }
                else {
                    bgColor = QColor(245, 245, 245);
                }
            }
        }
    }

    painter.fillPath(path, bgColor);
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