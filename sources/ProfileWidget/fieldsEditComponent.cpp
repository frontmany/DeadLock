#include"fieldsEditComponent.h"
#include"mainwindow.h"
#include"profileEditorWidget.h"
#include"client.h"
#include"utility.h"
#include"photo.h"

FieldsEditComponent::FieldsEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, Theme theme)
    : QWidget(parent), m_profile_editor_widget(profileEditorWidget), m_client(client), m_theme(theme)
{
    m_style = new StyleFieldsEditComponent();

    QPixmap currentAvatar;
    if (m_client->getIsHasPhoto()) {
        currentAvatar = QPixmap(QString::fromStdString(m_client->getPhoto()->getPhotoPath())); 
    }
    else {
        currentAvatar = QPixmap(":/resources/ChatsWidget/userFriend.png");
    }
    
    m_avatar_label = new QLabel();
    m_avatar_label->setPixmap(currentAvatar.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    m_change_photo_button = new QPushButton("Change Photo");
    connect(m_change_photo_button, &QPushButton::clicked, [this]() {m_profile_editor_widget->setPhotoEditor(); });

    m_login_label = new QLabel("Login:");
    m_login_edit = new QLineEdit(QString::fromStdString(m_client->getMyLogin()));
    m_login_edit->setDisabled(true);

    m_name_label = new QLabel("Name:");
    m_name_edit = new QLineEdit(QString::fromStdString(m_client->getMyName()));

    m_password_label = new QLabel("Password:");
    m_password_edit = new QLineEdit();
    m_password_edit->setPlaceholderText("Leave empty if no changes");
    connect(m_password_edit, &QLineEdit::textChanged, [this](const QString& text) {
        if (!text.isEmpty()) {
            m_password_edit->setEchoMode(QLineEdit::Password);
        }
        else {
            m_password_edit->setEchoMode(QLineEdit::Normal);
        }
    });

    QRegularExpressionValidator* validator = new QRegularExpressionValidator(
        QRegularExpression("^[a-zA-Z0-9_!@#$%^&*()-+=;:'\",.<>?/\\\\|`~\\[\\]{} ]*$"),
        this
    );

    m_login_edit->setValidator(validator);
    m_password_edit->setValidator(validator);

    m_save_button = new QPushButton("Save");
    m_save_button->setMinimumHeight(30);
    connect(m_save_button, &QPushButton::clicked, [this]() {
        std::string name = m_name_edit->text().toStdString();
        std::string password = m_password_edit->text().toStdString();

        if (m_client->getMyName() != name) {
            m_client->updateMyName(name);
        }
        if (password != "") {
            m_client->updateMyPassword(utility::hashPassword(password));
        }
        m_profile_editor_widget->close();
        });

    m_cancel_button = new QPushButton("Cancel");
    m_cancel_button->setMinimumHeight(30);
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

    setTheme(m_theme);
}


void FieldsEditComponent::setTheme(Theme theme) {
    if (theme == Theme::DARK) {
        m_login_label->setStyleSheet(m_style->DarkLabelStyle);
        m_name_label->setStyleSheet(m_style->DarkLabelStyle);
        m_password_label->setStyleSheet(m_style->DarkLabelStyle);
        m_cancel_button->setStyleSheet(m_style->buttonStyleGrayDark);
        m_save_button->setStyleSheet(m_style->DarkButtonStyleBlue);
        m_change_photo_button->setStyleSheet(m_style->buttonToChoosePhotoStyleDark);
        m_login_edit->setStyleSheet(m_style->DarkLineEditDisabledStyle);
        m_name_edit->setStyleSheet(m_style->DarkLineEditStyle);
        m_password_edit->setStyleSheet(m_style->DarkLineEditStyle);
    }
    else {
        m_login_label->setStyleSheet(m_style->LightLabelStyle);
        m_name_label->setStyleSheet(m_style->LightLabelStyle);
        m_password_label->setStyleSheet(m_style->LightLabelStyle);
        m_cancel_button->setStyleSheet(m_style->buttonStyleGrayLight);
        m_save_button->setStyleSheet(m_style->LightButtonStyleBlue);
        m_change_photo_button->setStyleSheet(m_style->buttonToChoosePhotoStyleLight);
        m_login_edit->setStyleSheet(m_style->LightLineEditDisabledStyle);
        m_name_edit->setStyleSheet(m_style->LightLineEditStyle);
        m_password_edit->setStyleSheet(m_style->LightLineEditStyle);
    }
}

void FieldsEditComponent::updateAvatar() {
    QPixmap currentAvatar = QPixmap(QString::fromStdString(m_client->getPhoto()->getPhotoPath()));
    m_avatar_label->setPixmap(currentAvatar);
    update();
}
