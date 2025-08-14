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
#include <QEvent>
#include <QPushButton>
#include <QScrollBar>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

#include "theme.h"
#include "net_file.h"
#include "queryType.h"
#include "fileWrapper.h"

class InnerComponent;
class FilesComponent;
class MessagingAreaComponent;
class Message;
enum Theme;


class MessageComponent : public QWidget {
    Q_OBJECT
private:
    enum class ComponentStructureType {
        FILES_COMPONENT,
        MESSAGE_COMPONENT
    };

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
    void setRetry();
    void requestedFileUnLoadedError(const fileWrapper& fileWrapper);
    void onSendMeFile(const fileWrapper& fileWrapper);
    void requestedFileLoaded(const fileWrapper& fileWrapper);
    void setIsRead(bool isRead);
    void setProgress(const net::File& file, int percent);

private:
    void removeRetry();

private:
    Message* m_message;
    Theme m_theme;
    QString m_id;
    bool m_isSent;
    bool m_isRead;

    QWidget*     m_retryButtonContainer = nullptr;
    QPushButton* m_retryButton = nullptr;
    QHBoxLayout* m_main_HLayout;

    InnerComponent* m_inner_component = nullptr;
    FilesComponent* m_files_component = nullptr;

    MessagingAreaComponent* m_messaging_area_component;
    ComponentStructureType  m_component_structure_type;
};