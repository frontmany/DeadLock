#include <QRegularExpression>

#include "passwordEditorComponent.h"
#include "profileEditorWidget.h"
#include "mainwindow.h"
#include "utility.h"
#include "client.h"


StylePasswordEditComponent::StylePasswordEditComponent() {

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

    DarkLabelStyle = R"(
    QLabel {
        background-color: transparent;  
        color: rgb(161, 161, 161);
        border: none;
        border-radius: 5px;
        padding: 5px 10px;
        font-size: 14px;
    }
    )";

    LightLabelStyle = R"(
    QLabel {
        background-color: transparent;  
        color: rgb(51, 51, 51); 
        border: none;
        border-radius: 5px;
        padding: 5px 10px;
        font-size: 14px;
    }
    )";
}

PasswordEditComponent::PasswordEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, Theme theme)
    : QWidget(parent), m_profile_editor_widget(profileEditorWidget), m_client(client), m_theme(theme), m_verificationStage(true) {
    m_style = new StylePasswordEditComponent();
    setupUI();
    setTheme(theme);
}

void PasswordEditComponent::setupUI() {
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setSpacing(15);
    m_mainLayout->setContentsMargins(20, 20, 20, 20);

    showCurrentPasswordInput();
}

void PasswordEditComponent::showCurrentPasswordInput() {
    // Очищаем предыдущие виджеты
    QLayoutItem* child;
    while ((child = m_mainLayout->takeAt(1)) != nullptr) {
        delete child->widget();
        delete child;
    }
    if (m_changePasswordButton != nullptr) {
        delete m_changePasswordButton;
        m_changePasswordButton = nullptr;
    }
    if (m_cancel_button != nullptr) {
        delete m_cancel_button;
        m_cancel_button = nullptr;
    }
    if (m_newPassLabel != nullptr) {
        delete m_newPassLabel;
        m_newPassLabel = nullptr;
    }
    m_newPasswordEdit = nullptr;
    m_confirmPasswordEdit = nullptr;

    m_verificationStage = true;

    m_currentPasswordLabel = new QLabel("Current password:", this);
    m_currentPasswordEdit = new QLineEdit(this);
    m_currentPasswordEdit->setEchoMode(QLineEdit::Password);
    m_currentPasswordEdit->setPlaceholderText("Enter your current password");
    connect(m_currentPasswordEdit, &QLineEdit::textChanged, [this]() {
            m_errorLabel->setText("");
        });

    m_cancel_button = new QPushButton("Cancel");
    m_cancel_button->setMinimumHeight(30);
    connect(m_cancel_button, &QPushButton::clicked, m_profile_editor_widget, &ProfileEditorWidget::setFieldsEditor);
    connect(m_cancel_button, &QPushButton::clicked, this, [this]() {m_errorLabel->setText(""); });

    m_continueButton = new QPushButton("Continue", this);
    connect(m_continueButton, &QPushButton::clicked, this, &PasswordEditComponent::onContinueClicked);

    m_buttonsHLayout = new QHBoxLayout;
    m_buttonsHLayout->addWidget(m_cancel_button);
    m_buttonsHLayout->addWidget(m_continueButton);


    m_errorLabel = new QLabel(this);
    m_errorLabel->setStyleSheet("color: red;");

    m_mainLayout->addWidget(m_currentPasswordLabel);
    m_mainLayout->addWidget(m_currentPasswordEdit);
    m_mainLayout->addLayout(m_buttonsHLayout);
    m_mainLayout->addWidget(m_errorLabel);

    setTheme(m_theme);
}

void PasswordEditComponent::showNewPasswordInput() {
    // Очищаем предыдущие виджеты
    QLayoutItem* child;
    while ((child = m_mainLayout->takeAt(1)) != nullptr) {
        delete child->widget();
        delete child;
    }
    delete m_continueButton;
    m_continueButton = nullptr;

    delete m_currentPasswordLabel;
    m_currentPasswordLabel = nullptr;

    m_verificationStage = false;

    m_newPassLabel = new QLabel("New password:", this);
    m_newPasswordEdit = new QLineEdit(this);
    m_newPasswordEdit->setEchoMode(QLineEdit::Password);
    m_newPasswordEdit->setPlaceholderText("Enter new password");

    m_confirmPassLabel = new QLabel("Confirm password:", this);
    m_confirmPasswordEdit = new QLineEdit(this);
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setPlaceholderText("Confirm new password");



    m_changePasswordButton = new QPushButton("Change Password", this);
    connect(m_changePasswordButton, &QPushButton::clicked, this, &PasswordEditComponent::onChangePasswordClicked);
    connect(m_newPasswordEdit, &QLineEdit::textChanged, this, &PasswordEditComponent::clearErrorLabel);
    connect(m_confirmPasswordEdit, &QLineEdit::textChanged, this, &PasswordEditComponent::clearErrorLabel);

    m_buttonsHLayout = new QHBoxLayout;
    m_buttonsHLayout->addWidget(m_cancel_button);
    m_buttonsHLayout->addWidget(m_changePasswordButton);


    m_errorLabel = new QLabel(this);

    m_mainLayout->addWidget(m_newPassLabel);
    m_mainLayout->addWidget(m_newPasswordEdit);
    m_mainLayout->addWidget(m_confirmPassLabel);
    m_mainLayout->addWidget(m_confirmPasswordEdit);
    m_mainLayout->addSpacing(15);
    m_mainLayout->addLayout(m_buttonsHLayout);
    m_mainLayout->addWidget(m_errorLabel);

    setTheme(m_theme);
}

