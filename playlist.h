#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "album.h"
#include "song.h"

#include <QDebug>
#include <QString>
#include <QVector>
#include <QMediaPlaylist>

class Playlist
{
public:
    Playlist(QObject* parent = Q_NULLPTR);

    void AddMedia(Album* _album);

    void AddMedia(Song*);

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

    Song* operator[] (int i) { return playlist[i]; }


private:

    QVector<Song*>  playlist;

    QMediaPlaylist* media_playlist;

};

#endif // PLAYLIST_H
