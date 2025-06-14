#pragma once

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <QFileInfo>
#include <QPainter>
#include <QEvent>
#include <QPainterPath>

#include "theme.h"

struct StyleCaptionItem {
    StyleCaptionItem();

    QString darkSentWidget;
    QString lightSentWidget;

    QString darkReceivedWidget;
    QString lightReceivedWidget;

    QString darkSentMessage;
    QString lightSentMessage;

    QString darkReceivedMessage;
    QString lightReceivedMessage;

    QString darkTimestamp;
    QString lightTimestamp;
};

class CaptionItem : public QWidget {
    Q_OBJECT
public:
    explicit CaptionItem(QWidget* parent, bool isSent, Theme theme);
    ~CaptionItem();

    void setMessage(const QString& message);
    void setTimestamp(const QString& timestamp);
    void setTheme(Theme theme);
    void setIsRead(bool isRead);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void updateLayout();
    void updateStyles();

    QLabel* m_messageText;
    QLabel* m_timestampLabel;
    QPushButton* m_readStatusIcon;

    bool m_isRead;
    bool m_isSent;
    Theme m_theme;
    QColor m_backgroundColor;
    StyleCaptionItem m_style;
};