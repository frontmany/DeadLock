#pragma once
#include <QWidget>
#include <vector>
#include <unordered_map>
#include <QVBoxLayout>
#include <QPixmap>
#include <QScrollArea>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>

enum Theme;
class ChatsListComponent;
class FriendSearchDialogComponent;
class FriendInfo;
class Chat;
class ButtonIcon;
class AvatarIcon;




struct StyleFriendComponent {
    StyleFriendComponent();

    QString widgetStyleDark;
    QString widgetStyleLight;

    QString labelStyleDark;
    QString labelStyleLight;

    QString avatarHoverStyleDark;
    QString avatarHoverStyleLight;
};

struct StyleFriendSearchDialogComponent {
    StyleFriendSearchDialogComponent();

    QString addButtonStyle;
    QString DarkButtonStyleBlue;
    QString LightButtonStyleBlue;
};




class FriendComponent : public QWidget {
    Q_OBJECT

public:
    FriendComponent(QWidget* parent, 
        FriendSearchDialogComponent* friendSearchDialogComponent, Theme theme);

    void setFriendData(const QString& name, const QPixmap& photo = QPixmap());
    void setTheme(Theme theme);

private:
    void setupUi();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Theme                 m_theme;
    StyleFriendComponent* m_style;
    QVBoxLayout*          m_mainVLayout;
    AvatarIcon*           m_avatar_button;
    QLabel*               m_name_label;

    FriendSearchDialogComponent* m_friend_search_dialog_component;
};




class FriendSearchDialogComponent : public QWidget {
    Q_OBJECT

public:
    FriendSearchDialogComponent(QWidget* parent, ChatsListComponent* chatsListComponent, 
        Theme theme);

    void showDialog();
    void closeDialog();
    void setTheme(Theme theme);

public slots:
    void refreshFriendsList(const std::vector<FriendInfo*>& friendInfoVec);


private:
    void updateFriendsListUI();
    void setupUI();
    void setupScrollArea();

private:

    std::unordered_map<std::string, FriendInfo*> m_suggestions_map;
    StyleFriendSearchDialogComponent*            m_style;
    ChatsListComponent*                          m_chats_list_component;
    Theme                                        m_theme;
    bool                                         m_is_visible;

    QVBoxLayout* m_mainVLayout;
    QScrollArea* m_scrollArea;       
    QWidget*     m_scrollContent;
    QHBoxLayout* m_scrollHLayout;  
};