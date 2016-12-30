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
    Playlist(QObject* parent = Q_NULLPTR);

    void AddMedia(Media);

    /**
     * If the media is album, removes all songs of the album,
     * else removes the song specified in param.
     *
     * @param media of Song or Album type to be removed
     * @return true if removed succesfully
    */
    bool RemoveMedia(Media);

    Song *CurrentMedia();

    QMediaPlaylist* MediaPlaylist() const { return media_playlist; }

    void setCurrentIndex(int index) {
        media_playlist->setCurrentIndex(index);
    }

    void Clear();

    bool isEmpty() const { return playlist.isEmpty(); }

    void Update();

    int Size() const { return playlist.size(); }

    Song* operator[] (int i) { return &playlist[i]; }


private:

    bool isSong(Media);

    bool isAlbum(Media);

    QVector<Song>   playlist;

    QMediaPlaylist* media_playlist;

};

#endif // PLAYLIST_H
