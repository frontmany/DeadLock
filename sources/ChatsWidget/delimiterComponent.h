#pragma once
#include <QWidget>
#include <QLabel>
#include <QPainter>
#include <QVBoxLayout>
#include <QPainterPath>

enum Theme;

struct StyleDelimiterComponent {
    StyleDelimiterComponent();
    QString DarkLabelStyle;
    QString LightLabelStyle;
};

class DelimiterComponent : public QWidget
{
    Q_OBJECT

public:
    explicit DelimiterComponent(const QString& text, QWidget* parent, Theme theme);
    ~DelimiterComponent() { delete m_style; }
    void setTheme(Theme theme);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    StyleDelimiterComponent* m_style;
    Theme m_theme;
    QString m_text;
    QLabel* m_label;

    QColor m_bgColor;
    QColor m_textColor;

    
};
