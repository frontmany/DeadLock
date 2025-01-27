#include "buttons.h"
#include "mainwindow.h"


ButtonCursor::ButtonCursor(QWidget* parent, Theme theme)
    : QWidget(parent),  m_theme(theme){
    setAttribute(Qt::WA_Hover);
}

QSize ButtonCursor::sizeHint() const
{
    return QSize(50, 50);
}

void ButtonCursor::setTheme(Theme theme)
{
    m_theme = theme;
    update();
    
}

void ButtonCursor::uploadIconsLight(QIcon light, QIcon lightHover)
{
    m_iconLight = light;
    m_hoverIconLight = lightHover;
    m_currentIcon = m_iconLight; // Устанавливаем иконку по умолчанию
}

void ButtonCursor::uploadIconsDark(QIcon dark, QIcon darkHover)
{
    m_iconDark = dark;
    m_hoverIconDark = darkHover;
    m_currentIcon = m_iconDark; // Устанавливаем иконку по умолчанию
}

void ButtonCursor::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(Qt::transparent);
    painter.setPen(Qt::NoPen);
    painter.drawRect(rect());

    QPixmap pixmap = m_currentIcon.pixmap(32, 32);
    int x = (width() - pixmap.width()) / 2;
    int y = (height() - pixmap.height()) / 2;
    painter.drawPixmap(x, y, pixmap);
}

bool ButtonCursor::event(QEvent* event)
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

void ButtonCursor::hoverEnter(QHoverEvent* event)
{
    setCursor(Qt::PointingHandCursor); 
    update();
}

void ButtonCursor::hoverLeave(QHoverEvent* event)
{
    unsetCursor(); 
    update();
}

void ButtonCursor::hoverMove(QHoverEvent* event)
{
    update();
}

void ButtonCursor::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}




ButtonIcon::ButtonIcon(QWidget* parent, int x, int y)
    : QWidget(parent)
{
    m_theme = DARK;
    setFixedSize(x, y);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
}

void ButtonIcon::setTheme(Theme theme) {
    m_theme = theme;

    if (theme == DARK) {
        m_currentIcon = m_iconDark;
        update();
    }
    else {
        m_currentIcon = m_iconLight;
        update();
    }
}

void ButtonIcon::uploadIconsLight(QIcon light, QIcon lightHover) {
    m_hoverIconLight = lightHover;
    m_iconLight = light;
}


void ButtonIcon::uploadIconsDark(QIcon dark, QIcon darkHover) {
    m_hoverIconDark = darkHover;
    m_iconDark = dark;
}

void ButtonIcon::hoverEnter(QHoverEvent* event)
{
    if (m_theme == DARK) {
        m_currentIcon = m_hoverIconDark;
        update();
    }
    else {
        m_currentIcon = m_hoverIconLight;
        update();
    }

}

void ButtonIcon::hoverLeave(QHoverEvent* event)
{
    if (m_theme == DARK) {
        m_currentIcon = m_iconDark;
        update();
    }
    else {
        m_currentIcon = m_iconLight;
        update();
    }
}


void ButtonIcon::setIconSize(QSize size) {
    setFixedSize(size);
}

void RoundIconButton::setTheme(Theme theme) {
    if (theme == DARK) {
        QIcon icon(":/resources/ChatsWidget/userDark.png");
        QIcon iconHover(":/resources/ChatsWidget/userDarkHover.png");
        m_normalIcon = icon;
        m_hoverIcon = iconHover;
        m_currentIcon = m_normalIcon;
        update();
    }
    else {
        QIcon icon(":/resources/ChatsWidget/userLight.png");
        QIcon iconHover(":/resources/ChatsWidget/userLightHover.png");
        m_normalIcon = icon;
        m_hoverIcon = iconHover;
        m_currentIcon = m_normalIcon;
        update();
    }
}
