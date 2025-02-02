#include "addChatDialogComponent.h"
#include "chatsListComponent.h"
#include "buttons.h"
#include "mainwindow.h"

EditComponent::EditComponent(QWidget* parent, ChatsListComponent* chatsListComponent, Theme theme) : QWidget(parent) {
    m_theme = theme;
    m_chatsListComponent = chatsListComponent;
    style = new StyleEditComponent;

    m_lineEdit = new QLineEdit(this);
    m_lineEdit->setPlaceholderText("friend's login...");
    m_lineEdit->setMaximumSize(930, 30);

    m_cancelButton = new ButtonIcon(this, 25, 25);
    QIcon icon1(":/resources/ChatsWidget/closeDark.png");
    QIcon iconHover1(":/resources/ChatsWidget/closeHoverDark.png");
    m_cancelButton->uploadIconsDark(icon1, iconHover1);
    QIcon icon2(":/resources/ChatsWidget/closeLight.png");
    QIcon iconHover2(":/resources/ChatsWidget/closeHoverLight.png");
    m_cancelButton->uploadIconsLight(icon2, iconHover2);
    m_cancelButton->setTheme(m_theme);

    auto mainLayout = new QHBoxLayout(this);
    auto adjustVLayout = new QVBoxLayout;
    auto adjustVLayout2 = new QVBoxLayout;
    
    adjustVLayout->setAlignment(Qt::AlignTop);
    adjustVLayout->addSpacing(0);
    adjustVLayout->addWidget(m_cancelButton);

    adjustVLayout2->setAlignment(Qt::AlignTop);
    adjustVLayout2->addSpacing(-2);
    adjustVLayout2->addWidget(m_lineEdit);

    mainLayout->setSpacing(0); 
    mainLayout->setContentsMargins(5, 5, 5, 5); 
    mainLayout->addLayout(adjustVLayout2);
    mainLayout->addLayout(adjustVLayout);


    setFixedHeight(35);
    setLayout(mainLayout);
    setTheme(m_theme);
    connect(m_cancelButton, &ButtonIcon::clicked, m_chatsListComponent, &ChatsListComponent::closeAddChatDialog);
    connect(m_lineEdit, &QLineEdit::textChanged, this, &EditComponent::onTextChanged);
}

void EditComponent::setRedBorderToChatAddDialog() {
    m_drawRedBorder = true; 
    update();
}

void EditComponent::onTextChanged(const QString& text) {
    if (text == "") {
        resetBorder();
    }

}




void EditComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (theme == DARK) {
        m_color = QColor(51, 51, 51);
        m_lineEdit->setStyleSheet(style->DarkLineEditStyle);
        m_cancelButton->setTheme(DARK);
    }
    else {

        m_color = QColor(255, 255, 255);
        m_lineEdit->setStyleSheet(style->LightLineEditStyle);
        m_cancelButton->setTheme(LIGHT);
    }
    update();
}

void EditComponent::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);

    // Создаем path для закругленного прямоугольника
    QPainterPath path;
    QRect rect = this->rect();
    int radius = 16; // Радиус закругления
    path.addRoundedRect(rect, radius, radius);

    // Рисуем фон с помощью path
    painter.fillPath(path, m_color); // Цвет фона
    // Устанавливаем контур обрезки и отрисовываем
    painter.setClipPath(path);

    QColor color;
    if (m_theme == DARK) {
        color = QColor(255, 82, 82);
    }
    else {
        color = QColor(255, 189, 189);
    }

    // Если нужно, рисуем красную рамку
    if (m_drawRedBorder) {
        painter.setPen(QPen(color, 3)); // Устанавливаем цвет и толщину рамки
        painter.drawRoundedRect(rect, radius, radius); // Рисуем рамку
    }

    QWidget::paintEvent(event);
}



QSize EditComponent::sizeHint() const {
    return QSize(200, 40);
}

void EditComponent::resetBorder() {
    m_drawRedBorder = false; 
    update();
}

AddChatDialogComponent::AddChatDialogComponent(QWidget* parent, ChatsListComponent* chatsListComponent, Theme theme)
    : QWidget(parent), m_isVisible(false), m_chatsListComponent(chatsListComponent), m_theme(theme) {
    style = new StyleAddChatDialogComponent;
    setupUI();
    setTheme(m_theme);
    this->hide();
    this->setFixedHeight(100);
}

void AddChatDialogComponent::showDialog() {
    if (m_isVisible) {
        return;
    }

    this->show();
    this->setFixedHeight(100);
}


void AddChatDialogComponent::closeDialog() {
    QPropertyAnimation* closeAnimation = new QPropertyAnimation(this, "geometry");
    closeAnimation->setDuration(200);
    QRect rect = this->geometry();
    closeAnimation->setStartValue(rect);
    closeAnimation->setEndValue(QRect(rect.x(), rect.y() - 20, rect.width(), 0));
    closeAnimation->setEasingCurve(QEasingCurve::OutCubic);

    connect(closeAnimation, &QPropertyAnimation::finished, this, [this]() {
        this->hide();
        m_chatsListComponent->getSearchLineEdit()->setHidden(false);
        m_isVisible = false;
        });

    closeAnimation->start();
}

void AddChatDialogComponent::setupUI() {
    m_createChatButton = new QPushButton("Create Chat", this);
    m_editComponent = new EditComponent(this, m_chatsListComponent, m_theme);

    m_dialogVLayout = new QVBoxLayout(this);
    m_dialogVLayout->addWidget(m_editComponent);
    m_dialogVLayout->addWidget(m_createChatButton);
    setLayout(m_dialogVLayout);

    
    connect(m_createChatButton, &QPushButton::clicked, this, &AddChatDialogComponent::slotToSendCreateChatData);
}

void AddChatDialogComponent::slotToSendCreateChatData() {
    m_chatsListComponent->receiveCreateChatData(m_editComponent->getText());
}


void AddChatDialogComponent::setTheme(Theme theme) {
    m_theme = theme;
    if (theme == DARK) {
        m_createChatButton->setStyleSheet(style->DarkButtonStyleBlue);
        m_editComponent->setTheme(DARK);
    }
    else {
        m_createChatButton->setStyleSheet(style->LightButtonStyleBlue);
        m_editComponent->setTheme(LIGHT);
    }
    update();
}