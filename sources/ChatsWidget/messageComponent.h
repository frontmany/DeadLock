#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QLabel>
#include <QSize>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPainterPath>
#include <QPainter>
#include <QScrollBar>

#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>



struct StyleInnerComponent {
    QString labelStyleDarkMessage = R"(
    QLabel {
        background-color: transparent; 
        color: rgb(240, 240, 240); 
        font-family: 'Segoe UI'; 
        font-size: 14px; 
        font-weight: normal;
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";

    QString labelStyleLightMessage = R"(
    QLabel {
        background-color: transparent; 
        color: rgb(52, 52, 52); 
        font-family: 'Segoe UI'; 
        font-size: 14px; 
        font-weight: normal;
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";

    QString labelStyleDarkTime = R"(
    QLabel {
        background-color: transparent; 
        color: rgb(219, 219, 219); 
        font-family: 'Segoe UI'; 
        font-size: 13px; 
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";

    QString labelStyleLightTime = R"(
    QLabel {
        background-color: transparent; 
        color: rgb(153, 153, 153); 
        font-family: 'Segoe UI'; 
        font-size: 13px; 
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";

    QString labelStyleLight = R"(
    QLabel {
        background-color: transparent; 
        color: rgb(38, 38, 38); 
        font-family: 'Segoe UI'; 
        font-size: 14px; 
        padding: 10px; 
        border: none; 
        border-radius: 20px; 
    }
)";
};


class ButtonIcon;
class MessagingAreaComponent;
class Message;
enum Theme;

class InnerComponent : public QWidget {
public:
    InnerComponent(QWidget* parent, const QString& timestamp, const QString& text, Theme theme, bool isSent);
    void setTheme(Theme theme);

    void setText(const QString& text) { m_textLabel->setText(text); }
    const QString& getText() const { return m_textLabel->text(); }
    void setTimestamp(const QString& text) { m_timestampLabel->setText(text); }
    const QString& getTimestamp() { return m_timestampLabel->text(); }
    bool getIsRead() const { return m_is_read; }
    void setIsRead(bool isRead) {m_is_read = isRead; }
    void setReadStatus(bool read);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    bool m_is_read = false;
    bool m_isSent = false;
    StyleInnerComponent* style;
    Theme m_theme;
    QColor m_backColor;

    QLabel* m_textLabel;          
    QLabel* m_timestampLabel; 
    ButtonIcon* m_readStatusBtn;

    QHBoxLayout* m_main_HLayout; 
    QVBoxLayout* m_text_VLayout; 
    QVBoxLayout* m_time_VLayout; 
    QVBoxLayout* m_isRead_VLayout; 

};


class MessageComponent : public QWidget {
    Q_OBJECT

public:
    explicit MessageComponent(QWidget* parent, Message* message, Theme theme);
    void setTheme(Theme theme) { m_innerWidget->setTheme(theme); }

    void setMessage(const QString& message) { m_innerWidget->setText(message); }
    const QString& getMessage() const{ return m_innerWidget->getText(); }

    void setTimestamp(const QString& timestamp) { m_innerWidget->setTimestamp(timestamp); }
    const QString& getTimestamp() const { return m_innerWidget->getTimestamp(); }

    bool getReadStatus() const { return  m_innerWidget->getIsRead(); }

    bool getIsSent() const { return  m_isSent; }

    bool getIsRead() const { return  m_isRead; }

    const QString& getId() const { return m_id; }

public slots:
    void setReadStatus(bool isRead) { m_innerWidget->setReadStatus(isRead); }

private:
    Theme m_theme;
    QString m_id;
    bool m_isSent;
    bool m_isRead;
    QHBoxLayout* m_main_HLayout;
    InnerComponent* m_innerWidget;
};