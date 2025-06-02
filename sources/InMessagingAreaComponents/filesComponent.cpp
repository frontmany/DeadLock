#include "FilesComponent.h"
#include "imageGridItem.h"
#include "messageComponent.h"
#include "message.h"
#include "fileItem.h"
#include "captionItem.h"
#include "message.h"

FilesComponent::FilesComponent(QWidget* parent, MessageComponent* messageComponent, Message* parentMessage,
    std::vector<fileWrapper>& fileWrappersVec,
    const QString& caption,
    const QString& timestamp,
    bool isRead,
    bool isSent,
    Theme theme)
    : QWidget(parent),
    m_vec_file_wrappers(fileWrappersVec),
    m_parent_message(parentMessage),
    m_message_component(messageComponent),
    m_caption(caption),
    m_theme(theme),
    m_timestamp(timestamp),
    m_isRead(isRead),
    m_isSent(isSent)
{
    m_mainHLayout = new QHBoxLayout(this);
    m_mainHLayout->setContentsMargins(0, 0, 0, 0);

    m_contentWrapper = new QWidget(this);
    m_contentWrapper->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    m_mainVLayout = new QVBoxLayout(m_contentWrapper);
    m_mainVLayout->setContentsMargins(0, 0, 0, 0);

    m_filesLayout = new QVBoxLayout();
    m_filesLayout->setContentsMargins(0, 5, 0, 0);

    m_mainVLayout->addLayout(m_filesLayout);

    m_captionItem = new CaptionItem(m_contentWrapper, m_isSent, m_theme);
    m_captionItem->setContentsMargins(0, 0, 0, 0);
    m_captionItem->setMessage(m_caption);
    m_captionItem->setTimestamp(m_timestamp);
    m_captionItem->setTheme(m_theme);

    std::vector<fileWrapper*> images;
    std::vector<fileWrapper*> otherFiles;

    for (auto& fileWrapper : m_vec_file_wrappers) {
        QString filePath = QString::fromStdString(fileWrapper.file.filePath);
        if (isImage(filePath)) {
            images.push_back(&fileWrapper);
        }
        else {
            otherFiles.push_back(&fileWrapper);
        }
    }

    m_is_files = !otherFiles.empty();

    if (m_caption == "" && otherFiles.size() != 0) {
        if (images.size() > 0) {
            m_mainVLayout->addSpacing(-42);
        }
        else {
            m_mainVLayout->addSpacing(-32);
        }
    }
    m_mainVLayout->addWidget(m_captionItem);
    m_mainHLayout->addWidget(m_contentWrapper);

    if (m_is_files) {
        m_mainVLayout->insertSpacing(1, -6);
    }

    if (!images.empty()) { 
        QVector<ImageRow> imageRows = calculateImageRows(images);
        for (ImageRow& row : imageRows) {
            QHBoxLayout* rowLayout = new QHBoxLayout();
            rowLayout->setSpacing(7);
            rowLayout->setContentsMargins(5, 0, 5, 0);

            for (fileWrapper* file : row.images) {
                ImageGridItem* item = new ImageGridItem(this, this, *file, row.optimalHeight);
                item->setCornerRadius(5);
                rowLayout->addWidget(item);
                
            }
            m_filesLayout->addLayout(rowLayout);
        }
    }

    m_filesLayout->addSpacing(4);

    if (!otherFiles.empty()) {
        m_filesContainer = new QWidget(this);

        QVBoxLayout* containerLayout = new QVBoxLayout(m_filesContainer);
        containerLayout->setContentsMargins(0, 15, 0, 0);
        containerLayout->setSpacing(2);

        for (fileWrapper* file : otherFiles) {
            FileItem* fileWidget = new FileItem(this, this, *file, m_theme);
            m_vec_file_items.push_back(fileWidget);
            containerLayout->addWidget(fileWidget);
        }

        if (!images.empty())
            m_filesLayout->addSpacing(-10);

        m_filesLayout->addWidget(m_filesContainer);

        if (!images.empty())
            m_filesLayout->addSpacing(10);
    }

    connect(this, &FilesComponent::fileClicked, m_message_component, &MessageComponent::onSendMeFile);

    setTheme(m_theme);
}


FilesComponent::~FilesComponent() {
    clearLayout();
}

void FilesComponent::setIsRead(bool isRead) {
    m_isRead = isRead;
    m_captionItem->setIsRead(isRead);
}

void FilesComponent::onFileClicked(const fileWrapper& fileWrapper) {
    emit fileClicked(fileWrapper);
}


