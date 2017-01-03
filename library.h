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

    Library(QWidget* _parent);
    ~Library();

    void Clear();

    void AddMedia(Album* _album);

    void AddMedia(Song* _song);

    void Load();

    void SetupPlaylist();

    QVector<Album>* Albums() {return &albums; }

    Song* getSongByTitle(const QString & _title);

    Album* getAlbumByTitle(const QString & _title);

    Playlist* getPlaylist() { return &playlist; }

    int CountAlbums() const { return albums.size(); }

    bool isSong(const QString& _path);

    bool isAlbum(const QString& _path);


private:

    QSqlDatabase database;

    QWidget* parent;

    Playlist playlist;

    QVector<Album> albums;

};

#endif // LIBRARY_H
