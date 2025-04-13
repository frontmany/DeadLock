#include <QWidget>
#include <vector>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>

struct StyleEditComponent {
    StyleEditComponent();

    QString DarkLineEditStyle;
    QString LightLineEditStyle;
};


struct StyleAddChatDialogComponent {
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

    QString DarkButtonStyleBlue = R"(
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

    QString LightButtonStyleBlue = R"(
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

};


enum Theme;
class ChatsListComponent;
class ButtonIcon;

class EditComponent : public QWidget {
    Q_OBJECT
public:
    explicit EditComponent(QWidget* parent, ChatsListComponent* chatsListComponent, Theme theme);
    QSize sizeHint() const override;
    void setTheme(Theme theme);
    const QString& getText() const { return m_lineEdit->text(); }
    void resetBorder();


protected:
    void paintEvent(QPaintEvent* event) override;

public slots:
    void setRedBorderToChatAddDialog();

private slots:
    void onTextChanged(const QString& text);

private:
    StyleEditComponent* style;
    Theme               m_theme;
    bool                m_drawRedBorder = false;
    QLineEdit*          m_lineEdit;
    ButtonIcon*         m_cancelButton;
    ChatsListComponent* m_chatsListComponent;
    QColor              m_color;
};


class AddChatDialogComponent : public QWidget {
    Q_OBJECT

public:
    AddChatDialogComponent(QWidget* parent, ChatsListComponent* chatsListComponent, Theme theme);
    void showDialog();
    void closeDialog();
    void setTheme(Theme theme);

    const QString& getText() { return  m_editComponent->getText(); }
    EditComponent* getEditComponent() { return  m_editComponent; }

private slots:
    void slotToSendCreateChatData();

private:
    void setupUI();

private:
    StyleAddChatDialogComponent* style;
    Theme m_theme;

    ChatsListComponent* m_chatsListComponent;
    EditComponent*      m_editComponent;
    QVBoxLayout*        m_dialogVLayout;
    QPushButton*        m_createChatButton;
    bool                m_isVisible;
};