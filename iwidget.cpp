#include "iwidget.h"

iWidget::iWidget(QWidget* parent) :
    QWidget(parent)
{
    _icon = new Icon();
    _event_timer = new QTimer(this);
    _icon_title_editor = new QLineEdit("Unknown", this);

    DefaultAdjustement();
}

iWidget::iWidget(Icon *icon, QWidget *parent) :
    QWidget(parent)
{
    _icon = icon;
    _event_timer = new QTimer(this);

    QString icon_title = icon->getTitle();
    _icon_title_editor = new QLineEdit(icon_title, this);
    _icon_title_editor->setStyleSheet("border-radius: 3px; ");

    DefaultAdjustement();
}

iWidget::~iWidget()
{
    delete _icon;
    delete _icon_title_editor;
    delete _event_timer;
}

void iWidget::setTitle(const QString &new_title)
{
    _icon->setTitle(new_title);
    _icon_title_editor->setText(_icon->getTitle());
}

void iWidget::DefaultAdjustement() {

    _icon->setBuddy(_icon_title_editor);
    _icon->setStyleSheet("background: transparent;");

    this->setLayout(new QGridLayout(this));

    _event_timer->setSingleShot(true);
    _event_timer->setInterval(200);
    _icon_title_editor->setFont(QFont("Tahoma", 12, QFont::Normal));
    _icon_title_editor->setStyleSheet("background: transparent");
    _icon_title_editor->setFrame(false);
    _icon_title_editor->setReadOnly(true);
    _icon_title_editor->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    _icon_title_editor->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    this->setContentsMargins(4,4,4,4);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->layout()->addWidget(_icon);
    this->layout()->addWidget(_icon_title_editor);
    QSize icon_size = _icon->size();
    this->setBaseSize(icon_size + QSize(icon_size.width(), icon_size.height()));
    this->setFixedSize(this->baseSize());
}

QSize iWidget::sizeHint()
{
    return _icon->isNull() ? QWidget::sizeHint() : this->baseSize();
}

bool iWidget::operator ==(const iWidget &other)
{
    return this->_icon_title_editor->text() == other.getTitle() &&
           this->getType() == other.getType();
}

void iWidget::mousePressEvent(QMouseEvent *ev)
{
    (void) ev;
    if (!_event_timer->isActive()) {
        emit clicked();
        _event_timer->start();
    }
    else emit double_clicked(this);
}

void iWidget::mouseReleaseEvent(QMouseEvent *ev)
{
    (void) ev;
    emit released();
}

//void iWidget::mouseDoubleClickEvent(QMouseEvent *ev)
//{
//    (void) ev;
//    emit double_clicked();
//}

bool iWidget::hasHeightForWidth() const
{
    return true;
}
