#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QPainter>
#include <QPixmap>
#include <QVBoxLayout>
#include <QRegularExpression>

#include "loginWidget.h"


struct StyleAuthorizationComponent {
    StyleAuthorizationComponent();

    QString DarkButtonStyle;
    QString DarkLineEditStyle;
    QString LightButtonStyle;
    QString LightLineEditStyle;
    QString DarkLineEditStyleRedBorder;
    QString LightLineEditStyleRedBorder;
};

class LoginWidget;
enum Theme;

class AuthorizationComponent : public QWidget {
    Q_OBJECT

public:
    explicit AuthorizationComponent(QWidget* parent, LoginWidget* loginWidget);
    void setTheme(Theme& theme);

    void setRedBorderToLoginEdit();
    void setRedBorderToPasswordEdit();
    void setErrorMessageToLabel(const QString& errorText);

protected:
    void paintEvent(QPaintEvent* event) override;
    

private slots:
    void SlotToSendLoginData();
    void resetLoginEditStyle();
    void resetPasswordEditStyle();

signals:
    void sendLoginData(QString& login, QString& password);


private:
    StyleAuthorizationComponent*    style;
    QColor                      m_backgroundColor;


    QHBoxLayout*                m_loginButtonHla;
    QHBoxLayout*                m_loginEditHla;
    QHBoxLayout*                m_passwordEditHla;

    QPushButton*                m_loginButton;
    QLineEdit*                  m_loginEdit;
    QLineEdit*                  m_passwordEdit;

    QLabel*                     m_error_label;

    Theme* m_theme = nullptr;
};