#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "request.h"


class ButtonIcon;
enum Theme;

class ChatHeaderComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatHeaderComponent(QWidget* parent, Theme theme, QString name, QString lastSeen, QPixmap avatar);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Theme m_theme;
    QColor m_backcolor;
    
    ButtonIcon*     m_leftIcon;
    ButtonIcon*     m_rightButton;

    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_rightLayout;

    QLabel*     m_nameLabel;
    QLabel*     m_lastSeenLabel;
};