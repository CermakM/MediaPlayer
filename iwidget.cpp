#include "iwidget.h"

iWidget::iWidget(QWidget* parent) :
    QWidget(parent)
{
    _icon = new Icon();
    _icon_title_label = new QLineEdit("Unknown", this);

    Adjust();
}

iWidget::iWidget(Icon *icon, QWidget *parent) :
    QWidget(parent)
{
    _icon = icon;

    QString icon_title = icon->getTitle();
    _icon_title_label = new QLineEdit(icon_title, this);
    _icon_title_label->setStyleSheet("border-radius: 3px; ");
    Adjust();

    QObject::connect(_icon, SIGNAL(clicked()), this, SLOT(on_click()));
    QObject::connect(_icon, SIGNAL(double_clicked()), this, SLOT(on_doubleClick()));
    QObject::connect(_icon, SIGNAL(pressed()), this, SLOT(on_press()));

    _icon->setBuddy(_icon_title_label);

    Adjust();
}

void iWidget::setTitle(const QString &new_title)
{
    _icon->setTitle(new_title);
    _icon_title_label->setText(_icon->getTitle());
}

void iWidget::Adjust() {
    this->setLayout(new QGridLayout(this));

    _icon_title_label->setStyleSheet("font: 14px Tahoma;"
                                     "background: transparent;"
                                     );
    _icon_title_label->setFrame(false);
    _icon_title_label->setReadOnly(true);
    _icon_title_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    _icon_title_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    this->setContentsMargins(4,4,4,4);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    this->layout()->addWidget(_icon);
    this->layout()->addWidget(_icon_title_label);
    QSize icon_size = _icon->size();
    this->setBaseSize(icon_size + QSize(icon_size.width(), icon_size.height() - 5));
    this->setFixedSize(this->baseSize());
}

QSize iWidget::sizeHint()
{
    return _icon->isNull() ? QWidget::sizeHint() : _icon->size() + QSize(5, 5);
}

bool iWidget::operator ==(const iWidget &other)
{
    return this->_icon_title_label->text() == other.getTitle() &&
           this->getType() == other.getType();
}

void iWidget::on_click() {
    this->_icon_title_label->setFocus();
    this->_icon_title_label->raise();
}

void iWidget::on_doubleClick() {

    // Put into media player or something..
}

void iWidget::on_press()
{
    // Used for dragging I suppose
}
