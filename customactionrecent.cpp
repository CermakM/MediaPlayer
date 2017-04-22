#include "customactionrecent.h"

CustomActionRecent::CustomActionRecent(const QString &title, QWidget *parent) : QAction(parent)
{
    setText((_title = title));
}

CustomActionRecent::CustomActionRecent(iWidget *target, QWidget *parent) : QAction(parent)
{
    _target = target;
    setText((_title = target->getAlbumTitle() + "  -  " + target->getTitle()));
    connect(this, SIGNAL(triggered(bool)), this, SLOT(on_triggered(bool)));
}

CustomActionRecent::~CustomActionRecent()
{
}

void CustomActionRecent::setWidget(iWidget * const target)
{
    _target = target;
    setText((_title = target->getAlbumTitle() + "  -  " + target->getTitle()));
}

void CustomActionRecent::on_triggered(bool)
{
    emit triggered(_target);
}
