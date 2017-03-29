#ifndef DRAGAREA_H
#define DRAGAREA_H

#include <QWidget>
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>

class DragArea : public QWidget
{
    Q_OBJECT

public:
    DragArea(QWidget* parent = 0);
    ~DragArea();

public slots:
    void clear();

signals:
    void dropped(const QMimeData* mime_data = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

private:
    QString _default_message;
};

#endif // DRAGAREA_H
