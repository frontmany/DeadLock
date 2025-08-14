#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QPainterPath>
#include <QMovie>

class ButtonIcon;
class AvatarIcon;
class MessagingAreaComponent;
enum Theme;

struct StyleChatHeaderComponent {
    StyleChatHeaderComponent();

    QString deepBlueLabelStyle;

    QString blueLabelStyle;
    QString lightLabelStyle;

    QString darkLabelStyle;
    QString grayLabelStyle;
};

class ChatHeaderComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatHeaderComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Theme theme, QString name, QString lastSeen, QPixmap avatar);
  
    const QString& getLastSeen() const { return m_lastSeenLabel->text(); }

public slots:
    void swapLastSeenLabel(bool isTyping);
    void setLastSeen(const QString& lastSeen);
    void setAvatar(const QPixmap& pixMap);
    void setName(const QString& name);

protected:
    void paintEvent(QPaintEvent* event) override;

public:
    void setTheme(Theme theme);


private:
    StyleChatHeaderComponent* style;
    Theme m_theme;
    QColor m_backColor;

    MessagingAreaComponent* m_messaging_area_component;

    AvatarIcon*     m_leftIcon;
    ButtonIcon*     m_rightButton;

    QWidget* m_typingContainer = nullptr;

    QHBoxLayout* m_mainLayout;
    QVBoxLayout* m_rightLayout;

    QLabel*     m_nameLabel;
    QLabel*     m_lastSeenLabel = nullptr;
};