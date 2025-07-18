#include"theme.h"
#include"fieldsEditComponent.h"
#include"profileEditorWidget.h"
#include"client.h"
#include"utility.h"
#include"photo.h"
#include"configManager.h"

StyleFieldsEditComponent::StyleFieldsEditComponent() {
    DarkLabelStyle = R"(
        QLabel {
        background-color: transparent;   
        color: white;             
        }
        )";

    LightLabelStyle = R"(
        QLabel {
        background-color: transparent;   
        color: rgb(38, 38, 38);             
        }
        )";

    DarkButtonStyleBlue = R"(
        QPushButton {
        background-color: rgb(21, 119, 232);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        font-family: "Segoe UI";  
        font-size: 14px;          
        }
        QPushButton:hover {
            background-color: rgb(26, 133, 255);   
        }
        QPushButton:pressed {
            background-color: rgb(26, 133, 255);      
        }
        )";

    LightButtonStyleBlue = R"(
        QPushButton {
        background-color: rgb(26, 133, 255);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        font-family: "Segoe UI";  
        font-size: 14px;          
        }
        QPushButton:hover {
            background-color: rgb(21, 119, 232);   
        }
        QPushButton:pressed {
            background-color: rgb(21, 119, 232);      
        }
        )";

    buttonStyleGrayDark = R"(
        QPushButton {
        background-color: rgb(115, 115, 115);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        }
        QPushButton:hover {
            background-color: rgb(135, 135, 135);   
        }
        QPushButton:pressed {
            background-color: rgb(115, 115, 115);      
        }
        )";

    buttonStyleGrayLight = R"(
        QPushButton {
        background-color: rgb(173, 173, 173);   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
        }
        QPushButton:hover {
            background-color: rgb(158, 158, 158);   
        }
        QPushButton:pressed {
            background-color: rgb(158, 158, 158);      
        }
        )";

    buttonToChoosePhotoStyleDark = "QPushButton {"
        "background-color: #2b2b2b;"
        "color: white;"
        "border-radius: 10px;"
        "padding: 8px 12px;"
        "}"
        "QPushButton:hover {"
        "background-color: #3a3a3a;"
        "}";

    buttonToChoosePhotoStyleLight = "QPushButton {"
        "background-color: rgb(209, 209, 209);"
        "color: rgb(48, 48, 48);"
        "border-radius: 10px;"
        "padding: 8px 12px;"
        "}"
        "QPushButton:hover {"
        "background-color: rgb(201, 201, 201);"
        "}";

    DarkLineEditStyle = R"(
        QLineEdit {
            background-color: #333;    
            color: white;     
            font-size: 12px;           
            border: none;     
            border-radius: 5px;         
            padding: 5px;               
        }
        QLineEdit:focus {
            border: none;     
        }
        )";

    LightLineEditStyle = R"(
        QLineEdit {
            background-color: #ffffff;    
            color: rgb(51, 51, 51);      
            font-size: 12px;            
            border: none;       
            border-radius: 5px;           
            padding: 5px;                 
        }
        QLineEdit:focus {
            border: none;     
        }
        )";

    buttonToChangePasswordStyleDark = R"(
            QPushButton {
                background-color: transparent;     
                color: rgb(153, 150, 150);              
                border: none;        
                border-radius: 5px;             
                padding: 5px 10px;              
                font-family: 'Arial';            
                font-size: 12px;                 
            }
            QPushButton:hover {
                color: rgb(26, 133, 255);      
            }
            QPushButton:pressed {
                color: rgb(26, 133, 255);      
            }
        )";

    buttonToChangePasswordStyleLight = R"(
            QPushButton {
                background-color: transparent;     
                color: rgb(153, 150, 150);              
                border: none;        
                border-radius: 5px;             
                padding: 5px 10px;              
                font-family: 'Arial';            
                font-size: 12px;                 
            }
            QPushButton:hover {
                color: rgb(26, 133, 255);      
            }
            QPushButton:pressed {
                color: rgb(26, 133, 255);      
            }
        )";
}


