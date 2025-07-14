#include "ImageGridItem.h"
#include "filesComponent.h"

ImageGridItem::ImageGridItem(QWidget* parent, FilesComponent* filesComponent, Image& image, const QString& filePath)
    : QWidget(parent),
    m_image(image),
    m_cornerRadius(5),
    m_aspectRatio(static_cast<double>(image.width) / image.height),
    m_filePath(filePath),
    m_files_component(filesComponent),
    m_hovered(false)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

    setFixedSize(m_image.width, m_image.height);

    if (!m_image.image.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(m_image.image.scaled(
            m_image.width,
            m_image.height,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
        ));

        if (!pixmap.isNull()) {
            m_originalPixmap = pixmap;
            updateRoundedPixmap();
        }
    }

    connect(this, &ImageGridItem::clicked, this, [this, filePath]() {
        QUrl fileUrl = QUrl::fromLocalFile(filePath);
        if (!QDesktopServices::openUrl(fileUrl)) {
            qWarning() << "Не удалось открыть файл:" << filePath;
        }
        });
}

void ImageGridItem::updateRoundedPixmap() {
    if (m_originalPixmap.isNull()) return;

    QPixmap rounded(size());
    rounded.fill(Qt::transparent);

    QPainter painter(&rounded);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect targetRect(0, 0, width(), height());
    QPixmap scaledPixmap = m_originalPixmap.scaled(
        targetRect.size(),
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    QRect centeredRect = scaledPixmap.rect();
    centeredRect.moveCenter(targetRect.center());

    QPainterPath path;
    path.addRoundedRect(targetRect, m_cornerRadius, m_cornerRadius);
    painter.setClipPath(path);
    painter.drawPixmap(centeredRect, scaledPixmap);

    m_roundedPixmap = rounded;
}

void ImageGridItem::setCornerRadius(int radius) {
    m_cornerRadius = radius;
    updateRoundedPixmap();
    update();
}

void ImageGridItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!m_roundedPixmap.isNull()) {
        painter.drawPixmap(rect(), m_roundedPixmap);

        if (m_hovered) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 30));
            painter.drawRoundedRect(rect(), m_cornerRadius, m_cornerRadius);
        }
    }
}

void ImageGridItem::resizeEvent(QResizeEvent* event) {
    updateRoundedPixmap();
    QWidget::resizeEvent(event);
}

void ImageGridItem::hoverEnter(QHoverEvent* event) {
    m_hovered = true;
    setCursor(Qt::PointingHandCursor);
    update();
}

void ImageGridItem::hoverLeave(QHoverEvent* event) {
    m_hovered = false;
    setCursor(Qt::ArrowCursor);
    update();
}

void ImageGridItem::hoverMove(QHoverEvent* event) {
    update();
}

void ImageGridItem::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}