#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QEvent>
#include <QHoverEvent>
#include <vector>
#include <fstream>

#include "theme.h"
#include "fileWrapper.h"


class ImageGridItem;
class Message;
class FileItem;
class CaptionItem;
class MessageComponent;

class FilesComponent : public QWidget {
    Q_OBJECT

private:
    struct ImageRow {
        std::vector<fileWrapper*> images;
        int optimalHeight;
        double totalAspectRatio;
    };

signals:
    void fileClicked(const fileWrapper& fileWrapper);

public slots:
    void onFileClicked(const fileWrapper& fileWrapper);

public:
    explicit FilesComponent(QWidget* parent, MessageComponent* messageComponent, Message* parentMessage, std::vector<fileWrapper>& fileWrappersVec,
        const QString& caption, const QString& timestamp, bool isRead, bool isSent, Theme theme);
    ~FilesComponent();
    void setTheme(Theme theme);

    void setChecked(); 
    void requestedFileLoaded(const fileWrapper& fileWrapper);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QVector<ImageRow> calculateImageRows(std::vector<fileWrapper*>& images) const;
    int calculateOptimalRowHeight(std::vector<fileWrapper*>& rowImages) const;
    bool isImage(const QString& filePath) const;
    void clearLayout();

    void updateFileContainerStyle(QWidget* container);
    void updateContainerStyle();

private:
    std::vector<fileWrapper>& m_vec_file_wrappers;
    std::vector<FileItem*> m_vec_file_items;
    

    Message* m_parent_message;
    QString  m_caption;
    Theme    m_theme;
    QString  m_timestamp;
    bool     m_is_files = false;
    bool     m_isRead;
    bool     m_isSent;

    MessageComponent* m_message_component;
    QWidget* m_filesContainer = nullptr;
    QWidget* m_contentWrapper;
    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_mainHLayout;
    QVBoxLayout* m_filesLayout;
    CaptionItem* m_captionItem;
};