void PasswordEditComponent::setTheme(Theme theme) {
    m_theme = theme;

    if (theme == Theme::DARK) {
        m_errorLabel->setStyleSheet("color: rgb(250, 132, 132);");
        m_cancel_button->setStyleSheet(m_style->buttonStyleGrayDark);
        

        if (m_verificationStage) {
            m_currentPasswordLabel->setStyleSheet(m_style->DarkLabelStyle);
            m_continueButton->setStyleSheet(m_style->DarkButtonStyleBlue);
            m_currentPasswordEdit->setStyleSheet(m_style->DarkLineEditStyle);
        }
        else {
            m_newPassLabel->setStyleSheet(m_style->DarkLabelStyle);
            m_confirmPassLabel->setStyleSheet(m_style->DarkLabelStyle);
            m_changePasswordButton->setStyleSheet(m_style->DarkButtonStyleBlue);
        }

        if (m_newPasswordEdit != nullptr)
            m_newPasswordEdit->setStyleSheet(m_style->DarkLineEditStyle);

        if (m_confirmPasswordEdit != nullptr) 
            m_confirmPasswordEdit->setStyleSheet(m_style->DarkLineEditStyle);
    }
    else {
        m_errorLabel->setStyleSheet("color: rgb(255, 74, 74);");
        m_cancel_button->setStyleSheet(m_style->buttonStyleGrayLight);

        if (m_verificationStage) {
            m_currentPasswordLabel->setStyleSheet(m_style->LightLabelStyle);
            m_continueButton->setStyleSheet(m_style->LightButtonStyleBlue);
            m_currentPasswordEdit->setStyleSheet(m_style->LightLineEditStyle);
        }
        else {
            m_newPassLabel->setStyleSheet(m_style->LightLabelStyle);
            m_confirmPassLabel->setStyleSheet(m_style->LightLabelStyle);
            m_changePasswordButton->setStyleSheet(m_style->LightButtonStyleBlue);
        }

        if (m_newPasswordEdit != nullptr)
            m_newPasswordEdit->setStyleSheet(m_style->LightLineEditStyle);

        if (m_confirmPasswordEdit != nullptr)
            m_confirmPasswordEdit->setStyleSheet(m_style->LightLineEditStyle);
    }
}

void PasswordEditComponent::onContinueClicked() {
    QString currentPassword = m_currentPasswordEdit->text();

    if (currentPassword.isEmpty()) {
        m_errorLabel->setText("Please enter your current password");
        return;
    }
    else {
        m_errorLabel->setText("");
    }

    m_client->verifyPassword(utility::hashPassword(currentPassword.toStdString()));
}

void PasswordEditComponent::onChangePasswordClicked() {
    QString newPassword = m_newPasswordEdit->text();
    QString confirmPassword = m_confirmPasswordEdit->text();

    if (newPassword.isEmpty() || confirmPassword.isEmpty()) {
        m_errorLabel->setText("Please fill in all fields");
        return;
    }

    if (newPassword != confirmPassword) {
        m_errorLabel->setText("Passwords don't match");
        return;
    }

    // add this check in release
     /*
    QRegularExpression passwordRegex("^(?=.*[a-z])(?=.*[A-Z])(?=.*\\d).+$");
    if (!passwordRegex.match(newPassword).hasMatch()) {
        m_errorLabel->setText("Password must contain:\n- At least one lowercase letter\n- At least one uppercase letter\n- At least one digit");
        return;
    }
    */
    /*
    if (newPassword.length() < 8) {
        m_errorLabel->setText("Password must be at least 8 characters");
        return;
    }
    */

    m_client->updateMyPassword(utility::hashPassword(newPassword.toStdString()));
    m_profile_editor_widget->setFieldsEditor();
}

void PasswordEditComponent::clearErrorLabel() {
    m_errorLabel->setText("");
}


void PasswordEditComponent::showErrorLabelPasswordInput() {
    m_errorLabel->setText("Incorrect current password");
}