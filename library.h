#ifndef LIBRARY_H
#define LIBRARY_H

#include "album.h"
#include "song.h"
#include "playlist.h"

#include <QtSql>
#include <QVector>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>


class Library
{
public:

    Library(QWidget* parent);
    ~Library();

    void Clear();

    void AddMedia(Album* album);

    //void AddMedia(Song* _song);

    void Load();

    void SetupPlaylist();

    QVector<Album>* Albums() {return &_albums; }

    Song* getSongByTitle(const QString & title);

    Album* getAlbumByTitle(const QString & title);

    Playlist* getPlaylist() { return &_playlist; }

    int CountAlbums() const { return _albums.size(); }

    bool isSong(const QString& path);

    bool isAlbum(const QString& path);


private:

    QSqlDatabase _database;

    QWidget* _parent;

    Playlist _playlist;

    QVector<Album> _albums;

};

#endif // LIBRARY_H
