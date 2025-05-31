#include "ImageGridItem.h"
#include "filesComponent.h"

ImageGridItem::ImageGridItem(QWidget* parent, FilesComponent* filesComponent, fileWrapper& fileWrapper, int rowHeight)
    : QWidget(parent), m_file_wrapper(fileWrapper),
    m_cornerRadius(5), m_rowHeight(rowHeight),
    m_aspectRatio(1.0), m_files_component(filesComponent)
{
    setAttribute(Qt::WA_Hover);
    setMouseTracking(true);

    QPixmap pixmap(m_file_wrapper.file.filePath.c_str());
    if (!pixmap.isNull()) {
        m_originalPixmap = pixmap;
        m_aspectRatio = static_cast<double>(pixmap.width()) / pixmap.height();
    }

    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumHeight(m_rowHeight);
    updateSizeConstraints();
    updatePixmaps();

    connect(this, &ImageGridItem::clicked, this, [this, fileWrapper]() {
        QString filePath = QString::fromStdString(fileWrapper.file.filePath);
        QUrl fileUrl = QUrl::fromLocalFile(filePath);
        if (!QDesktopServices::openUrl(fileUrl)) {
            qWarning() << "Не удалось открыть файл:" << filePath;
        }
        });
}

void ImageGridItem::updateSizeConstraints() {
    int minWidth = static_cast<int>(m_rowHeight * m_aspectRatio * 0.8);
    int maxWidth = static_cast<int>(m_rowHeight * m_aspectRatio * 1.0);

    setMinimumWidth(minWidth);
    setMaximumWidth(maxWidth);
    updateGeometry();
}

void ImageGridItem::setRowHeight(int height) {
    m_rowHeight = height;
    setFixedHeight(height);
    updateSizeConstraints();
    updatePixmaps();
    update();
}

void ImageGridItem::setCornerRadius(int radius) {
    m_cornerRadius = radius;
    updatePixmaps();
    update();
}

double ImageGridItem::aspectRatio() const {
    return m_aspectRatio;
}

void ImageGridItem::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRect targetRect = rect();

    if (!m_roundedPixmap.isNull()) {
        QPainterPath clipPath;
        clipPath.addRoundedRect(targetRect, m_cornerRadius, m_cornerRadius);
        painter.setClipPath(clipPath);

        painter.drawPixmap(targetRect, m_roundedPixmap);

        if (m_hovered) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 30));
            painter.drawRoundedRect(targetRect, m_cornerRadius, m_cornerRadius);
        }
    }
}

QSize ImageGridItem::calculateDisplaySize() const {
    int maxWidth = width();
    int calculatedHeight = m_rowHeight;
    int calculatedWidth = static_cast<int>(calculatedHeight * m_aspectRatio);

    if (calculatedWidth > maxWidth) {
        calculatedWidth = maxWidth;
        calculatedHeight = static_cast<int>(maxWidth / m_aspectRatio);
    }

    return QSize(calculatedWidth, calculatedHeight);
}

void ImageGridItem::updatePixmaps() {
    if (m_originalPixmap.isNull()) return;

    QSize displaySize = calculateDisplaySize();
    m_scaledPixmap = m_originalPixmap.scaled(
        displaySize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );

    m_roundedPixmap = QPixmap(displaySize);
    m_roundedPixmap.fill(Qt::transparent);

    QPainter painter(&m_roundedPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(m_roundedPixmap.rect(), m_cornerRadius, m_cornerRadius);
    painter.setClipPath(path);
    painter.drawPixmap(0, 0, m_scaledPixmap);
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