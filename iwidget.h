#ifndef ICONPREVIEW_H
#define ICONPREVIEW_H

#include "icon.h"

#include <QWidget>
#include <QBoxLayout>
#include <QObject>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>

class iWidget : public QWidget
{
    Q_OBJECT

public:
    explicit iWidget(QWidget* parent = 0);
    explicit iWidget(Icon* icon, QWidget* parent = 0);

    void Adjust();

    void setTitle(QString const& new_title);

    QString getTitle() const { return _icon_title_label->text(); }

    Icon* getIcon() { return _icon; }

    Type  getType() const { return _icon->getType(); }

    QSize sizeHint();

    QSize size() { return this->size(); }

    bool operator == (const iWidget& other);

public slots:

    void on_click();

    void on_doubleClick();

    void on_press();

private:
    Icon* _icon;
    QLineEdit* _icon_title_label;
};

#endif // ICONPREVIEW_H
