#include "profileEditorWidget.h"
#include "fieldsEditComponent.h"
#include "photoEditComponent.h"
#include "chatsListComponent.h"
#include "client.h"
#include "chatsWidget.h"

ProfileEditorWidget::ProfileEditorWidget(QWidget* parent, ChatsListComponent* chatsListComponent, Client* client, Theme theme)
    : QWidget(parent), m_client(client), m_theme(theme), m_chats_list_component(chatsListComponent) {
    m_fields_edit_component = new FieldsEditComponent(parent, this, client, m_theme);
    m_fields_edit_component->setMaximumSize(400, 500);

    m_photo_edit_component = new PhotoEditComponent(parent, this, client, m_theme);
    m_photo_edit_component->hide();

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
    setFixedHeight(700);
    m_mainVLayout->removeWidget(m_fields_edit_component);
    m_fields_edit_component->hide();
    m_mainVLayout->addWidget(m_photo_edit_component);
    m_photo_edit_component->show();
}

void ProfileEditorWidget::setFieldsEditor() {
    setFixedHeight(500);
    m_fields_edit_component->setMaximumSize(400, 500);
    m_mainVLayout->removeWidget(m_photo_edit_component);
    m_photo_edit_component->hide();
    m_mainVLayout->addWidget(m_fields_edit_component);
    m_fields_edit_component->show();
    m_fields_edit_component->update();
}

void ProfileEditorWidget::onImagePicker() {
    setFixedHeight(800);
}

void ProfileEditorWidget::setTheme(Theme theme) {
    m_theme = theme;
    if (m_fields_edit_component != nullptr) {
        m_fields_edit_component->setTheme(theme);
    }
    if (m_photo_edit_component != nullptr) {
        m_photo_edit_component->setTheme(theme);
    }

    update();
}

void ProfileEditorWidget::close() {
    m_chats_list_component->setIsEditDialogFlag(false);
    m_chats_list_component->closeEditUserDialogWidnow();
}

void ProfileEditorWidget::updateAvatar(const Photo& photo) {
    m_fields_edit_component->updateAvatar();
    m_chats_list_component->SetAvatar(photo);
}

void ProfileEditorWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (m_theme == DARK) {
        painter.setBrush(QColor(26, 26, 26, 200));
    }
    if (m_theme == LIGHT) {
        painter.setBrush(QColor(212, 212, 212, 200));
    }

    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 20, 20);
}