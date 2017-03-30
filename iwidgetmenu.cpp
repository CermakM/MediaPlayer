#include "iwidgetmenu.h"

iWidgetMenu::iWidgetMenu(iWidget *target, QWidget *parent) : QMenu(parent)
{
    if (!target)
        return;

    if (Type::T_SONG == target->getType())
        addAction("Play", this, SIGNAL(action_Play_triggered()));
    else {
        addAction("Explore", this, SIGNAL(action_Play_triggered()));
    }
    addAction("Add to Playlist", this, SIGNAL(action_Playlist_triggered()));
    addAction("Remove", this, SIGNAL(action_Remove_triggered()));
    addAction("Properties", this, SIGNAL(action_Properties_triggered()));
}

iWidgetMenu::~iWidgetMenu()
{
    qDebug() << "iWidgetMenu destructor called";
}

void iWidgetMenu::setPosition(QPoint& point)
{
    popup(point);
}
