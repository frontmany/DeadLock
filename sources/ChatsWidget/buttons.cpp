#include "buttons.h"
#include "mainwindow.h"

void AvatarIcon::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Если кнопка в состоянии hover, рисуем мягкую белую подсветку ПОД иконкой
    if (m_hovered && m_needHover)
    {
        QRectF circleRect = rect().adjusted(2, 2, -2, -2); // Увеличиваем область подсветки (меньше отступ)

        // Создаем радиальный градиент для эффекта свечения

        QRadialGradient gradient(circleRect.center(), circleRect.width() / 1.5);
        if (m_theme == Theme::LIGHT) {
            gradient.setColorAt(0, QColor(63, 139, 252, 150)); // Ярче и менее прозрачный в центре
            gradient.setColorAt(0.7, QColor(63, 139, 252, 50)); // Плавнее спад прозрачности
            gradient.setColorAt(1, QColor(63, 139, 252, 0));   // Полностью прозрачный на краях
        }
        else {
            gradient.setColorAt(0, QColor(255, 255, 255, 150)); // Ярче и менее прозрачный в центре
            gradient.setColorAt(0.7, QColor(255, 255, 255, 50)); // Плавнее спад прозрачности
            gradient.setColorAt(1, QColor(255, 255, 255, 0));   // Полностью прозрачный на краях
        }

        // Настраиваем кисть с градиентом
        painter.setBrush(gradient);
        painter.setPen(Qt::NoPen);

        // Рисуем круг с градиентом
        painter.drawEllipse(circleRect);
    }

    // Рисуем круглый фон (если нужен, иначе можно убрать)
    QRectF circleRect = rect().adjusted(0, 0, 0, 0); // Круг с отступом
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


ToggleSwitch::ToggleSwitch(QWidget* parent, Theme theme)
    : QWidget(parent), m_isChecked(false), m_radius(20), m_indicatorX(5) {
    setFixedSize(52, 30); 
    m_theme = theme;

    if (m_theme == DARK) {
        m_circleColor = QColor(102, 102, 102);
        m_backgroundColor = QColor(52, 52, 52);

    }
    else {
        m_circleColor = QColor(204, 234, 255);
        m_backgroundColor = QColor(212, 212, 212);
    }

    m_animation = new QPropertyAnimation(this, "indicatorX");
    m_animation->setDuration(200); // Длительность анимации
    m_animation->setEasingCurve(QEasingCurve::InOutQuad); // Кривая анимации
}

bool ToggleSwitch::isChecked() const {
    return m_isChecked;
}

void ToggleSwitch::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setBrush(m_backgroundColor);
    painter.drawRoundedRect(0, 0, width(), height(), 15, 15);

    painter.setBrush(m_circleColor);
    painter.drawEllipse(m_indicatorX, 5, 20, 20);
}

void ToggleSwitch::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        setTheme(m_theme == DARK ? LIGHT : DARK);
        m_isChecked = !m_isChecked;
        updateAnimation();
        emit toggled(m_isChecked);
    }
}

void ToggleSwitch::updateAnimation() {
    if (m_isChecked) {
        m_animation->setStartValue(m_indicatorX);
        m_animation->setEndValue(width() - m_radius - 5);
    }
    else {
        m_animation->setStartValue(m_indicatorX);
        m_animation->setEndValue(5); 
    }

    m_animation->start(); 

    connect(m_animation, &QPropertyAnimation::valueChanged, [this](const QVariant& value) {
        m_indicatorX = value.toInt();
        update(); 
        });
}

void ToggleSwitch::setTheme(Theme theme) {
    m_theme = theme;
    if (m_theme == DARK) {
        m_circleColor = QColor(102, 102, 102);
        m_backgroundColor = QColor(52, 52, 52);

    }
    else {
        m_circleColor = QColor(255, 255, 255);
        m_backgroundColor = QColor(212, 212, 212);
    }
    update();
}

QColor ToggleSwitch::backgroundColor() const {
    return m_backgroundColor;
}

void ToggleSwitch::setBackgroundColor(const QColor& color) {
    m_backgroundColor = color;
    update(); // Обновляем виджет
}

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
