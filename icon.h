#ifndef ICON_H
#define ICON_H

#include "library.h"

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>

#include <QPainter>
#include <QBrush>

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

   void setTitle(QString const& new_title);

   inline QString getTitle() const { return _title; }

   inline Type getType() const { return _type; }

   void Update();

   inline bool isNull() { return _pixmap == nullptr; }

   inline QSize size() const {return this->_size; }

public slots:

   void on_click();

private:
    QString _path_to_media;
    QString _title;
    QPixmap* _pixmap;
    Type _type;

    bool _in_playlist = false;
    bool _clicked = false;

    QSize _size = QSize(50, 50);
};

#endif // ICON_H
