#pragma once
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMessageBox>

class EditDialogComponent;
class ProfileEditorWidget;
class Client;
enum Theme;

struct StylePasswordEditComponent {
    StylePasswordEditComponent();

    QString DarkButtonStyleBlue;
    QString LightButtonStyleBlue;

    QString buttonStyleGrayDark;
    QString buttonStyleGrayLight;

    QString DarkLineEditStyle;
    QString LightLineEditStyle;

    QString LightLabelStyle;
    QString DarkLabelStyle;
};

class PasswordEditComponent : public QWidget {
    Q_OBJECT

public:
    PasswordEditComponent(QWidget* parent, ProfileEditorWidget* profileEditorWidget, Client* client, Theme theme);
    void setTheme(Theme theme);
    bool isVerificationStage() { return m_verificationStage; }

public slots:
    void showNewPasswordInput();
    void showCurrentPasswordInput();
    void showErrorLabelPasswordInput();

private slots:
    void onContinueClicked();
    void onChangePasswordClicked();
    void clearErrorLabel();

private:
    void setupUI();

    StylePasswordEditComponent* m_style = nullptr;
    Theme m_theme;

    ProfileEditorWidget* m_profile_editor_widget = nullptr;
    Client* m_client = nullptr;

    QLabel* m_currentPasswordLabel = nullptr;

    QLabel* m_newPassLabel = nullptr;
    QLabel* m_confirmPassLabel = nullptr;;

    QVBoxLayout* m_mainLayout = nullptr;
    QHBoxLayout* m_buttonsHLayout = nullptr;
    QLineEdit* m_currentPasswordEdit = nullptr;
    QLineEdit* m_newPasswordEdit = nullptr;
    QLineEdit* m_confirmPasswordEdit = nullptr;

    QPushButton* m_continueButton = nullptr;
    QPushButton* m_cancel_button = nullptr;
    QPushButton* m_changePasswordButton = nullptr;

    QLabel* m_errorLabel = nullptr;

    bool m_verificationStage;
};
