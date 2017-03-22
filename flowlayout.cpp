#include "flowlayout.h"


FlowLayout::FlowLayout(QWidget *parent, int margin, int hSpacing, int vSpacing) : QLayout(parent)
{
    _hSpacing = hSpacing;
    _vSpacing = vSpacing;

    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::FlowLayout(int margin, int hSpacing, int vSpacing)
{
    _hSpacing = hSpacing;
    _vSpacing = vSpacing;

    setContentsMargins(margin, margin, margin, margin);
}

FlowLayout::~FlowLayout()
{
    QLayoutItem* item;
    while( (item = takeAt(0)) )
        delete item;
}

void FlowLayout::addItem(QLayoutItem * item)
{
    _itemList.append(item);
}

int FlowLayout::count() const
{
    return _itemList.size();
}

QLayoutItem* FlowLayout::itemAt(int index) const
{
    return _itemList.value(index);
}

QLayoutItem* FlowLayout::takeAt(int index)
{
    if (index >= 0 && index < _itemList.size()) {
        return _itemList.takeAt(index);
    }
    else return 0;
}

int FlowLayout::horizontalSpacing() const
{
    if (_hSpacing >= 0) {
        return _hSpacing;
    }
    else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

int FlowLayout::verticalSpacing() const
{
    if (_vSpacing >= 0) {
        return _vSpacing;
    }
    else {
        return smartSpacing(QStyle::PM_LayoutHorizontalSpacing);
    }
}

Qt::Orientations FlowLayout::expandingDirections() const
{
    return 0;
}

bool FlowLayout::hasHeightForWidth() const
{
    return true;
}

int FlowLayout::heightForWidth(int width) const
{
    int height = doLayout(QRect(0,0, width, 0));
    return height;
}

void FlowLayout::setGeometry(const QRect &rect)
{
    QLayout::setGeometry(rect);
    doLayout(rect);
}

QSize FlowLayout::sizeHint() const
{
    return minimumSize();
}

QSize FlowLayout::minimumSize() const
{
    QSize size;
    for( QLayoutItem* const item : _itemList ) {
        size = size.expandedTo(item->minimumSize());
    }

    size += QSize(2*margin(), 2*margin());
    return size;
}

int FlowLayout::doLayout(const QRect &rect) const
{
    int left, top, right, bottom;
    getContentsMargins(&left, &top, &right, &bottom);
    QRect effectiveRect = rect.adjusted(+left, +top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;
    for (QLayoutItem* const item : _itemList) {
        QWidget* widget = item->widget();
        int spaceX = horizontalSpacing();
        if (spaceX == -1)
            spaceX = widget->style()->layoutSpacing(
                        QSizePolicy::GroupBox, QSizePolicy::GroupBox, Qt::Horizontal);
        int spaceY = verticalSpacing();
        if (spaceY == -1)
            spaceY = widget->style()->layoutSpacing(
                        QSizePolicy::GroupBox, QSizePolicy::GroupBox, Qt::Vertical);
        int nextX = x + item->sizeHint().width() + spaceX;
        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y = y + lineHeight + spaceY;
            nextX = x + item->sizeHint().width() + spaceX;
            lineHeight = 0;
        }

        item->setGeometry(QRect(QPoint(x,y), item->sizeHint()));

        x = nextX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }

    return y + lineHeight - rect.y() + bottom;
}

int FlowLayout::smartSpacing(QStyle::PixelMetric pixel_metric) const
{
    QObject* parent = this->parent();
    if (!parent) {
        return -1;
    }
    else if (parent->isWidgetType()) {
        QWidget* widget_ptr = static_cast<QWidget*>(parent);
        return widget_ptr ->style()->pixelMetric(pixel_metric, 0, widget_ptr);
    }
    else {
        return static_cast<QLayout*>(parent)->spacing();
    }
}
