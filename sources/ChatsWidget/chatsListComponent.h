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
class OverlayWidget;

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
    QString DarkNoConnectionLabelStyle;
    QString LightNoConnectionLabelStyle;
    QString updateButtonDark;
    QString updateButtonLight;
    QString updateButtonInProgressDark;
    QString updateButtonInProgressLight;
};

class ChatsListComponent : public QWidget {
    Q_OBJECT

public:
    ChatsListComponent(QWidget* parent, ChatsWidget* chatsWidget, Theme theme, bool isHidden);
    ~ChatsListComponent() = default;

    void setTheme(Theme theme);
    void setAbleToCreateChatFlag(bool fl) { m_ableToCreateChat = fl; }
    void removeComponent(const QString& loginOfRemovedComponent);

    std::vector<ChatComponent*>& getChatComponentsVec() { return m_vec_chatComponents; }
    AddChatDialogComponent* getAddChatDialogComponent() { return m_chatAddDialog; }
    FriendSearchDialogComponent* getFriendSearchDialogComponent() { return m_friend_search_dialog; }
    ProfileEditorWidget* getProfileEditorWidget() { return m_profile_editor_widget; }

    bool getIsHidden() { return m_is_hidden; }

    QLineEdit* getSearchLineEdit() { return m_searchLineEdit; }

    ChatsWidget* getChatsWidget() const;
    

signals:
    void sendCreateChatData(QString login);
    void sendChangeTheme();


public slots:
    void showNoConnectionLabel();
    void showServerOfflineLabel();
    void showUpdateButton();

    void addChatComponent(Theme theme, Chat* chat, bool isSelected);

    void openAddChatDialog();
    void closeAddChatDialog();

    void closeEditUserDialogWidnow();
    void openEditUserDialogWidnow();

    void onHideButtonToggled(bool checked, bool isNeedToToggleButton);
    void disableHiddenMode(Client* client);
    void enableHiddenMode(Client* client);
    
    void openHiddenButtonHintDialog();

    void activateProfileButton();
    void disableProfileButton();
    void receiveCreateChatData(QString login);
    void popUpComponent(ChatComponent* comp);

    void setAvatar(const Photo& photo);
    void setNameFieldInProfileEditorWidget(const std::string& name);
    void setAvatarInProfileEditorWidget(const Photo& photo);
protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void toSendChangeTheme(bool fl);


private:
    void closeHiddenButtonHintDialog();
    void showProfileDialog();
    void updateHideButton();

private:
    StyleChatsListComponent*    style;
    Theme                       m_theme;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_profileHLayout;
    QHBoxLayout* m_contentsHLayout;

    ToggleSwitch*       m_darkModeSwitch;
    ChatsWidget*        m_chatsWidget;
    QScrollArea*        m_scrollArea;
    QWidget*            m_containerWidget;
    QDialog*            m_hintDialog = nullptr;
    OverlayWidget*      m_hintOverlay;
    QVBoxLayout*        m_containerVLayout;
    QLineEdit*          m_searchLineEdit;

    AvatarIcon*         m_profileButton;
    ButtonIcon*         m_newChatButton;

    QPushButton*        m_hideButton;
    QPushButton*        m_updateButton;
    QLabel*             m_noConnectionLabel;
    bool                m_is_hidden;

    QTimer* m_search_timer;
    FriendSearchDialogComponent* m_friend_search_dialog;
    AddChatDialogComponent* m_chatAddDialog;

    ChatsWidget* m_chats_widget;
    ProfileEditorWidget* m_profile_editor_widget;

    std::vector<ChatComponent*> m_vec_chatComponents;

    bool m_isChatAddDialog = false;
    bool m_ableToCreateChat = true;
    bool m_isUpdateLoading = false;

    QIcon m_hiddenIconDark = QIcon(":/resources/ChatsWidget/hiddenStateButtonDark.png");
    QIcon m_hiddenIconDarkHover = QIcon(":/resources/ChatsWidget/hiddenStateButtonDarkHover.png");
    QIcon m_hiddenIconLight = QIcon(":/resources/ChatsWidget/hiddenStateButtonLight.png");
    QIcon m_hiddenIconLightHover = QIcon(":/resources/ChatsWidget/hiddenStateButtonLightHover.png");

    QIcon m_visibleIconDark = QIcon(":/resources/ChatsWidget/visibleStateButtonDark.png");
    QIcon m_visibleIconDarkHover = QIcon(":/resources/ChatsWidget/visibleStateButtonDarkHover.png");
    QIcon m_visibleIconLight = QIcon(":/resources/ChatsWidget/visibleStateButtonLight.png");
    QIcon m_visibleIconLightHover = QIcon(":/resources/ChatsWidget/visibleStateButtonLightHover.png");
};