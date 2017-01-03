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
    Playlist(QSqlDatabase* _database, QObject* parent = Q_NULLPTR);

    bool AddMedia(Album* _album);

    bool AddMedia(Song*);

    bool RemoveMedia(Album* _album);

    bool RemoveMedia(Song* _song);

    Song* CurrentMedia();

    QMediaPlaylist* MediaPlaylist() const { return media_playlist; }

    void setCurrentIndex(int index) {
        media_playlist->setCurrentIndex(index);
    }

    void Clear();

    bool isEmpty() const { return playlist.isEmpty(); }

    void Update();

    int Size() const { return playlist.size(); }

    Song* operator[] (int i);


private:

    QVector<Song*>  playlist;

    QMediaPlaylist* media_playlist;
    QSqlDatabase* database;

};

#endif // PLAYLIST_H
