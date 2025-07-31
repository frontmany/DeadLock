#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include <QPainterPath>
#include <QPushButton>
#include <QEvent>
#include <QVector>
#include <QImage>
#include <QHoverEvent>
#include <vector>
#include <fstream>

#include "theme.h"
#include "net_file.h"
#include "filewrapper.h"
#include "queryType.h"


class ImageGridItem;
class Message;
class FileItem;
class CaptionItem;
class MessageComponent;

struct Image {
    QImage image;
    QString filePath;
    int width;
    int height;
};

struct ImageRow {
    std::vector<Image> imagesVec;
    double rowHeight;
};

class FilesComponent : public QWidget {
    Q_OBJECT

signals:
    void fileClicked(const fileWrapper& fileWrapper);


public slots:
    void onFileClicked(const fileWrapper& fileWrapper);


public:
    FilesComponent(QWidget* parent, MessageComponent* messageComponent, Message* parentMessage, Theme theme);
    ~FilesComponent();
    void setTheme(Theme theme);
    void setIsRead(bool isRead);
    void setRetryStyle(bool isNeedToRetry);
    void requestedFileLoaded(const fileWrapper& fileWrapper);
    void requestedFileUnLoaded(const fileWrapper& fileWrapper);
    void setProgress(const net::File& file, int percent);


protected:
    void paintEvent(QPaintEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    std::vector<ImageRow> arrangeImages(const QVector<std::pair<QString, QImage>>& images, Image& exceptionImage);
    std::optional<Image> findExceptionImage(const std::vector<Image>& imagesVec);
    
private:    
    bool isImage(const QString& filePath) const;
    void clearLayout();
    void updateContainerStyle();


private:
    std::vector<FileItem*> m_vec_file_items;
    Message* m_parent_message;
    Theme    m_theme;
    bool     m_is_need_to_retry = false;

    MessageComponent* m_message_component;
    QWidget* m_filesContainer;
    CaptionItem* m_captionItem;

    QVBoxLayout* m_mainVLayout;
    QVBoxLayout* m_filesVLayout;
    QVBoxLayout* m_imagesVLayout;
    QHBoxLayout* m_imagesHMainLayout;
};