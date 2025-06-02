#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QDesktopServices>
#include <QUrl>
#include <QFileInfo>
#include <QPainter>
#include <QEvent>
#include <QPainterPath>
#include <QVariantAnimation>

#include "theme.h"
#include "fileWrapper.h"

class FilesComponent;

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
    ~FileItem() { delete m_style; }

    void setTheme(Theme theme);
    void setRetryStyle(bool isNeedToRetry);
    const fileWrapper& getFileWrapper() { return m_file_wrapper; }
    void updateFileInfo(const fileWrapper& wrapper);
    void startLoadingAnimation();
    void stopLoadingAnimation();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void initUI();

    Theme              m_theme;
    bool               m_isHovered;
    bool               m_isNeedToRetry;
    fileWrapper&       m_file_wrapper;

    QVariantAnimation* m_loadingAnimation = nullptr;
    int m_rotationAngle = 0;
    bool m_isLoading = false;

    FilesComponent*     m_files_component;
    StyleFileItem*      m_style;
    QPushButton*       m_iconBtn;
    QLabel*            m_nameLabel;
    QLabel*            m_sizeLabel;
};