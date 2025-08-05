#pragma once
#include <QWidget>
#include <vector>
#include <unordered_map>
#include <QVBoxLayout>
#include <QPixmap>
#include <QScrollArea>
#include <QLabel>
#include <iostream>
#include <QEvent>
#include <QMouseEvent>
#include <QHoverEvent>
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
class Avatar;

struct StyleFriendComponent {
    StyleFriendComponent();

    QString labelStyleDark;
    QString labelStyleLight;

    QString widgetStyleDark;
    QString widgetStyleLight;

    QString avatarHoverStyleDark;
    QString avatarHoverStyleLight;
};

struct StyleFriendSearchDialogComponent {
    StyleFriendSearchDialogComponent();

    QString labelStyleDark;
    QString labelStyleLight;

    QString addButtonStyle;

    QString DarkButtonStyleBlue;
    QString LightButtonStyleBlue;

    QString darkSlider;
    QString lightSlider;
};

class FriendComponent : public QWidget {
    Q_OBJECT

public:
    FriendComponent(QWidget* parent, 
        FriendSearchDialogComponent* friendSearchDialogComponent, Theme theme);

    void setFriendData(const QString& name, const QString& loginHash, const QPixmap& photo = QPixmap());
    void setTheme(Theme theme);

signals:
    void sendData(const QString loginHash);

private slots:
    void slotToSendData();

private:
    Theme                 m_theme;
    StyleFriendComponent* m_style;
    QVBoxLayout*          m_mainVLayout;
    AvatarIcon*           m_avatar_button;
    QLabel*               m_name_label;

    QString m_login_hash;
    bool m_hovered = false;
    FriendSearchDialogComponent* m_friend_search_dialog_component;
};




class FriendSearchDialogComponent : public QWidget {
    Q_OBJECT

public:
    FriendSearchDialogComponent(QWidget* parent, ChatsListComponent* chatsListComponent, 
        Theme theme);

    void showDialog(int size);
    void closeDialog();
    void setTheme(Theme theme);

public slots:
    void supplyNewFriendsList(const std::vector<FriendInfo*>& friendInfoVec);
    void onFriendComponentClicked(const QString& loginHash);
    void supplyAvatar(Avatar* avatar, std::string loginHash);

private:
    void clear();
    void showExistingChat(const std::string& loginHash);
    void addNewChatAndShow(const std::string& loginHash, FriendInfo* friendInfo, const std::string& avatarNewPath);
    void showNoUsersFoundLabel();
    void addToFriendsListUI(const std::string& loginHash);
    void clearComponentsMapAndUI();
    void deduceAvatarAndSetDataTo(FriendComponent* friendComp, FriendInfo* friendInfo, bool isPhotoReceived);


private:

    std::unordered_map<std::string, std::pair<FriendInfo*, bool>> m_suggestions_map;
    std::unordered_map<std::string, FriendComponent*> m_components_map;

    StyleFriendSearchDialogComponent*            m_style;
    ChatsListComponent*                          m_chats_list_component;
    Theme                                        m_theme;

    uint32_t countUsersWithPhoto = 0;
    uint32_t countUsersWithPhotoReceived = 0;

    QLabel*      m_not_found_label = nullptr;
    QVBoxLayout* m_mainVLayout;
    QScrollArea* m_scrollArea;       
    QWidget*     m_scrollContent;
    QHBoxLayout* m_scrollHLayout;  
};