/**
  * This class has been reimplemented from an idea of "leemes"
  * original on: http://stackoverflow.com/questions/10651514/text-scrolling-marquee-in-qlabel
  */

#ifndef SCROLLTEXT_H
#define SCROLLTEXT_H

#include <QWidget>
#include <QStaticText>
#include <QTimer>
#include <QPainter>
#include <QDebug>

#define DEFAULT_SCROLL_OFFSET 64
#define DEFAULT_ALPHA_WIDTH 15

class ScrollText : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText)
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)

public:
    explicit ScrollText(QWidget *parent = 0);

    void setTextFlow(bool on);

public slots:
    QString text() const;
    void setText(QString text);

    QString separator() const;
    void setSeparator(QString separator);


protected:
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);

private:
    void updateText();
    QString _text;
    QString _separator;
    QStaticText _staticText;
    QSize _whole_text_size;
    int  _single_text_width;
    int  _left_margin;
    int  _scroll_pos;
    bool _scroll_enabled;
    bool _scroll_possible;
    QImage _alphaChannel;
    QImage _buffer;
    QTimer _timer;

private slots:
    virtual void timer_timeout();
};

#endif // SCROLLTEXT_H
