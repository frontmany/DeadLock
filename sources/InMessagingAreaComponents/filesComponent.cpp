#include "FilesComponent.h"
#include "imageGridItem.h"
#include "messageComponent.h"
#include "message.h"
#include "fileItem.h"
#include "captionItem.h"
#include "message.h"

#include <vector>
#include <utility>
#include <algorithm>
#include <limits>
#include <cmath>
#include <queue>


FilesComponent::FilesComponent(QWidget* parent, MessageComponent* messageComponent, Message* parentMessage,
    Theme theme)
    : QWidget(parent),
    m_parent_message(parentMessage),
    m_message_component(messageComponent),
    m_theme(theme)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    setMaximumSize(400, 8000);

    m_mainVLayout = new QVBoxLayout(this);
    m_mainVLayout->setContentsMargins(0, 0, 0, 0);

    m_imagesVLayout = new QVBoxLayout();
    m_imagesVLayout->setContentsMargins(0, 0, 0, 0);

    m_filesVLayout = new QVBoxLayout();
    m_filesVLayout->setContentsMargins(0, 0, 0, 0);

    m_imagesHMainLayout = new QHBoxLayout();
    m_imagesHMainLayout->setContentsMargins(0, 0, 0, 0);

    m_captionItem = new CaptionItem(this, m_parent_message->getIsSend(), m_theme);
    m_captionItem->setMessage(QString::fromStdString(m_parent_message->getMessage()));
    m_captionItem->setTimestamp(QString::fromStdString(m_parent_message->getTimestamp()));

    QVector<std::pair<QString, QImage>> images;
    std::vector<fileWrapper*> otherFiles;

    for (auto& fileWrapper : m_message_component->getMessage()->getRelatedFiles()) {
        QString filePath = QString::fromStdString(fileWrapper.file.filePath);
        if (isImage(filePath)) {
            QImage img(filePath);
            if (!img.isNull()) {
                images.emplace_back(filePath, img);
            }
            else {
                qWarning() << "Failed to load image:" << filePath;
            }
        }
        else {
            otherFiles.push_back(&fileWrapper);
        }
    }

    if (!images.empty()) {
        Image exceptionImage{};
        std::vector<ImageRow> imageRows = arrangeImages(images, exceptionImage);
        
        for (ImageRow& imgRow : imageRows) {  
            QHBoxLayout* rowLayout = new QHBoxLayout();
            rowLayout->setContentsMargins(0, 0, 0, 0);

            for (Image& img : imgRow.imagesVec) {
                ImageGridItem* item = new ImageGridItem(this, this, img, img.filePath);
                rowLayout->addWidget(item);
            }

            m_imagesVLayout->addLayout(rowLayout);
        }

        if (exceptionImage.filePath != "") {
            ImageGridItem* item = new ImageGridItem(this, this, exceptionImage, exceptionImage.filePath);
            m_imagesHMainLayout->addWidget(item);
        }
        
        m_imagesHMainLayout->addLayout(m_imagesVLayout);
    }

    if (!otherFiles.empty()) {
        for (fileWrapper* file : otherFiles) {
            FileItem* fileWidget = new FileItem(this, this, *file, m_theme, m_parent_message->getIsSend());
            m_vec_file_items.push_back(fileWidget);
            m_filesVLayout->addWidget(fileWidget);
        }
    }

    m_mainVLayout->addLayout(m_imagesHMainLayout);
    if (images.empty()) {
        m_mainVLayout->addSpacing(6);
    }
    m_mainVLayout->addLayout(m_filesVLayout);
    if (m_parent_message->getMessage() == "") {
        m_mainVLayout->addSpacing(-36);
    }
    m_mainVLayout->addWidget(m_captionItem);

    connect(this, &FilesComponent::fileClicked, m_message_component, &MessageComponent::onSendMeFile);
    
    setTheme(m_theme);
}

std::optional<Image> FilesComponent::findExceptionImage(const std::vector<Image>& imagesVec) {
    for (const auto& image : imagesVec) {
        if (image.height > image.width) {
            double ratio = static_cast<double>(image.height) / image.width;
            if (ratio > 2.5) {
                return image;
            }
        }
    }

    return std::nullopt;
}


