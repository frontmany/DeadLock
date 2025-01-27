#pragma once

#include <QWidget>
#include <QPixmap>
#include <QIcon>
#include <QHoverEvent>
#include <QMouseEvent>
#include <QEvent>
#include <QPainter>



enum Theme;

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