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
    Playlist(Playlist const& other);
    ~Playlist();

    bool AddMedia(Album* album);

    bool AddMedia(Song*song);

    void AddSample(Song* song);

    void AddSamples(QVector<Song*> const & songs);

    void RemoveSample(Song* const song);

    void RemoveAllSamples();

    bool RemoveMedia(Album* album);

    bool RemoveMedia(Song* song);

    void CacheCurrentIndex() { _cache_index = _media_playlist->currentIndex(); }

    void CacheLastIndex() { _cache_index = _playlist.size() - 1; }

    void setDatabase(QSqlDatabase* const db);

    QSqlDatabase* DatabaseAddress() const { return _database; }

    Song* CurrentMedia();

    QMediaPlaylist* MediaPlaylist() const { return _media_playlist; }

    void setCurrentIndex(int index) {
        _media_playlist->setCurrentIndex(index);
    }

    void Clear();

    bool isSample(Song* const song);

    bool empty() const { return _playlist.isEmpty(); }

    void Update();

    int  Size() const { return _playlist.size(); }

    Song* at(int i);

    Song* operator[] (int i);

private:

    QVector<Song*>  _playlist;
//    QVector<int> _sample_song_indexes;
    QVector<Song*> _samples;

    QMediaPlaylist* _media_playlist;
    QSqlDatabase*   _database;

    int _cache_index = 0;
};

#endif // PLAYLIST_H
