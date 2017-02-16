#include "iwidget.h"

template<class T> iWidget<T>::iWidget(Icon<T>* icon, QWidget *parent) :
    QWidget(parent)
{
    _main_layout = new QHBoxLayout(this);

    _icon = icon;
    _icon->setParent(_main_layout);

    QString icon_title = '&' + icon->getTitle();
    _icon_title_label = new QLabel(icon_title, this);
    _icon_title_label->setStyleSheet("border-radius: 3px;");

    QObject::connect(_icon, SIGNAL(clicked()), this, SLOT(on_click()));
    QObject::connect(_icon, SIGNAL(double_clicked()), this, SLOT(on_doubleClick()));
    QObject::connect(_icon, SIGNAL(pressed(bool)), this, SLOT(on_press(bool)));

    _icon->setBuddy(_icon_title_label);
    _main_layout->addWidget(_icon);
    _main_layout->addWidget(_icon_title_label);
    _main_layout->SetMinimumSize;
    _main_layout->setObjectName(_icon->getTitle());
}

template<class T> void iWidget<T>::on_click() {
    this->_icon_title_label->setFocus();
    this->_icon_title_label->raise();
}

template<class T> void iWidget<T>::on_doubleClick() {

    // Put into media player or something..
}
