#pragma once

#include <QWidget>
#include <vector>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QHoverEvent>
#include <QMouseEvent>
#include <QEvent>
#include "ChatComponent.h"
#include "mainwindow.h"

class AvatarIcon;
class AddChatDialogComponent;
class ProfileEditorWidget;
class MessagingAreaComponent;
class FriendSearchDialogComponent;
class ButtonIcon;
class ToggleSwitch;
class Client;
class ChatsWidget;

struct StyleChatsListComponent {
    StyleChatsListComponent();
    QString DarkSlider;
    QString LightSlider;
    QString DarkLineEditStyle;
    QString LightLineEditStyle;
    QString TransparentButtonStyle;
    QString DialogStyle;
};

class ChatsListComponent : public QWidget {
    Q_OBJECT

public:
    ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme);
    ~ChatsListComponent();

    void setTheme(Theme theme);
    void setAbleToCreateChatFlag(bool fl) { m_ableToCreateChat = fl; }
    void SetAvatar(const Photo& photo);
    void removeComponent(const QString& loginOfRemovedComponent);

    std::vector<ChatComponent*>& getChatComponentsVec() { return m_vec_chatComponents; }
    AddChatDialogComponent* getAddChatDialogComponent() { return m_chatAddDialog; }
    FriendSearchDialogComponent* getFriendSearchDialogComponent() { return m_friend_search_dialog; }
    ProfileEditorWidget* getProfileEditorWidget() { return m_profile_editor_widget; }

    QLineEdit* getSearchLineEdit() { return m_searchLineEdit; }
    void setIsEditDialogFlag(bool isEditDialog) { m_isEditDialog = isEditDialog; }

    ChatsWidget* getChatsWidget() const;
    

signals:
    void sendCreateChatData(QString login);
    void sendChangeTheme();
    void logoutRequested();


public slots:
    void addChatComponent(Theme theme, Chat* chat, bool isSelected);

    void openAddChatDialog();
    void closeAddChatDialog();

    void closeEditUserDialogWidnow();
    void openEditUserDialogWidnow();

    void receiveCreateChatData(QString login);
    void popUpComponent(ChatComponent* comp);
    void loadAvatarFromPC(const std::string& login);

private slots:
    void toSendChangeTheme(bool fl);

private:
    QColor                      m_backgroundColor;
    StyleChatsListComponent*    style;
    Theme                       m_theme;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_profileHLayout;
    QHBoxLayout* m_contentsHLayout;

    QLabel*             m_moon_icon;
    ToggleSwitch*       m_darkModeSwitch;
    ChatsWidget*        m_chatsWidget;
    QScrollArea*        m_scrollArea;
    QWidget*            m_containerWidget;
    QVBoxLayout*        m_containerVLayout;
    QLineEdit*          m_searchLineEdit;

    AvatarIcon*         m_profileButton;
    ButtonIcon*         m_newChatButton;
    ButtonIcon*         m_logoutButton;

    QTimer*                      m_search_timer;
    FriendSearchDialogComponent* m_friend_search_dialog;
    AddChatDialogComponent*      m_chatAddDialog;

    ChatsWidget*         m_chats_widget;
    ProfileEditorWidget* m_profile_editor_widget;

    std::vector<ChatComponent*> m_vec_chatComponents;

    bool m_isChatAddDialog = false;
    bool m_isEditDialog = false;
    bool m_ableToCreateChat = true;
};