std::vector<ImageRow> FilesComponent::arrangeImages(const QVector<std::pair<QString, QImage>>& images, Image& exceptionImage) {
    if (images.empty()) {
        return {};
    }

    const double containerWidth = 400.0;
    const double containerHeight = 600.0;
     
    std::vector<Image> imagesVec;
    for (const auto& img : images) {
        imagesVec.push_back({ img.second, img.first, img.second.width(), img.second.height() });
    }
    
    auto optionalExceptionImage = findExceptionImage(imagesVec);
    if (optionalExceptionImage.has_value()) {
        exceptionImage = optionalExceptionImage.value();
    }

    
    imagesVec.erase(
        std::remove_if(
            imagesVec.begin(),
            imagesVec.end(),
            [&exceptionImage](const Image& img) {
                return img.filePath == exceptionImage.filePath;
            }
        ),
        imagesVec.end()
    );

    if (imagesVec.size() == 1) {
        Image image = imagesVec[0];
        double ratioCoefficient = 0;
        if (image.width > image.height) {
            ratioCoefficient = static_cast<double>(image.width) / image.height;
        }
        else {
            ratioCoefficient = static_cast<double>(image.height) / image.width;
        }

        double newWidth, newHeight;

        if (image.width > image.height) {
            newWidth = containerWidth;
            newHeight = containerWidth / ratioCoefficient;

            if (newHeight > containerHeight) {
                newHeight = containerHeight;
                newWidth = containerHeight * ratioCoefficient;
            }
        }
        else {
            newHeight = containerHeight;
            newWidth = containerHeight / ratioCoefficient;

            if (newWidth > containerWidth) {
                newWidth = containerWidth;
                newHeight = containerWidth * ratioCoefficient;
            }
        }

        Image scaledImage;
        scaledImage.width = static_cast<int>(newWidth);
        scaledImage.height = static_cast<int>(newHeight);
        scaledImage.filePath = image.filePath;
        scaledImage.image = image.image;

        ImageRow row;
        row.imagesVec.push_back(scaledImage);

        std::vector<ImageRow> vec;
        vec.push_back(row);
        return vec;
    }

    else {
        double totalArea = 0;
        for (const auto& img : imagesVec) {
            totalArea += img.width * img.height;
        }

        const double containerArea = containerWidth * containerHeight;
        const double scaleCoeff = 1; // Additional scaling coefficient

        double c_s = std::sqrt(totalArea / containerArea);
        double maxWidth = 0, maxHeight = 0;
        for (const auto& img : imagesVec) {
            if (img.width > maxWidth) maxWidth = img.width;
            if (img.height > maxHeight) maxHeight = img.height;
        }

        double c_w = maxWidth / containerWidth;
        double c_h = maxHeight / containerHeight;
        double c_base = std::max({ c_s, c_w, c_h });
        double c = c_base * scaleCoeff;

        std::vector<Image> scaledImages;
        for (const auto& img : imagesVec) {
            Image scaled;
            scaled.width = img.width / c;
            scaled.height = img.height / c;
            scaled.filePath = img.filePath;
            scaled.image = img.image;
            scaledImages.push_back(scaled);
        }

        double avgWidth = 0, avgHeight = 0;
        for (const auto& img : scaledImages) {
            avgWidth += img.width;
            avgHeight += img.height;
        }
        avgWidth /= scaledImages.size();
        avgHeight /= scaledImages.size();

        std::vector<Image> equalHeightImages;
        for (const auto& img : scaledImages) {
            Image scaled;
            double factor = avgHeight / img.height;
            scaled.width = img.width * factor;
            scaled.height = avgHeight;
            scaled.filePath = img.filePath;
            scaled.image = img.image;
            equalHeightImages.push_back(scaled);
        }


        std::vector<ImageRow> result;

        std::sort(equalHeightImages.begin(), equalHeightImages.end(),
            [](const Image& a, const Image& b) { return a.width > b.width; });

        std::vector<Image> currentRow;
        double currentRowWidth = 0;

        for (const auto& img : equalHeightImages) {
            if (currentRowWidth + img.width <= containerWidth) {
                currentRow.push_back(img);
                currentRowWidth += img.width;
            }
            else {
                if (!currentRow.empty()) {
                    double scaleFactor = containerWidth / currentRowWidth;
                    ImageRow row;
                    for (auto& rowImg : currentRow) {
                        Image scaled;
                        scaled.width = rowImg.width * scaleFactor;
                        scaled.height = rowImg.height * scaleFactor;
                        scaled.filePath = rowImg.filePath;
                        scaled.image = rowImg.image;
                        row.imagesVec.push_back(scaled);
                    }
                    result.push_back(row);
                }

                currentRow.clear();
                currentRow.push_back(img);
                currentRowWidth = img.width;
            }
        }

        if (!currentRow.empty()) {
            double scaleFactor = containerWidth / currentRowWidth;
            ImageRow row;
            for (auto& rowImg : currentRow) {
                Image scaled;
                scaled.width = rowImg.width * scaleFactor;
                scaled.height = rowImg.height * scaleFactor;
                scaled.filePath = rowImg.filePath;
                scaled.image = rowImg.image;
                row.imagesVec.push_back(scaled);
            }
            result.push_back(row);
        }


        double totalRowsHeight = 0;
        if (!result.empty()) {
            for (const auto& row : result) {
                if (!row.imagesVec.empty()) {
                    totalRowsHeight += row.imagesVec[0].height;
                }
            }
        }
        else {
            totalRowsHeight = containerHeight / 3;
        }

        
        if (optionalExceptionImage.has_value()) {
            Image scaled;
            scaled.image = exceptionImage.image;
            scaled.filePath = exceptionImage.filePath;

            double ratio = static_cast<double>(exceptionImage.height) / exceptionImage.width;
            scaled.height = totalRowsHeight;
            scaled.width = totalRowsHeight / ratio;

            if (scaled.width > containerWidth) {
                double scaleFactor = containerWidth / scaled.width;
                scaled.width = containerWidth;
                scaled.height *= scaleFactor;
            }

            exceptionImage = scaled;
        }
        
        return result;
    }
}

