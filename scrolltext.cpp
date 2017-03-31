#include "scrolltext.h"

ScrollText::ScrollText(QWidget *parent) :
    QWidget(parent), _scroll_pos(0)
{
    _staticText.setTextFormat(Qt::PlainText);

    setFixedHeight(fontMetrics().height());
    _left_margin = height() / 3;

    setSeparator("   ---   ");

    connect(&_timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
    _timer.setInterval(50);

    _scroll_enabled = false;
}

QString ScrollText::text() const
{
    return _text;
}

void ScrollText::setText(QString text)
{
    _text = text;
    updateText();
    update();
}


void ScrollText::setTextFlow(bool on)
{
    QFont new_font = font();
    if (on) {
        _scroll_enabled = true;
        new_font.setBold(true);
        setFont(new_font);
    }
    else {
        _timer.stop();
        _scroll_enabled = false;
        new_font.setBold(false);
        setFont(new_font);
    }

    updateText();
    update();
}

QString ScrollText::separator() const
{
    return _separator;
}

void ScrollText::setSeparator(QString separator)
{
    _separator = separator;
    updateText();
    update();
}


void ScrollText::updateText()
{
    _timer.stop();

    _single_text_width = fontMetrics().width(_text);
    _scroll_possible = _single_text_width > width() - _left_margin;

    if(_scroll_enabled && _scroll_possible)
    {
        _scroll_pos = -DEFAULT_SCROLL_OFFSET;
        _staticText.setText(_text + _separator);
        _timer.start();
    }
    else {
        _scroll_pos = -DEFAULT_SCROLL_OFFSET;
        _staticText.setText(_text);
    }

    _staticText.prepare(QTransform(), font());
    _whole_text_size = QSize(fontMetrics().width(_staticText.text()), fontMetrics().height());
}

void ScrollText::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    if(_scroll_possible)
    {
        _buffer.fill(qRgba(0, 0, 0, 0));
        QPainter pb(&_buffer);
        pb.setPen(p.pen());
        pb.setFont(p.font());
        int x = qMin(-_scroll_pos, 0) + _left_margin;
        while(x < width())
        {
            if (_scroll_enabled)
                pb.drawStaticText(QPointF(x, (height() - _whole_text_size.height()) / 2) + QPoint(2, 2), _staticText);
            else
                pb.drawStaticText(QPointF(x, (height() - _whole_text_size.height()) / 2), _staticText);
            x += _whole_text_size.width();
        }

        //Apply Alpha Channel
        pb.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        pb.setClipRect(width() - DEFAULT_ALPHA_WIDTH, 0, DEFAULT_ALPHA_WIDTH, height());
        pb.drawImage(0, 0, _alphaChannel);
        pb.setClipRect(0, 0, DEFAULT_ALPHA_WIDTH, height());
        //initial situation: don't apply alpha channel in the left half of the image at all; apply it more and more until scrollPos gets positive
        if(_scroll_pos < 0)
            pb.setOpacity((qreal)(qMax(-8, _scroll_pos) + 8) / 8.0);
        pb.drawImage(0, 0, _alphaChannel);

        p.drawImage(0, 0, _buffer);
    }
    else
    {
        if (_scroll_enabled) {
            p.drawStaticText(QPointF((width() - _whole_text_size.width()) / 2, (height() - _whole_text_size.height()) / 2) + QPoint(2, 2),
                             _staticText
                             );
        }
        else {
            p.drawStaticText(QPointF((width() - _whole_text_size.width()) / 2, (height() - _whole_text_size.height()) / 2),
                             _staticText
                             );
        }
    }
}

void ScrollText::resizeEvent(QResizeEvent*)
{
    //When the widget is resized, we need to update the alpha channel.

    _alphaChannel = QImage(size(), QImage::Format_ARGB32_Premultiplied);
    _buffer = QImage(size(), QImage::Format_ARGB32_Premultiplied);

    //Create Alpha Channel:
    if(width() > DEFAULT_SCROLL_OFFSET)
    {
        //create first scanline
        QRgb* scanline1 = (QRgb*)_alphaChannel.scanLine(0);
        for(int x = 1; x < DEFAULT_ALPHA_WIDTH + 1; ++x)
            scanline1[x - 1] = scanline1[width() - x] = qRgba(0, 0, 0, x << 4);
        for(int x = DEFAULT_ALPHA_WIDTH; x < width() - DEFAULT_ALPHA_WIDTH; ++x)
            scanline1[x] = qRgb(0, 0, 0);
        //copy scanline to the other ones
        for(int y = 1; y < height(); ++y)
            memcpy(_alphaChannel.scanLine(y), (uchar*)scanline1, width() * 4);
    }
    else
        _alphaChannel.fill(qRgb(0, 0, 0));


    //Update scrolling state
    bool newScrollEnabled = (_single_text_width > width() - _left_margin);
    if(newScrollEnabled != _scroll_possible)
        updateText();
}

void ScrollText::timer_timeout()
{
    _scroll_pos = (_scroll_pos + 2)
                % _whole_text_size.width();
    update();
}
