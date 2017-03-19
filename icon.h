#ifndef ICON_H
#define ICON_H

#include "library.h"
#include "media.h"

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>

enum Type {T_NOTYPE, T_ALBUM, T_SONG};

class Icon : public QLabel
{
    Q_OBJECT

public:
   explicit Icon(QWidget* parent = 0);
   ~Icon();
    Icon(Album* media, QWidget* parent = 0);
    Icon(Song* media, QWidget* parent = 0);

   bool CreateLabel(QPixmap* pixmap);

   inline void setSize(const QSize& size) {
       _size = size; this->Update();
   }

   inline QString getTitle() const {
       return _title;
   }

   void setTitle(QString const& new_title);

   void mousePressEvent(QMouseEvent* ev);

   void mouseReleaseEvent(QMouseEvent* ev);

   void mouseDoubleClickEvent(QMouseEvent* ev);

   void Update();

   bool isNull() { return _pixmap == nullptr; }

   QSize size() const {return this->_size; }

   Type getType() const { return _type; }

signals:
   void clicked();

   void double_clicked();

   void pressed();

   void released();


public slots:

   void on_click();

   void on_doubleClick();

   void on_press();

private:
    QString _path_to_media;
    QString _title;
    QPixmap* _pixmap;
    Type _type;
    Media* _media_ptr;

    bool _in_playlist = false;
    bool _clicked = false;

    QSize _size = QSize(50, 50);
};

#endif // ICON_H
