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
    explicit iWidgetMenu(QWidget* parent = 0);
    ~iWidgetMenu();

protected:
    void setPosition(QPoint& point);   
};

#endif // IWIDGETMENU_H
