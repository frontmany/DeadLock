#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QFileInfo>
#include <QPainter>
#include <QEvent>
#include <QPainterPath>
#include <QVariantAnimation>
#include <QMovie>

#include "theme.h"

class FilesComponent;
class fileWrapper;

struct StyleFileItem {
    StyleFileItem();

    QString darkThemeStyle;
    QString lightThemeStyle;
    QString highlightedDarkStyle;
    QString highlightedLightStyle;
    QString iconButtonStyle;
};

class FileItem : public QWidget {
    Q_OBJECT

public:
    FileItem(QWidget* parent, FilesComponent* filesComponent, fileWrapper& fileWrapper, Theme theme);
    ~FileItem();

    void setTheme(Theme theme);
    void setRetryStyle(bool isNeedToRetry);
    void updateFileInfo(const fileWrapper& wrapper);
    const fileWrapper& getFileWrapper() { return m_file_wrapper; }

    void setProgress(int percent);
    void startProgressAnimation();
    void stopProgressAnimation();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setDownloadState(bool inProgress);
    void updateProgressLabel();

    Theme m_theme;
    bool m_isHovered;
    bool m_isNeedToRetry = false;
    fileWrapper& m_file_wrapper;
    FilesComponent* m_files_component;
    StyleFileItem* m_style;
    QPushButton* m_iconBtn;
    QLabel* m_nameLabel;
    QLabel* m_sizeLabel;
    QLabel* m_progressLabel;
    QLabel* m_loadingAnimationLabel;
    QMovie* m_loadingMovie;
    int m_progress = 0;
    bool m_isLoading = false;
    bool m_isAnimationStarted = false;
};