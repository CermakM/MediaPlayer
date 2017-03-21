#ifndef ICONPREVIEW_H
#define ICONPREVIEW_H

#include "icon.h"

#include <QWidget>
#include <QBoxLayout>
#include <QObject>
#include <QLabel>
#include <QLineEdit>
#include <QSpacerItem>
#include <QBitmap>

class iWidget : public QWidget
{
    Q_OBJECT

public:
    explicit iWidget(QWidget* parent = 0);
    explicit iWidget(Icon* icon, QWidget* parent = 0);

    void DefaultAdjustement();

    void setTitle(QString const& new_title);

    QLineEdit* getTitleEditor() const { return _icon_title_editor; }

    QString getTitle() const { return _icon_title_editor->text(); }

    Icon* getIcon() { return _icon; }

    Type  getType() const { return _icon->getType(); }

    QSize sizeHint();

    QSize size() { return this->size(); }

    bool operator == (const iWidget& other);

    void mousePressEvent(QMouseEvent* ev);

    void mouseReleaseEvent(QMouseEvent* ev);

    void mouseDoubleClickEvent(QMouseEvent* ev);

signals:

    void clicked();

    void double_clicked();

    void pressed();

    void released();

private:
    Icon* _icon;
    QLineEdit* _icon_title_editor;
};

#endif // ICONPREVIEW_H