FieldsEditComponent::FieldsEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, std::shared_ptr<ConfigManager> configManager, Theme theme)
    : QWidget(parent), m_profile_editor_widget(profileEditorWidget), m_client(client), m_config_manager(configManager), m_theme(theme)
{
    m_style = new StyleFieldsEditComponent();
    m_error_label = new QLabel;
    m_avatar_label = new QLabel();

    QPixmap currentAvatar;
    if (m_config_manager->getIsHasPhoto()) {
        updateAvatar(*m_config_manager->getPhoto());
    }
    else {
        currentAvatar = QPixmap(":/resources/ChatsWidget/userFriend.png");
        m_avatar_label->setPixmap(currentAvatar.scaled(utility::getScaledSize(95), utility::getScaledSize(95), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    m_change_photo_button = new QPushButton("Change Photo");
    connect(m_change_photo_button, &QPushButton::clicked, [this]() {m_profile_editor_widget->setPhotoEditor(); });
    m_change_photo_button->setFixedHeight(35);

    QRegularExpressionValidator* validator = new QRegularExpressionValidator(
        QRegularExpression("^[a-zA-Z0-9_!@#$%^&*()-+=;:'\",.<>?/\\\\|`~\\[\\]{} ]*$"),
        this
    );

    m_login_label = new QLabel("Login:");
    m_login_edit = new QLineEdit(QString::fromStdString(m_config_manager->getMyLogin()));
    m_login_edit->setFixedHeight(30);
    m_login_edit->setValidator(validator);
    connect(m_login_edit, &QLineEdit::textChanged, [this]() {
        m_error_label->setText("");
    });

    m_name_label = new QLabel("Name:");
    m_name_edit = new QLineEdit(QString::fromStdString(m_config_manager->getMyName()));
    m_name_edit->setFixedHeight(30);
    connect(m_name_edit, &QLineEdit::textChanged, [this]() {
        m_error_label->setText("");
    });

    m_password_button = new QPushButton("Change Password");
    m_password_button->setFixedHeight(35);
    connect(m_password_button, &QPushButton::clicked, m_profile_editor_widget, &ProfileEditorWidget::setPasswordEditor);

   
    m_logoutButton = new QPushButton("Logout");
    connect(m_logoutButton, &QPushButton::clicked, [this]() {
        QDialog logoutDialog(this);
        logoutDialog.setWindowTitle(tr("Exit Confirmation"));
        logoutDialog.setMinimumSize(300, 150);

        QString dialogStyle;
        QString buttonStyle;

        if (m_theme == Theme::DARK) {
            dialogStyle = R"(
            QDialog {
                background-color: #333333;
                color: #f0f0f0;
                font-family: 'Segoe UI';
                font-size: 14px;
                border: 1px solid #444444;
                border-radius: 8px;
            }
            QLabel {
                color: #f0f0f0;
            }
        )";

            buttonStyle = R"(
            QPushButton {
                background-color: #444444;
                color: #f0f0f0;
                border: 1px solid #555555;
                border-radius: 4px;
                padding: 6px 12px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: #555555;
            }
            QPushButton:pressed {
                background-color: #3a3a3a;
            }
        )";
        }
        else {
            dialogStyle = R"(
            QDialog {
                background-color: #ffffff;
                color: #333333;
                font-family: 'Segoe UI';
                font-size: 14px;
                border: 1px solid #dddddd;
                border-radius: 8px;
            }
            QLabel {
                color: #333333;
            }
        )";

            buttonStyle = R"(
            QPushButton {
                background-color: #f0f0f0;
                color: #333333;
                border: 1px solid #cccccc;
                border-radius: 4px;
                padding: 6px 12px;
                min-width: 80px;
            }
            QPushButton:hover {
                background-color: #e5e5e5;
            }
            QPushButton:pressed {
                background-color: #d9d9d9;
            }
        )";
        }

        logoutDialog.setStyleSheet(dialogStyle);

        QVBoxLayout* layout = new QVBoxLayout(&logoutDialog);

        QLabel* messageLabel = new QLabel(tr("Are you sure you want to quit?"), &logoutDialog);
        messageLabel->setAlignment(Qt::AlignCenter);
        messageLabel->setStyleSheet("font-size: 16px; padding: 20px;");

        QHBoxLayout* buttonLayout = new QHBoxLayout();
        QPushButton* confirmButton = new QPushButton(tr("quit"), &logoutDialog);
        QPushButton* cancelButton = new QPushButton(tr("cancel"), &logoutDialog);

        confirmButton->setStyleSheet(buttonStyle);
        cancelButton->setStyleSheet(buttonStyle);

        buttonLayout->addStretch();
        buttonLayout->addWidget(confirmButton);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addStretch();

        layout->addWidget(messageLabel);
        layout->addLayout(buttonLayout);

        connect(confirmButton, &QPushButton::clicked, [&]() {
            logoutDialog.accept();
            m_config_manager->undoAutoLogin();
            m_config_manager->setNeedToUndoAutoLogin(true);
            QApplication::quit();
        });

        connect(cancelButton, &QPushButton::clicked, [&]() {
            logoutDialog.reject();
        });

        if (logoutDialog.exec() == QDialog::Accepted) {
        }
        });


    m_photo_password_buttonsHLayout = new QHBoxLayout();
    m_photo_password_buttonsHLayout->setAlignment(Qt::AlignRight);
    m_photo_password_buttonsHLayout->addSpacing(-20);
    m_photo_password_buttonsHLayout->addWidget(m_logoutButton);
    m_photo_password_buttonsHLayout->addSpacing(20);
    m_photo_password_buttonsHLayout->addWidget(m_change_photo_button);
    m_photo_password_buttonsHLayout->addWidget(m_password_button);

    m_save_button = new QPushButton("Save");
    m_save_button->setMinimumHeight(30);

    connect(m_save_button, &QPushButton::clicked, [this]() {
        std::string name = m_name_edit->text().toStdString();
        std::string login = m_login_edit->text().toStdString();

        if (name == m_config_manager->getMyName() && login == m_config_manager->getMyLogin()) {
            m_profile_editor_widget->close();
            return;
        }

        if (login == "" || name == "") {
            m_error_label->setText("Please fill in all fields");
            return;
        }

        if (login != m_config_manager->getMyLogin()) {
            m_client->checkIsNewLoginAvailable(login);
        }

        if (name != m_config_manager->getMyName() && login != m_config_manager->getMyLogin()) {
            m_client->updateMyName(name);
        }

        if (name != m_config_manager->getMyName() && login == m_config_manager->getMyLogin()) {
            m_client->updateMyName(name);
            m_profile_editor_widget->close();
        }
    });



    m_cancel_button = new QPushButton("Cancel");
    m_cancel_button->setMinimumHeight(30);
    connect(m_cancel_button, &QPushButton::clicked, [this]() {
        m_login_edit->setText(QString::fromStdString(m_config_manager->getMyLogin()));
        m_name_edit->setText(QString::fromStdString(m_config_manager->getMyName()));
        m_profile_editor_widget->close();
    });
     
    m_save_cancel_buttonsHLayout = new QHBoxLayout();
    m_save_cancel_buttonsHLayout->addWidget(m_cancel_button);
    m_save_cancel_buttonsHLayout->addWidget(m_save_button);

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addSpacing(-8); 
    hLayout->setAlignment(Qt::AlignCenter);
    hLayout->addWidget(m_avatar_label);

    m_mainVLayout = new QVBoxLayout;
    m_mainVLayout->setAlignment(Qt::AlignCenter);
    m_mainVLayout->addLayout(hLayout);
    m_mainVLayout->addSpacing(20);
    m_mainVLayout->addLayout(m_photo_password_buttonsHLayout);
    m_mainVLayout->addSpacing(20);
    m_mainVLayout->addWidget(m_login_label);
    m_mainVLayout->addWidget(m_login_edit);
    m_mainVLayout->addSpacing(10);
    m_mainVLayout->addWidget(m_name_label);
    m_mainVLayout->addWidget(m_name_edit);
    m_mainVLayout->addSpacing(5);
    m_mainVLayout->addWidget(m_error_label);
    m_mainVLayout->addSpacing(20);
    m_mainVLayout->addLayout(m_save_cancel_buttonsHLayout);
    m_mainVLayout->addSpacing(10);

    setLayout(m_mainVLayout);

    setTheme(m_theme);
}

