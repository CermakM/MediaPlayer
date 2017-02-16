#ifndef ICONPREVIEW_H
#define ICONPREVIEW_H

#include "icon.h"

#include <QHBoxLayout>
#include <QWidget>
#include <QObject>
#include <QLabel>

template<class T>
class iWidget : QWidget, iSignalSlots
{
public:
    explicit iWidget(Icon<T>* icon, QWidget* parent = 0);

private:
    QHBoxLayout* _main_layout;
    Icon<T>* _icon;
    QLabel* _icon_title_label;

public slots:
    void on_click();
    void on_doubleClick();
};

#endif // ICONPREVIEW_H
