#ifndef PLAYLIST_H
#define PLAYLIST_H

#include "album.h"
#include "song.h"

#include <QDebug>
#include <QString>
#include <QVector>
#include <QMediaPlaylist>

#include <boost/variant.hpp>

typedef boost::variant<Album, Song> Media;

class Playlist
{
public:
    Playlist(QObject* parent);

    void AddMedia(Media);

    bool RemoveMedia(Media);

    Song *CurrentMedia();

    QMediaPlaylist* MediaPlaylist() const { return media_playlist; }

    void Clear();

    bool isEmpty() const { return playlist.isEmpty(); }


private:
    void Update();

    bool isSong(Media);

    bool isAlbum(Media);

    QVector<Song>   playlist;

    QMediaPlaylist* media_playlist;

};

#endif // PLAYLIST_H
