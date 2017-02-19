#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "album.h"
#include "song.h"

#include <QtSql>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QMediaPlaylist>

class Playlist
{
public:
    Playlist(QSqlDatabase* database, QObject* parent = Q_NULLPTR);

    bool AddMedia(Album* album);

    bool AddMedia(Song*);

    bool RemoveMedia(Album* album);

    bool RemoveMedia(Song* song);

    Song* CurrentMedia();

    QMediaPlaylist* MediaPlaylist() const { return _media_playlist; }

    void setCurrentIndex(int index) {
        _media_playlist->setCurrentIndex(index);
    }

    void Clear();

    bool isEmpty() const { return _playlist.isEmpty(); }

    void Update();

    int Size() const { return _playlist.size(); }

    Song* operator[] (int i);


private:

    QVector<Song*>  _playlist;

    QMediaPlaylist* _media_playlist;
    QSqlDatabase*  _database;

};

#endif // PLAYLIST_H
