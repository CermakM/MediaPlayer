#include "dragarea.h"

DragArea::DragArea(QWidget* parent) : QWidget(parent)
{
    setAcceptDrops(true);
}

DragArea::~DragArea()
{

}

void DragArea::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
    }
    else
        event->ignore();

    setBackgroundRole(QPalette::Dark);
}

void DragArea::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void DragArea::dropEvent(QDropEvent *event)
{
    const QMimeData* mime_data = event->mimeData();

    event->setDropAction(Qt::DropAction::IgnoreAction);
    event->accept();

    QList<QUrl> url_list = mime_data->urls();

    for (const QUrl& url : url_list) {

        qDebug() << "Dropped file: " << url.toLocalFile();
    }

    clear();
    emit dropped(mime_data);
}

void DragArea::dragLeaveEvent(QDragLeaveEvent *event)
{
    clear();
    event->accept();
}


void DragArea::clear()
{
    // Default setup
    setBackgroundRole(QPalette::NoRole);
}
