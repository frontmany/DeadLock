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
class Client;
class Photo;
enum  Theme;

class ProfileEditorWidget : public QWidget {
    Q_OBJECT

public:
    explicit ProfileEditorWidget(QWidget* parent, ChatsListComponent* chatsListComponent, Client* client, Theme theme);
    void updateAvatar(const Photo& photo);
    void setTheme(Theme theme);

public slots:
    void onImagePicker();
    void setPhotoEditor();
    void setFieldsEditor();
    void close();
    
protected:
    void paintEvent(QPaintEvent* event) override;

private:
    Theme               m_theme;
    Client*             m_client;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_mainHLayout;

    ChatsListComponent* m_chats_list_component = nullptr;
    FieldsEditComponent* m_fields_edit_component = nullptr;
    PhotoEditComponent* m_photo_edit_component = nullptr;
};