FilesComponent::~FilesComponent() {
    clearLayout();
}

void FilesComponent::setIsRead(bool isRead) {
    m_captionItem->setIsRead(isRead);
}

void FilesComponent::onFileClicked(const fileWrapper& fileWrapper) {
    emit fileClicked(fileWrapper);
}

void FilesComponent::setTheme(Theme theme) {
    m_theme = theme;
    m_captionItem->setTheme(m_theme);

    updateContainerStyle();

    for (auto item : m_vec_file_items) {
        item->setTheme(m_theme);
    }
}

void FilesComponent::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect rect = this->rect();

    // Choose background color based on theme
    QColor bgColor;
    if (m_is_need_to_retry) {
        bgColor = (m_theme == Theme::DARK) ? QColor(255, 117, 117) : QColor(255, 212, 212);
    }
    else {
        if (!m_parent_message->getIsSend()) {
            bgColor = (m_theme == Theme::DARK) ? QColor(85, 85, 85) : QColor(245, 245, 245);
        }
        else {
            bgColor = (m_theme == Theme::DARK) ? QColor(132, 132, 130) : QColor(240, 248, 255);
        }
    }

    // Draw rounded rectangle background
    painter.setPen(Qt::NoPen);
    painter.setBrush(bgColor);
    painter.drawRoundedRect(rect, 8, 8);

    QWidget::paintEvent(event);
}

void FilesComponent::updateContainerStyle() {
    update();
}

void FilesComponent::setRetryStyle(bool isNeedToRetry) {
    m_is_need_to_retry = isNeedToRetry;
    QString style;
    if (isNeedToRetry) {
        if (m_theme == Theme::DARK) {
            style = "background-color: rgb(255, 117, 117); border-radius: 8px;";

        }
        else {
            style = "background-color: rgb(255, 212, 212); border-radius: 8px;";
        }
        setStyleSheet(style);
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
    while ((child = m_filesVLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            delete child->widget();
        }
        delete child;
    }
}

void FilesComponent::setProgress(const net::File& file, int percent) {
    for (auto* fileItem : m_vec_file_items) {
        if (fileItem->getFileWrapper().file.id == file.id) {
            fileItem->setProgress(percent);
            break;
        }
    }
}

void FilesComponent::requestedFileLoaded(const fileWrapper& fileWrapper) {
    bool fileFound = false;
    for (auto& file : m_message_component->getMessage()->getRelatedFiles()) {
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
            fileItem->stopProgressAnimation();
            fileItem->setTheme(m_theme);
            fileItem->update();
            break;
        }
    }
}

void FilesComponent::requestedFileUnLoaded(const fileWrapper& fileWrapper) {
    for (auto* fileItem : m_vec_file_items) {
        if (fileItem->getFileWrapper().file.id == fileWrapper.file.id) {
            fileItem->stopProgressAnimation();
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