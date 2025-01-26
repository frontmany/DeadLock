#include "buttons.h"
#include "mainwindow.h"

ButtonIcon::ButtonIcon(QWidget* parent)
    : QWidget(parent)
{
    m_theme = DARK;
    setFixedSize(50, 50);
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
