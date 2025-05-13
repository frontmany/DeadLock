#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>
#include <QDebug>
#include <QRegularExpressionValidator>

class LoginWidget;
enum Theme;

struct StyleRegistrationComponent {
    StyleRegistrationComponent();

    QString DarkButtonStyle;
    QString LightButtonStyle;

    QString DarkLineEditStyle;
    QString DarkLineEditStyleRedBorder;


    QString LightLineEditStyle;
    QString LightLineEditStyleRedBorder;
};

class RegistrationComponent : public QWidget {
    Q_OBJECT

public:
    explicit RegistrationComponent(QWidget* parent, LoginWidget* loginWidget);
    void setTheme(Theme& theme);

    void setRedBorderToLoginEdit();
    void setRedBorderToNameEdit();
    void setRedBorderToPasswordEdits();
    void setErrorMessageToLabel(const QString& errorText);

protected:
    void paintEvent(QPaintEvent* event) override;


private slots:
    void slotToSendRegistrationData();
    void resetNameEditStyle();
    void resetLoginEditStyle();
    void resetPasswordEditsStyles();

signals:
    void sendRegistrationData(QString& login, QString& data, QString& name);



private:
    StyleRegistrationComponent* style;
    QColor                      m_backgroundColor;


    QHBoxLayout* m_registerButtonHla;
    QHBoxLayout* m_loginEditHla;
    QHBoxLayout* m_passwordEditHla;
    QHBoxLayout* m_password2EditHla;
    QHBoxLayout* m_nameEditHla;

    QPushButton* m_registerButton;
    QLineEdit* m_loginEdit;
    QLineEdit* m_passwordEdit;
    QLineEdit* m_password2Edit;
    QLineEdit* m_nameEdit;

    QLabel*    m_error_label;

    Theme* m_theme = nullptr;
};