#pragma once

#include <QWidget>
#include <QLayout>
#include <QDesktopServices>
#include <QURL>
#include <QFileInfo>
#include <QPainter>
#include <QPainterPath>
#include <QEvent>
#include <QHoverEvent>

#include "theme.h"

class FilesComponent;
class fileWrapper;
struct Image;

class ImageGridItem : public QWidget {
    Q_OBJECT

public:
    explicit ImageGridItem(QWidget* parent, FilesComponent* filesComponent, Image& image, const QString& filePath);
    void setCornerRadius(int radius);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    bool event(QEvent* event) override {
        switch (event->type()) {
        case QEvent::HoverEnter:
            hoverEnter(static_cast<QHoverEvent*>(event));
            return true;
        case QEvent::HoverLeave:
            hoverLeave(static_cast<QHoverEvent*>(event));
            return true;
        case QEvent::HoverMove:
            hoverMove(static_cast<QHoverEvent*>(event));
            return true;
        default:
            return QWidget::event(event);
        }
    }

    void hoverEnter(QHoverEvent* event);
    void hoverLeave(QHoverEvent* event);
    void hoverMove(QHoverEvent* event);
    void mouseReleaseEvent(QMouseEvent* event) override;
    void updateRoundedPixmap();

private:
    QString m_filePath;
    Image& m_image;

    FilesComponent* m_files_component;

    QPixmap m_originalPixmap;
    QPixmap m_scaledPixmap;
    QPixmap m_roundedPixmap;

    int     m_cornerRadius;
    double  m_aspectRatio;
    bool    m_hovered;
};