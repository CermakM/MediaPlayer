#include "dragarea.h"

DragArea::DragArea(QWidget* parent) : QWidget(parent)
{
    setAcceptDrops(true);
    setAutoFillBackground(false);

    _brush  = QBrush(QColor(168, 0, 0, 255), Qt::BrushStyle::SolidPattern);
    _pen =  QPen(_brush, 2.5, Qt::SolidLine, Qt::RoundCap);

    _timer.setParent(this);
    _timer.setInterval(50);
    _timer.setSingleShot(true);

    this->setStyleSheet("background: transparent;");
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

    _on_drag = true;
    update();
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

void DragArea::mouseReleaseEvent(QMouseEvent* )
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
    if (_on_drag) {
        QPainter painter(this);
        _buffer = QPixmap(this->size());
        _buffer.fill(QColor(0,0,0,180));
        QPainter pb(&_buffer);
        pb.setPen(_pen);
        QPainterPath path;
        int xcenter = width() / 2;
        int ycenter = height() / 2;
        QSize rect_size (25 % this->size().width(), 100 % this->size().height());
        if ( rect_size.isNull()) return;
        QRect h_rect(QPoint(xcenter - rect_size.width() / 2, ycenter - rect_size.height() / 2), rect_size);
        QRect v_rect(QPoint(xcenter - rect_size.height() / 2, ycenter - rect_size.width() / 2), rect_size.transposed());

        pb.fillRect(h_rect, _brush);
        pb.fillRect(v_rect, _brush);
        path.setFillRule(Qt::WindingFill);
        path.addRect(h_rect);
        path.addRect(v_rect);

        pb.drawPath(path);

        painter.drawPixmap(0,0, _buffer);
        _on_drag = false;
        return;
    }

    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    _buffer = QPixmap(this->size());
    _buffer.fill(QColor(0,0,0,0));

    QPainter pb(&_buffer);
    if (!_rect_selection.isNull()) {
        pb.setCompositionMode(QPainter::CompositionMode_SourceOver);
        pb.setClipRect(this->geometry());
        pb.setPen(_pen);
        pb.drawRect(_rect_selection);
        pb.fillRect(_rect_selection, QColor(168, 0, 0, 50));
    }

    painter.drawPixmap(0,0, _buffer);
}
