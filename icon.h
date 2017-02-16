#ifndef ICON_H
#define ICON_H

#include "library.h"

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QEvent>
#include <QDebug>

enum Type {T_NOTYPE, T_ALBUM, T_SONG};

class iSignalSlots : public QObject {
    Q_OBJECT

public:
    explicit iSignalSlots(QObject* parent = 0) : QObject(parent) {}

signals:
    void clicked();
    void double_clicked();
    void pressed(bool icon_pressed);

public slots:
    virtual void on_click() = 0;
    virtual void on_doubleClick() = 0;
    virtual void on_press(bool icon_pressed) = 0;

};

template <class T>
class Icon : public QLabel, iSignalSlots
{
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
       return _media_ptr ? _media_ptr->getTitle() : nullptr;
   }

   void mousePressEvent(QMouseEvent* ev);

   void mouseReleaseEvent(QMouseEvent* ev);

   void mouseDoubleClickEvent(QMouseEvent* ev);

   void Update();

private:
    QString _path_to_media;
    QPixmap* _pixmap;
    QLabel* _label;
    Type _type;
    T* _media_ptr;

    bool _in_playlist = false;
    bool _clicked = false;

    QSize _size = QSize(32, 32);


};

#endif // ICON_H