QVector<FilesComponent::ImageRow> FilesComponent::calculateImageRows(std::vector<fileWrapper*>& images) const {
    QVector<ImageRow> rows;
    const int maxRowWidth = 200;
    const int minRowHeight = 50;
    const int maxRowHeight = 150;

    std::sort(images.begin(), images.end(),
        [](const fileWrapper* a, const fileWrapper* b) {
            QPixmap pixmapA(QString::fromStdString(a->file.filePath));
            QPixmap pixmapB(QString::fromStdString(b->file.filePath));
            double arA = pixmapA.width() / (double)pixmapA.height();
            double arB = pixmapB.width() / (double)pixmapB.height();
            return arA > arB;
        });

    ImageRow currentRow;
    double currentRowAspect = 0;

    for (fileWrapper* file : images) {
        QString filePath = QString::fromStdString(file->file.filePath);
        QPixmap pixmap(filePath);
        if (pixmap.isNull()) continue;

        double aspect = pixmap.width() / (double)pixmap.height();

        if (currentRow.images.empty() ||
            (currentRowAspect + aspect) * minRowHeight <= maxRowWidth) {
            currentRow.images.push_back(file);
            currentRowAspect += aspect;
        }
        else {
            currentRow.optimalHeight = calculateOptimalRowHeight(currentRow.images);
            rows.append(currentRow);

            currentRow = ImageRow();
            currentRow.images.push_back(file);
            currentRowAspect = aspect;
        }
    }

    if (!currentRow.images.empty()) {
        currentRow.optimalHeight = calculateOptimalRowHeight(currentRow.images);
        rows.append(currentRow);
    }

    return rows;
}

int FilesComponent::calculateOptimalRowHeight(std::vector<fileWrapper*>& rowImages) const {
    const int maxRowWidth = 600;
    const int minRowHeight = 150;
    const int maxRowHeight = 300;

    double totalAspect = 0;
    for (const fileWrapper* file : rowImages) {
        QString filePath = QString::fromStdString(file->file.filePath);
        QPixmap pixmap(filePath);
        if (!pixmap.isNull()) {
            totalAspect += pixmap.width() / (double)pixmap.height();
        }
    }

    if (totalAspect == 0) return minRowHeight;

    int optimalHeight = static_cast<int>(maxRowWidth / totalAspect);
    return qBound(minRowHeight, optimalHeight, maxRowHeight);
}

void FilesComponent::setTheme(Theme theme) {
    m_theme = theme;
    m_captionItem->setTheme(m_theme);

    updateContainerStyle();

    for (auto item : m_vec_file_items) {
        item->setTheme(m_theme);
    }
}

void FilesComponent::updateContainerStyle() {
    QString style;
    if (m_is_need_to_retry) {
        setRetryStyle(m_is_need_to_retry);
    }
    else {
        if (m_theme == Theme::DARK) {
            style = "background-color: rgb(112, 112, 112); border-radius: 8px;";
        }
        else {
            style = "background-color: rgb(225, 225, 225); border-radius: 8px;";
        }
        m_contentWrapper->setStyleSheet(style);
    }
}

void FilesComponent::setRetryStyle(bool isNeedToRetry) {
    m_is_need_to_retry = isNeedToRetry;
    QString style;
    if (isNeedToRetry) {
        if (m_theme == Theme::DARK) {
            style = "background-color: rgb(189, 170, 170); border-radius: 8px;";
            
        }
        else {
            style = "background-color: rgb(255, 212, 212); border-radius: 8px;";
        }
        m_contentWrapper->setStyleSheet(style);
    }
    else {
        updateContainerStyle();
    }
    
    for (auto f : m_vec_file_items) {
        f->setRetryStyle(isNeedToRetry);
    }
}

bool FilesComponent::isImage(const QString& filePath) const {
    static const QStringList imageExtensions = {
        ".jpg", ".jpeg", ".png", ".gif", ".bmp",
        ".webp", ".tiff", ".svg"
    };

    for (const QString& ext : imageExtensions) {
        if (filePath.endsWith(ext, Qt::CaseInsensitive)) {
            return true;
        }
    }
    return false;
}

void FilesComponent::clearLayout() {
    QLayoutItem* child;
    while ((child = m_filesLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }
}

void FilesComponent::requestedFileLoaded(const fileWrapper& fileWrapper) {
    bool fileFound = false;
    for (auto& file : m_vec_file_wrappers) {
        if (file.file.id == fileWrapper.file.id) {
            file = fileWrapper;
            fileFound = true;
            break;
        }
    }

    if (!fileFound) {
        qWarning() << "File not found in vector:" << QString::fromStdString(fileWrapper.file.id);
        return;
    }

    for (auto* fileItem : m_vec_file_items) {
        if (fileItem->getFileWrapper().file.id == fileWrapper.file.id) {
            fileItem->updateFileInfo(fileWrapper);
            fileItem->stopLoadingAnimation();
            fileItem->setTheme(m_theme);
            fileItem->update();
            break;
        }
    }
}

void FilesComponent::requestedFileUnLoaded(const fileWrapper& fileWrapper) {
    for (auto* fileItem : m_vec_file_items) {
        if (fileItem->getFileWrapper().file.id == fileWrapper.file.id) {
            fileItem->stopLoadingAnimation();
            fileItem->setTheme(m_theme);
            fileItem->update();
            break;
        }
    }
}

bool FilesComponent::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::HoverEnter) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            widget->setStyleSheet("background-color: #F0F0F0;");
        }
    }
    else if (event->type() == QEvent::HoverLeave) {
        QWidget* widget = qobject_cast<QWidget*>(obj);
        if (widget) {
            widget->setStyleSheet("background-color: transparent;");
        }
    }
    return QWidget::eventFilter(obj, event);
}
