#pragma once

#include <QWidget>
#include <QVBoxLayout>
#include <QPixmap>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include "ChatComponent.h"


struct StyleChatsListComponent {
    QString darkSlider = R"(
    QScrollBar:vertical {
        border: 2px solid rgb(36, 36, 36);      
        background: rgb(36, 36, 36);        
        width: 10px;                 
        border-radius: 5px; 
    }

    QScrollBar::handle:vertical {
        background: rgb(56, 56, 56);   
        border: 2px solid rgb(56, 56, 56);      
        width: 10px;    
        border-radius: 5px;           
    }

    QScrollBar::add-line:vertical, 
    QScrollBar::sub-line:vertical { 
        background: none;             
    }
)";

    QString DarkLineEditStyle = R"(
    QLineEdit {
        background-color: #333;    
        color: white;               
        border: none;     
        border-radius: 15px;         
        padding: 5px;               
    }
    QLineEdit:focus {
        border: 1px solid #888;     
    }
)";

    QString LightLineEditStyle = R"(
    QLineEdit {
        background-color: #ffffff;    
        color: black;                 
        border: none;       
        border-radius: 15px;           
        padding: 5px;                 
    }
    QLineEdit:focus {
        border: 1px solid #888;        
    }
)";

    QString addButtonStyle = R"(
    QPushButton {
        background-color: transparent;   
        color: white;             
        border: none;   
        border-radius: 5px;       
        padding: 5px 10px;        
    }
    QPushButton:hover {
        background-color: rgb(26, 133, 255);   
    }
    QPushButton:pressed {
        background-color: rgb(26, 133, 255);      
    }
)";


};

enum Theme;

class ChatsListComponent : public QWidget {
    Q_OBJECT

public:
    explicit ChatsListComponent(QWidget* parent = nullptr);
    ~ChatsListComponent();
    void setTheme(Theme theme);

    void addChatComponent(const QString& name, const QString& message, const QPixmap& avatar);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor          m_backgroundColor;
    StyleChatsListComponent* style;

    QVBoxLayout* m_mainVLayout;
    QHBoxLayout* m_contentsHLayout;

    QScrollArea*        m_scrollArea;
    QWidget*    m_containerWidget;
    QVBoxLayout*        m_containerVLayout;
    QLineEdit*          m_searchLineEdit;
    QPushButton*        m_startChatButton;
};