#ifndef DRAGAREA_H
#define DRAGAREA_H

#include <QWidget>
#include <QDebug>
#include <QMimeData>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QTimer>

#include <QPainter>

class DragArea : public QWidget
{
    Q_OBJECT

public:
    DragArea(QWidget* parent = 0);
    ~DragArea();

    inline QRect getRectSelection() const { return _rect_selection; }

public slots:
    void clear();

signals:
    void dropped(const QMimeData* mime_data = 0);
    void selected(QRect&);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    QString _default_message;

    QTimer  _timer;
    QBrush  _brush;
    QPen    _pen;
    QImage  _buffer;
    QRect   _rect_selection;

    bool _mouse_pressed = false;
};

#endif // DRAGAREA_H
