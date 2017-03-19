#ifndef DRAGAREA_H
#define DRAGAREA_H

#include <QWidget>
#include <QMimeData>

class DragArea : public QWidget
{
    Q_OBJECT

public:
    DragArea(QWidget* parent = 0);

public slots:
    void clear();

signals:
    void changed(const QMimeData *mimeData = 0);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

private:
    QString _default_message;
};

#endif // DRAGAREA_H