void FieldsEditComponent::setName(const std::string& name) {
    m_name_edit->setText(QString::fromStdString(name));
}

void FieldsEditComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (theme == Theme::DARK) {
        m_error_label->setStyleSheet("color: rgb(250, 132, 132);");
        m_login_label->setStyleSheet(m_style->DarkLabelStyle);
        m_name_label->setStyleSheet(m_style->DarkLabelStyle);
        m_cancel_button->setStyleSheet(m_style->buttonStyleGrayDark);
        m_save_button->setStyleSheet(m_style->DarkButtonStyleBlue);
        m_change_photo_button->setStyleSheet(m_style->buttonToChoosePhotoStyleDark);

        m_password_button->setStyleSheet(m_style->buttonToChangePasswordStyleDark);
        m_logoutButton->setStyleSheet(m_style->buttonToChangePasswordStyleDark);

        m_login_edit->setStyleSheet(m_style->DarkLineEditStyle);
        m_name_edit->setStyleSheet(m_style->DarkLineEditStyle);
    }
    else {
        m_error_label->setStyleSheet("color: rgb(255, 74, 74);");
        m_login_label->setStyleSheet(m_style->LightLabelStyle);
        m_name_label->setStyleSheet(m_style->LightLabelStyle);
        m_cancel_button->setStyleSheet(m_style->buttonStyleGrayLight);
        m_save_button->setStyleSheet(m_style->LightButtonStyleBlue);
        m_change_photo_button->setStyleSheet(m_style->buttonToChoosePhotoStyleLight);

        m_password_button->setStyleSheet(m_style->buttonToChangePasswordStyleLight);
        m_logoutButton->setStyleSheet(m_style->buttonToChangePasswordStyleLight);

        m_login_edit->setStyleSheet(m_style->LightLineEditStyle);
        m_name_edit->setStyleSheet(m_style->LightLineEditStyle);
    }
}

