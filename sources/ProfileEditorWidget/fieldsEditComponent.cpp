#include"fieldsEditComponent.h"
#include"profileEditorWidget.h"
#include"client.h"

FieldsEditComponent::FieldsEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, Theme theme)
    : QWidget(parent), m_profile_editor_widget(profileEditorWidget), m_client(client), m_theme(theme)
{
    m_style = new StyleFieldsEditComponent();

    QPixmap currentAvatar;
    if (m_client->getIsHasPhoto()) {
        currentAvatar = QPixmap(QString::fromStdString(m_client->getPhoto().getPhotoPath()));
        
    }
    else {
        currentAvatar = QPixmap(":/resources/ChatsWidget/userFriend.png");
    }
    
    m_avatar_label = new QLabel();
    m_avatar_label->setPixmap(currentAvatar.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_change_photo_button = new QPushButton("Change Photo");
    m_change_photo_button->setStyleSheet(m_style->buttonToChoosePhotoStyleDark);
    connect(m_change_photo_button, &QPushButton::clicked, [this]() {m_profile_editor_widget->setPhotoEditor(); });

    m_login_label = new QLabel("Login:");
    m_login_edit = new QLineEdit(QString::fromStdString(m_client->getMyLogin()));
    m_login_edit->setStyleSheet(m_style->DarkLineEditStyle);

    m_name_label = new QLabel("Name:");
    m_name_edit = new QLineEdit(QString::fromStdString(m_client->getMyName()));
    m_name_edit->setStyleSheet(m_style->DarkLineEditStyle);

    m_password_label = new QLabel("Password:");
    m_password_edit = new QLineEdit(QString::fromStdString(m_client->getPassword()));
    m_password_edit->setStyleSheet(m_style->DarkLineEditStyle);
    m_password_edit->setEchoMode(QLineEdit::Password);

    m_save_button = new QPushButton("Save");
    m_save_button->setMinimumHeight(30);
    m_save_button->setStyleSheet(m_style->buttonStyleBlueDark);
    connect(m_save_button, &QPushButton::clicked, [this]() {

        });

    m_cancel_button = new QPushButton("Cancel");
    m_cancel_button->setMinimumHeight(30);
    m_cancel_button->setStyleSheet(m_style->buttonStyleGrayDark);
    connect(m_cancel_button, &QPushButton::clicked, m_profile_editor_widget, &ProfileEditorWidget::close);
     
    m_buttonsHLayout = new QHBoxLayout();
    m_buttonsHLayout->addWidget(m_cancel_button);
    m_buttonsHLayout->addWidget(m_save_button);


    m_mainVLayout = new QVBoxLayout;
    m_mainVLayout->addWidget(m_avatar_label, 0, Qt::AlignCenter);
    m_mainVLayout->addWidget(m_change_photo_button, 0, Qt::AlignCenter);
    m_mainVLayout->addSpacing(20);
    m_mainVLayout->addWidget(m_login_label);
    m_mainVLayout->addWidget(m_login_edit);
    m_mainVLayout->addSpacing(10);
    m_mainVLayout->addWidget(m_name_label);
    m_mainVLayout->addWidget(m_name_edit);
    m_mainVLayout->addSpacing(10);
    m_mainVLayout->addWidget(m_password_label);
    m_mainVLayout->addWidget(m_password_edit);
    m_mainVLayout->addSpacing(20);
    m_mainVLayout->addSpacing(20);
    m_mainVLayout->addLayout(m_buttonsHLayout);

    setLayout(m_mainVLayout);
}

void FieldsEditComponent::setRedBorderOnLoginEdit() {
    m_login_edit->setStyleSheet(m_style->DarkLineEditStyleRedBorder);
}

void FieldsEditComponent::updateAvatar() {
    QPixmap currentAvatar = QPixmap(QString::fromStdString(m_client->getPhoto().getPhotoPath()));
    m_avatar_label->setPixmap(currentAvatar);
}
