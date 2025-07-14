#pragma once
#include <QDialog>
#include <QPainter>
#include <QLabel>
#include <QPushButton>
#include <QPainterPath>
#include <QVBoxLayout>

class FieldsEditComponent;
class ChatsListComponent;
class PhotoEditComponent;
class PasswordEditComponent;
class ConfigManager;
class Client;
class Photo;
enum  Theme;

class ProfileEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProfileEditorWidget(QWidget* parent, ChatsListComponent* chatsListComponent, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme);
    void updateAvatar(const Photo& photo);
    void setTheme(Theme theme);
    void setName(const std::string& name);
    void setDialog(QDialog* dialog) { m_dialog = dialog; }

    PasswordEditComponent* getPasswordEditComponent() { return m_password_edit_component; }
    FieldsEditComponent* getFieldsEditComponent() { return m_fields_edit_component; }

public slots:
    void setPhotoEditor();
    void setFieldsEditor();
    void setPasswordEditor();
    void close();
    
protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Theme               m_theme;
    Client*             m_client;
    std::shared_ptr<ConfigManager> m_config_manager;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_mainHLayout;
    QDialog* m_dialog = nullptr;

    ChatsListComponent* m_chats_list_component = nullptr;
    FieldsEditComponent* m_fields_edit_component = nullptr;
    PhotoEditComponent* m_photo_edit_component = nullptr;
    PasswordEditComponent* m_password_edit_component = nullptr;
};