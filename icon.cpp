#include "icon.h"

template <class T> Icon<T>::Icon(QWidget* parent) : QLabel(parent) {

    _pixmap = new QPixmap(":/icons/icon_empty_folder");

    qDebug() << "Invalid or unspecified media passed to Icon constructor";
    qDebug() << "Empty folder will be created";

    _path_to_media = "Unspecified";
    _media_ptr = nullptr;

    CreateLabel(_pixmap);
}

template<> Icon<Album>::Icon (Album *media, QWidget *parent) : QLabel(parent) {
    _media_ptr = media;
    Album* album_ptr = media;

    if(album_ptr->CurrentIcon()) {
        QString file_path = album_ptr->getIcons()->front().absoluteFilePath();
        _pixmap = new QPixmap(file_path);
    }
    else {
        _pixmap = new QPixmap(":/icons/icon_album");
    }

    _path_to_media = album_ptr->getPath();

    CreateLabel(_pixmap);
}

template<> Icon<Song>::Icon(Song *media, QWidget *parent) : QLabel(parent) {

    _media_ptr = media;
    Song* song_ptr = media;

    _pixmap = new QPixmap(":/icons/icon_song");

    _in_playlist = song_ptr->is_in_playlist;

    _path_to_media = song_ptr->getPath();

    CreateLabel(_pixmap);
}


template<class T> Icon<T>::~Icon() {

    delete _label;
    delete _pixmap;
}


template<class T> bool Icon<T>::CreateLabel(QPixmap* pixmap) {
    if (!pixmap) {
        qDebug() << "Error creating QLabel: QPixmap = nullptr\n";
        return false;
    }

    _label = new QLabel();

    _label->setPixmap(*pixmap);

    return true;
}

template<class T> void Icon<T>::mousePressEvent(QMouseEvent *ev)
{
    (void) ev;
    _clicked = !_clicked;
    emit clicked();
    emit pressed(true);
}

template<class T> void Icon<T>::mouseReleaseEvent(QMouseEvent *ev)
{
    (void) ev;
    emit pressed(false);
}

template<class T> void Icon<T>::mouseDoubleClickEvent(QMouseEvent *ev)
{
    (void) ev;
    emit double_clicked();
}

template<class T> void Icon<T>::Update() {

    _label->setMaximumSize(_size);
}
