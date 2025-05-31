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
#include <QLabel>
#include <QHoverEvent>
#include <QObject>
#include <QMouseEvent>
#include <QEvent>

enum Theme;
class AvatarIcon;
class AddChatDialogComponent;
class ProfileEditorWidget;
class MessagingAreaComponent;
class FriendSearchDialogComponent;
class ButtonIcon;
class ToggleSwitch;
class ChatComponent;
class Client;
class Photo;
class Chat;
class ChatsWidget;

struct StyleChatsListComponent {
    StyleChatsListComponent();
    QString DarkSlider;
    QString LightSlider;
    QString DarkLineEditStyle;
    QString LightLineEditStyle;
    QString TransparentButtonStyle;
    QString DialogStyle;
    QString DarkHideButton;
    QString LightHideButton;
};

class ChatsListComponent : public QWidget {
    Q_OBJECT

public:
    ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme, bool isHidden);
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

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void toSendChangeTheme(bool fl);


private:
    void updateHideButton();

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

    QPushButton*        m_hideButton;
    bool                m_is_hidden;

    QTimer*                      m_search_timer;
    FriendSearchDialogComponent* m_friend_search_dialog;
    AddChatDialogComponent*      m_chatAddDialog;

    ChatsWidget*         m_chats_widget;
    ProfileEditorWidget* m_profile_editor_widget;

    std::vector<ChatComponent*> m_vec_chatComponents;

    bool m_isChatAddDialog = false;
    bool m_isEditDialog = false;
    bool m_ableToCreateChat = true;

    QIcon m_hiddenIconDark = QIcon(":/resources/ChatsWidget/hiddenStateButtonDark.png");
    QIcon m_hiddenIconDarkHover = QIcon(":/resources/ChatsWidget/hiddenStateButtonDarkHover.png");
    QIcon m_hiddenIconLight = QIcon(":/resources/ChatsWidget/hiddenStateButtonLight.png");
    QIcon m_hiddenIconLightHover = QIcon(":/resources/ChatsWidget/hiddenStateButtonLightHover.png");

    QIcon m_visibleIconDark = QIcon(":/resources/ChatsWidget/visibleStateButtonDark.png");
    QIcon m_visibleIconDarkHover = QIcon(":/resources/ChatsWidget/visibleStateButtonDarkHover.png");
    QIcon m_visibleIconLight = QIcon(":/resources/ChatsWidget/visibleStateButtonLight.png");
    QIcon m_visibleIconLightHover = QIcon(":/resources/ChatsWidget/visibleStateButtonLightHover.png");
};