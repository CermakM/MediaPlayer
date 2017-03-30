#ifndef IWIDGETMENU_H
#define IWIDGETMENU_H

#include "iwidget.h"

#include <QObject>
#include <QMenu>

class iWidgetMenu : public QMenu
{
    Q_OBJECT
public:
    /* Creates a contextual menu for the widget target
     * @param target context widget
     */
    explicit iWidgetMenu(iWidget* target, QWidget* parent = 0);
    ~iWidgetMenu();

protected:
    void setPosition(QPoint& point);

signals:
    void action_Play_triggered();
    void action_Playlist_triggered();
    void action_Remove_triggered();
    void action_Properties_triggered();
};

#endif // IWIDGETMENU_H
