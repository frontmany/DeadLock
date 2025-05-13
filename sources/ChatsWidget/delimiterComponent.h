#pragma once
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QPainterPath>

enum Theme;

class DelimiterComponent : public QWidget
{
    Q_OBJECT

public:
    explicit DelimiterComponent(const QString& text, QWidget* parent, Theme theme);
    void setTheme(Theme theme);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Theme m_theme;
    QString m_text;
    QLabel* m_label;

    QColor m_bgColor;
    QColor m_textColor;
};
