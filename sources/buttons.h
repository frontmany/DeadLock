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
    AvatarIcon(QWidget* parent, int x, int y, int iconSize, bool needHover)
        : QWidget(parent), m_iconSize(iconSize, iconSize), m_size(iconSize), m_needHover(needHover)
    {
        setFixedSize(50, 50);
        setGeometry(x, y, iconSize, iconSize);
        setAttribute(Qt::WA_Hover); // Включаем отслеживание hover-событий
    }

    QSize sizeHint() const override { return QSize(32, 32); }

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
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // Если кнопка в состоянии hover, рисуем мягкую белую подсветку ПОД иконкой
        if (m_hovered && m_needHover)
        {
            QRectF circleRect = rect().adjusted(2, 2, -2, -2); // Увеличиваем область подсветки (меньше отступ)

            // Создаем радиальный градиент для эффекта свечения
            QRadialGradient gradient(circleRect.center(), circleRect.width() / 1.5); // Увеличиваем радиус градиента
            gradient.setColorAt(0, QColor(255, 255, 255, 150)); // Ярче и менее прозрачный в центре
            gradient.setColorAt(0.7, QColor(255, 255, 255, 50)); // Плавнее спад прозрачности
            gradient.setColorAt(1, QColor(255, 255, 255, 0));   // Полностью прозрачный на краях

            // Настраиваем кисть с градиентом
            painter.setBrush(gradient);
            painter.setPen(Qt::NoPen);

            // Рисуем круг с градиентом
            painter.drawEllipse(circleRect);
        }

        // Рисуем круглый фон (если нужен, иначе можно убрать)
        QRectF circleRect = rect().adjusted(5, 5, -5, -5); // Круг с отступом
        painter.setBrush(Qt::transparent);
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(circleRect);

        // Рисуем иконку в центре (поверх всего)
        if (!m_icon.isNull()) // Проверяем, что иконка загружена
        {
            QPixmap pixmap = m_icon.pixmap(m_size, m_size);
            int x = (width() - pixmap.width()) / 2;
            int y = (height() - pixmap.height()) / 2;
            painter.drawPixmap(x, y, pixmap);
        }
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

    QSize sizeHint() const override { return QSize(50, 50); }
    void setTheme(Theme theme);
    void uploadIconsLight(QIcon light, QIcon lightHover);
    void uploadIconsDark(QIcon dark, QIcon darkHover);
    void setIconSize(QSize size);

protected:
    void paintEvent(QPaintEvent* event) override
    {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setBrush(Qt::transparent);
        painter.setPen(Qt::NoPen);
        painter.drawRect(rect().adjusted(0, 0, 0, 0));

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

    void hoverEnter(QHoverEvent* event);
    void hoverLeave(QHoverEvent* event);

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
    QIcon m_iconLight;
    QIcon m_iconDark;
    QIcon m_hoverIconLight;
    QIcon m_hoverIconDark;
    QIcon m_currentIcon;
    Theme m_theme;
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