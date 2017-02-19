#ifndef LIBRARY_H
#define LIBRARY_H

#include "probe.h"
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

#include <cstdio>
#include <fstream>

enum Preview {OFF, ON};

class DialogEditLibrary;

class Library
{
public:

    Library(QWidget* parent = 0);
    Library(const Library& other);
    ~Library();

    void AddMedia(Album* album);

    void RemoveMedia(Album* album);

    void RemoveMedia(Song* song);

    void setDatabase(QSqlDatabase new_database);

    QSqlDatabase getDatabase() const { return _database; }

    QVector<Album>* getAlbums() {return &_albums; }

    Song* getSongByTitle(const QString & title);

    Album* getAlbumByTitle(const QString & title);

    Playlist* getPlaylist() { return &_playlist; }

    int CountAlbums() const { return _albums.size(); }

    bool isSong(const QString& path);

    bool isAlbum(const QString& path);

    friend class DialogEditLibrary;

private:

    QSqlDatabase _database;

    QWidget* _parent;

    Playlist _playlist;

    QVector<Album> _albums;

    Preview _preview_state;

    QString _database_path;

    void LoadDatabase();

    void SetupPlaylist();

    bool create_temp_database_win();

    bool create_temp_database_linux();

};

#endif // LIBRARY_H
