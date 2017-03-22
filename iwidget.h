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

    inline void setTitle(QString const& new_title);

    inline QLineEdit* getTitleEditor() const { return _icon_title_editor; }

    inline QString getTitle() const { return _icon_title_editor->text(); }

    inline Icon* getIcon() { return _icon; }

    inline Type  getType() const { return _icon->getType(); }

    inline QString getPath() const { return _icon->getPath(); }

    QSize sizeHint();

    QSize size() { return this->size(); }

    bool operator == (const iWidget& other);

    void mousePressEvent(QMouseEvent* ev);

    void mouseReleaseEvent(QMouseEvent* ev);

    void mouseDoubleClickEvent(QMouseEvent* ev);

    bool hasHeightForWidth() const;

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
