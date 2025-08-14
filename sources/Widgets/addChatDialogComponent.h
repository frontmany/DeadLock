#include <QWidget>
#include <vector>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QPushButton>

struct StyleEditComponent {
    StyleEditComponent();
    QString DarkLineEditStyle;
    QString LightLineEditStyle;
};

struct StyleAddChatDialogComponent {
    StyleAddChatDialogComponent();
    QString AddButtonStyle;
    QString DarkButtonStyleBlue;
    QString LightButtonStyleBlue;
};

class ChatsListComponent;
class ButtonIcon;
enum Theme;

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