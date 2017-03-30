#include "iwidgetmenu.h"

iWidgetMenu::iWidgetMenu(QWidget *parent) : QMenu(parent)
{
    // TODO: Set the style of this menu
}

iWidgetMenu::~iWidgetMenu()
{
    qDebug() << "iWidgetMenu destructor called";
}

void iWidgetMenu::setPosition(QPoint& point)
{
    popup(point);
}
