#ifndef CUSTOMACTIONRECENT_H
#define CUSTOMACTIONRECENT_H

#include "iwidget.h"

#include <QAction>

class CustomActionRecent : public QAction
{
    Q_OBJECT
public:
    explicit CustomActionRecent(QString const& title, QWidget* parent = 0);
    explicit CustomActionRecent(iWidget* target, QWidget* parent = 0);
    ~CustomActionRecent();

    inline void setWidget(iWidget* const target);

    inline QString text() const { return _title; }

signals:
    void triggered(iWidget*);

private slots:
    void on_triggered(bool);

private:
    iWidget* _target;
    QString _title;
};

#endif // CUSTOMACTIONRECENT_H
