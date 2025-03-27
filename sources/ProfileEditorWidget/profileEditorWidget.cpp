#include "profileEditorWidget.h"
#include "fieldsEditComponent.h"
#include "photoEditComponent.h"
#include "chatsListComponent.h"
#include "client.h"
#include "chatsWidget.h"

ProfileEditorWidget::ProfileEditorWidget(QWidget* parent, ChatsListComponent* chatsListComponent, Client* client, Theme theme)
    : QWidget(parent), m_client(client), m_theme(theme), m_chats_list_component(chatsListComponent) {
    setFixedHeight(500);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_fields_edit_component = new FieldsEditComponent(parent, this, client, m_theme);
    m_fields_edit_component->setMaximumSize(400, 500);
    m_photo_edit_component = new PhotoEditComponent(parent, this, client, m_theme);
    m_photo_edit_component->setMinimumSize(600, 800);
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
}

void ProfileEditorWidget::saveChangedPhoto() {

}

void ProfileEditorWidget::save(const std::string& newLogin, const std::string& newName, const std::string& newPassword) {
    if (newLogin != m_client->getMyLogin()) {
        OperationResult isAvailable = m_client->checkIsLoginAvailable(newLogin);
        if (isAvailable == OperationResult::FAIL) {
            m_fields_edit_component->setRedBorderOnLoginEdit();
            return;
        }
    }
    OperationResult res = m_client->updateMyInfo(newLogin, newName, newPassword, m_client->getIsHasPhoto(), m_client->getPhoto());
}

void ProfileEditorWidget::close() {
    m_chats_list_component->setIsEditDialogFlag(false);
    hide();
}

void ProfileEditorWidget::updateAvatar(const Photo& photo) {
    m_fields_edit_component->updateAvatar();
    m_chats_list_component->SetAvatar(photo);
}

void ProfileEditorWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(26, 26, 26, 200));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect(), 20, 20);
}