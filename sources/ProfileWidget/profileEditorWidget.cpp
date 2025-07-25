#include "theme.h"
#include "profileEditorWidget.h"
#include "passwordEditorComponent.h"
#include "fieldsEditComponent.h"
#include "photoEditComponent.h"
#include "chatsListComponent.h"
#include "client.h"
#include "photo.h"
#include "utility.h"
#include "chatsWidget.h"
#include "mainwindow.h"
#include "configManager.h"


ProfileEditorWidget::ProfileEditorWidget(QWidget* parent, ChatsListComponent* chatsListComponent, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme)
    : QWidget(parent), m_client(client), m_theme(theme), m_chats_list_component(chatsListComponent), m_config_manager(configManager) {
    m_fields_edit_component = new FieldsEditComponent(parent, this, client, m_config_manager, m_theme);
    m_fields_edit_component->setMaximumSize(400, 500);

    m_photo_edit_component = new PhotoEditComponent(parent, this, client, m_config_manager, m_theme);
    m_photo_edit_component->hide();

    m_password_edit_component = new PasswordEditComponent(this, this, m_client, m_theme);

    m_mainVLayout = new QVBoxLayout();
    m_mainVLayout->addWidget(m_fields_edit_component);
    m_mainVLayout->setContentsMargins(20, 20, 20, 20);

    m_mainHLayout = new QHBoxLayout;
    m_mainHLayout->addLayout(m_mainVLayout);
    m_mainHLayout->setAlignment(Qt::AlignCenter);

    setLayout(m_mainHLayout);
    setFieldsEditor();
}

void ProfileEditorWidget::setPhotoEditor() {
    setMaximumWidth(800);
    setMaximumHeight(1200);

    m_mainVLayout->removeWidget(m_fields_edit_component);
    m_fields_edit_component->hide();
    m_mainVLayout->addWidget(m_photo_edit_component);
    m_photo_edit_component->show();

    qreal scaleFactor = utility::getDeviceScaleFactor();
    if (scaleFactor > 1 && scaleFactor <= 1.25) {
        setFixedHeight(740);
        setMaximumWidth(700);
    }
    else if (scaleFactor > 1.25 && scaleFactor <= 1.5) {
        setFixedHeight(630);
        setMaximumWidth(560);
    }
    else {
        setMaximumWidth(800);
        m_photo_edit_component->setFixedHeight(840);
    }
}

void ProfileEditorWidget::setFieldsEditor() {
    setMaximumSize(420, 600);

    if (utility::getDeviceScaleFactor() >= 1.5) {
        setFixedHeight(utility::getScaledSize(730));
    }
    else if (utility::getDeviceScaleFactor() >= 1.25) {
        setFixedHeight(utility::getScaledSize(620));
    }
    else {
        setFixedHeight(utility::getScaledSize(500));
    }

    m_fields_edit_component->setMaximumSize(400, utility::getScaledSize(500));
    if (utility::getDeviceScaleFactor() >= 1.5) {
        m_fields_edit_component->setMaximumSize(400, utility::getScaledSize(600));
    }
    m_mainVLayout->removeWidget(m_photo_edit_component);
    m_photo_edit_component->hide();
    m_password_edit_component->hide();
    m_mainVLayout->addWidget(m_fields_edit_component);
    m_fields_edit_component->show();
    m_fields_edit_component->update();
}

void ProfileEditorWidget::setPasswordEditor() {
    setMaximumSize(420, 600);

    setFixedHeight(utility::getScaledSize(500));

    m_password_edit_component->setMaximumSize(400, utility::getScaledSize(500));
    m_mainVLayout->removeWidget(m_fields_edit_component);
    m_fields_edit_component->hide();
    m_mainVLayout->addWidget(m_password_edit_component);

    if (m_password_edit_component->isVerificationStage() == false)
        m_password_edit_component->showCurrentPasswordInput();

    m_password_edit_component->show();
}

void ProfileEditorWidget::setTheme(Theme theme) {
    m_theme = theme;
    if (m_fields_edit_component != nullptr) {
        m_fields_edit_component->setTheme(theme);
    }
    if (m_photo_edit_component != nullptr) {
        m_photo_edit_component->setTheme(theme);
    }
    if (m_password_edit_component != nullptr) {
        m_password_edit_component->setTheme(theme);
    }

    update();
}

void ProfileEditorWidget::close() {
    m_chats_list_component->closeEditUserDialogWidnow();
    m_dialog->accept();
}

void ProfileEditorWidget::updateAvatar(const Photo& photo) {
    m_fields_edit_component->updateAvatar(photo);
    m_chats_list_component->setAvatar(photo);
}

void ProfileEditorWidget::setName(const std::string& name) {
    m_fields_edit_component->setName(name);
}

void ProfileEditorWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_theme == DARK) {
        painter.setBrush(QColor(36, 36, 36));
    }
    if (m_theme == LIGHT) {
        painter.setBrush(QColor(229, 228, 226));
    }

    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 20, 20);
}
