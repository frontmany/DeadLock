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

#include "fileWrapper.h"

class InnerComponent;
class FilesComponent;
class MessagingAreaComponent;
class Message;
enum Theme;

class MessageComponent : public QWidget {
    Q_OBJECT

public:
    explicit MessageComponent(QWidget* parent, MessagingAreaComponent* messagingAreaComponent, Message* message, Theme theme);
    ~MessageComponent();

    void setTheme(Theme theme);

    void setMessage(Message* message) { m_message = message; }
    Message* getMessage() { return m_message; }

    void setMessageText(const QString& message);
    const QString& getMessageText() const;

    void setTimestamp(const QString& timestamp);
    const QString& getTimestamp() const;

    bool getReadStatus() const;

    bool getIsSent() const { return  m_isSent; }

    bool getIsRead() const { return  m_isRead; }
   
    const QString& getId() const { return m_id; }

public slots:
    void onSendMeFile(const fileWrapper& fileWrapper);
    void requestedFileLoaded(const fileWrapper& fileWrapper);
    void setIsRead(bool isRead);

private:
    Message* m_message;
    Theme m_theme;
    QString m_id;
    bool m_isSent;
    bool m_isRead;
    QHBoxLayout* m_main_HLayout;
    MessagingAreaComponent* m_messaging_area_component;

    InnerComponent* m_inner_component = nullptr;
    FilesComponent* m_files_component = nullptr;
};