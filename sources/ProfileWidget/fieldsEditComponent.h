#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QFileDialog>
#include <QPropertyAnimation>
#include <QRegularExpressionValidator>
#include <QGraphicsOpacityEffect>
#include <QWheelEvent>

class EditDialogComponent;
class ProfileEditorWidget;
class ConfigManager;
class ButtonIcon;
class Client;
class Photo;
enum  Theme;

struct StyleFieldsEditComponent {
    StyleFieldsEditComponent();
    QString DarkLabelStyle;
    QString LightLabelStyle;

    QString DarkButtonStyleBlue;
    QString LightButtonStyleBlue;

    QString buttonStyleGrayDark;
    QString buttonStyleGrayLight;

    QString buttonToChoosePhotoStyleDark;
    QString buttonToChoosePhotoStyleLight;

    QString buttonToChangePasswordStyleDark;
    QString buttonToChangePasswordStyleLight;

    QString DarkLineEditStyle;
    QString LightLineEditStyle;
};

class FieldsEditComponent : public QWidget {
    Q_OBJECT

public:
    FieldsEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget,  Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme);
    void setTheme(Theme theme);
    void updateAvatar(const Photo& photo);

signals:
    void logoutRequested();

public slots:
    void setErrorText(const QString& text);

private:
    StyleFieldsEditComponent*  m_style;
    Theme                      m_theme;

    QVBoxLayout*    m_mainVLayout;
    QHBoxLayout*    m_save_cancel_buttonsHLayout;
    QHBoxLayout*    m_photo_password_buttonsHLayout;

    QLabel*         m_avatar_label;
    QPushButton*    m_change_photo_button;

    QLabel*         m_login_label;
    QLineEdit*      m_login_edit;

    QLabel*         m_name_label;
    QLineEdit*      m_name_edit;

    QLabel* m_error_label;

    QPushButton*    m_password_button;
    QPushButton*    m_save_button;
    QPushButton*    m_cancel_button;
    ButtonIcon*     m_logoutButton;

    ProfileEditorWidget* m_profile_editor_widget;
    std::shared_ptr<ConfigManager> m_config_manager;
    Client* m_client;
};