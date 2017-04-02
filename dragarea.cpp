#include "dragarea.h"

DragArea::DragArea(QWidget* parent) : QWidget(parent)
{
    setAcceptDrops(true);

    _brush  = QBrush(QColor(65, 70, 115, 255), Qt::BrushStyle::SolidPattern);
    _pen =  QPen(_brush, 2.5, Qt::SolidLine, Qt::RoundCap);

    _timer.setParent(this);
    _timer.setInterval(50);
    _timer.setSingleShot(true);
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

void DragArea::mousePressEvent(QMouseEvent *event)
{
    emit clicked(event);
    if (Qt::RightButton == event->buttons()) return;
    _mouse_pressed = true;
    _timer.start();
    _rect_selection.setTopLeft(event->pos());
    _rect_selection.setBottomRight(event->pos());
}


void DragArea::clear()
{
    // Default setup
    setBackgroundRole(QPalette::NoRole);
}

void DragArea::mouseReleaseEvent(QMouseEvent* event)
{
    _mouse_pressed = false;
    if ( !_rect_selection.width()) return;
    emit selected(_rect_selection);
    _rect_selection = QRect();
    update();
}

void DragArea::mouseMoveEvent(QMouseEvent *event)
{
    if (_mouse_pressed) {
        if (!_timer.isActive()) {
            _rect_selection.setBottomRight(event->pos());
            this->update();
        }
    }
}

void DragArea::paintEvent(QPaintEvent *)
{
    _buffer = QImage(size(), QImage::Format_ARGB32_Premultiplied);
    _buffer.fill(qRgba(0,0,0,0));

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QPainter painter_buffer(&_buffer);
    painter_buffer.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter_buffer.setClipRect(this->geometry());
    painter_buffer.setPen(_pen);
    painter_buffer.drawRect(_rect_selection);
    painter_buffer.fillRect(_rect_selection, QColor(65, 70, 115, 50));

    painter.drawImage(0,0, _buffer);
}