void FieldsEditComponent::updateAvatar(const Photo& photo) {
    try {
        std::string path = photo.getPhotoPath();
        if (path.empty()) {
            throw std::runtime_error("Empty photo path");
        }

        std::ifstream file(path, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open photo file");
        }

        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::string fileData(fileSize, '\0');
        file.read(&fileData[0], fileSize);
        file.close();

        size_t delimiterPos = fileData.find('\n');
        if (delimiterPos == std::string::npos) {
            throw std::runtime_error("Invalid photo file format");
        }

        std::string encryptedKey = fileData.substr(0, delimiterPos);
        std::string encryptedData = fileData.substr(delimiterPos + 1);

        auto aesKey = utility::RSADecryptKey(m_client->getPrivateKey(), encryptedKey);

        std::string decryptedData = utility::AESDecrypt(aesKey, encryptedData);

        QPixmap avatar;
        if (!avatar.loadFromData(reinterpret_cast<const uchar*>(decryptedData.data()),
            decryptedData.size())) {
            throw std::runtime_error("Failed to create pixmap from decrypted data");
        }

        m_avatar_label->setPixmap(avatar.scaled(utility::getScaledSize(95),
            utility::getScaledSize(95),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
        update();
    }
    catch (const std::exception& e) {
        std::cerr << "Error updating avatar: " << e.what() << std::endl;
        QPixmap defaultAvatar(":/resources/default_avatar.png");
        m_avatar_label->setPixmap(defaultAvatar.scaled(utility::getScaledSize(95),
            utility::getScaledSize(95),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation));
        update();
    }
}

void FieldsEditComponent::setErrorText(const QString& text) {
    m_error_label->setText(text);
}