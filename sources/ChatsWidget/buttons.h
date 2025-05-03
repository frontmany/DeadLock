#pragma once

#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QHoverEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QPainter>
#include <QWidget>
#include <QPropertyAnimation>


enum Theme;


class ToggleSwitch : public QWidget {
    Q_OBJECT
    Q_PROPERTY(int indicatorX READ indicatorX WRITE setIndicatorX NOTIFY indicatorXChanged)

public:
    explicit ToggleSwitch(QWidget* parent, Theme theme);
    bool isChecked() const;
    void setBackgroundColor(const QColor& color);
    void setTheme(Theme theme);
    int indicatorX() const { return m_indicatorX; }
    void setIndicatorX(int value) {
        if (m_indicatorX != value) {
            m_indicatorX = value;
            emit indicatorXChanged(m_indicatorX);
            update();
        }
    }

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    void updateAnimation();
    QColor backgroundColor() const; // Метод для получения цвета фона

signals:
    void toggled(bool checked);
    void indicatorXChanged(int newValue);

private:
    bool m_isChecked; // Состояние переключателя
    int m_radius; // Радиус индикатора
    QPropertyAnimation* m_animation; // Анимация индикатора
    int m_indicatorX; // Положение индикатора по оси X
    QColor m_backgroundColor; // Цвет фона переключателя
    QColor m_circleColor; // Цвет фона переключателя
    Theme m_theme;
};


class ButtonCursor : public QWidget
{
    Q_OBJECT

public:
    ButtonCursor(QWidget* parent, Theme theme);

    QSize sizeHint() const override;
    void setTheme(Theme theme);
    void uploadIconsLight(QIcon light, QIcon lightHover);
    void uploadIconsDark(QIcon dark, QIcon darkHover);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;
    void hoverEnter(QHoverEvent* event);
    void hoverLeave(QHoverEvent* event);
    void hoverMove(QHoverEvent* event);
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    QIcon m_iconLight;
    QIcon m_iconDark;
    QIcon m_hoverIconLight;
    QIcon m_hoverIconDark;
    QIcon m_currentIcon;
    Theme m_theme;
};

class AvatarIcon : public QWidget
{
    Q_OBJECT

public:
    AvatarIcon(QWidget* parent, int x, int y, int iconSize, bool needHover, Theme theme)
        : QWidget(parent), m_iconSize(iconSize, iconSize), m_size(iconSize), m_needHover(needHover), m_theme(theme)
    {
        setFixedSize(50, 50);
        setGeometry(x, y, iconSize, iconSize);
        setAttribute(Qt::WA_Hover); // Включаем отслеживание hover-событий
    }

    QSize sizeHint() const override { return QSize(32, 32); }

    void setTheme(Theme theme);

    void setIcon(const QIcon& icon)
    {
        m_icon = icon;
        update();
    }

    void setIconSize(QSize size)
    {
        m_iconSize = size;
        update();
    }

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;


    bool event(QEvent* event) override
    {
        switch (event->type())
        {
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

    void hoverEnter(QHoverEvent* event)
    {
        m_hovered = true;
        update();
    }

    void hoverLeave(QHoverEvent* event)
    {
        m_hovered = false;
        update();
    }

    void hoverMove(QHoverEvent* event)
    {
        update();
    }

    void mouseReleaseEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
    }

private:
    Theme m_theme;
    QIcon m_icon;

    QSize m_iconSize;
    int   m_size;
    bool m_hovered = false;
    bool m_needHover = false;
};


class ButtonIcon : public QWidget
{
    Q_OBJECT

public:
    ButtonIcon(QWidget* parent, int x, int y);

    QSize sizeHint() const override;
    void setTheme(Theme theme);
    void uploadIconsLight(QIcon light, QIcon lightHover);
    void uploadIconsDark(QIcon dark, QIcon darkHover);
    void setIconSize(QSize size);

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent* event) override;
    bool event(QEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void updateIcon();
    void hoverEnter(QHoverEvent* event);
    void hoverLeave(QHoverEvent* event);
    void hoverMove(QHoverEvent* event);

    QIcon m_iconLight;
    QIcon m_iconDark;
    QIcon m_hoverIconLight;
    QIcon m_hoverIconDark;
    QIcon m_currentIcon;
    QSize m_iconSize{ 32, 32 };
    Theme m_theme;
    bool m_hovered = false;
};


class RoundIconButton : public QWidget
{
    Q_OBJECT

public:
    RoundIconButton(QWidget* parent)
        : QWidget(parent)
    {
        setFixedSize(50, 50);
        setMouseTracking(true);
        setAttribute(Qt::WA_Hover);
    }

    QSize sizeHint() const override { return QSize(50, 50); }

    void setTheme(Theme theme);


protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(Qt::gray);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(rect().adjusted(5, 5, -5, -5));

        QPixmap pixmap = m_currentIcon.pixmap(32, 32);
        int x = (width() - pixmap.width()) / 2;
        int y = (height() - pixmap.height()) / 2;
        painter.drawPixmap(x, y, pixmap);
    }

    bool event(QEvent* event) override
    {
        switch (event->type())
        {
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

    void hoverEnter(QHoverEvent* event)
    {
        m_currentIcon = m_hoverIcon;
        update();
    }

    void hoverLeave(QHoverEvent* event)
    {
        m_currentIcon = m_normalIcon;
        update();
    }

    void hoverMove(QHoverEvent* event)
    {
        update();
    }

    void mouseReleaseEvent(QMouseEvent* event) override
    {
        if (event->button() == Qt::LeftButton) {
            emit clicked();
        }
    }

signals:
    void clicked();

private:
    QIcon m_normalIcon;
    QIcon m_hoverIcon;
    QIcon m_currentIcon;
};