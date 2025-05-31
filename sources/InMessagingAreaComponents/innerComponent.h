#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QSize>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QPainter>
#include <QScrollBar>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#include "theme.h"

class ButtonIcon;
class MessagingAreaComponent;

struct StyleInnerComponent {
    StyleInnerComponent();
    QString labelStyleDarkMessage;
    QString labelStyleLightMessage;
    QString labelStyleDarkTime;
    QString labelStyleLightTime;
};


class InnerComponent : public QWidget {
public:
    InnerComponent(QWidget* parent, const QString& timestamp, const QString& text, Theme theme, bool isSent);
    ~InnerComponent();

    void setTheme(Theme theme);

    void setText(const QString& text) { m_textLabel->setText(text); }
    const QString& getText() const { return m_textLabel->text(); }
    void setTimestamp(const QString& text) { m_timestampLabel->setText(text); }
    const QString& getTimestamp() { return m_timestampLabel->text(); }
    bool getIsRead() const { return m_is_read; }
    void setIsRead(bool isRead) { m_is_read = isRead; }
    void setReadStatus(bool read);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_is_read = false;
    bool m_isSent = false;
    StyleInnerComponent* style;
    Theme m_theme;
    QColor m_backColor;

    QLabel* m_textLabel;
    QLabel* m_timestampLabel;
    ButtonIcon* m_readStatusBtn;

    QHBoxLayout* m_main_HLayout;
    QVBoxLayout* m_text_VLayout;
    QVBoxLayout* m_time_VLayout;
    QVBoxLayout* m_isRead_VLayout;

};