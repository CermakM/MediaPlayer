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

const QString DB_FILENAME = "media.sqlite";
const QString TEMP_DB_FILENAME = ".temp_media.sqlite";

class DialogEditLibrary;

class Library
{
public:

    Library(QWidget* parent = 0);
    Library(const Library& other);
    ~Library();

    void setDatabaseFileName(const QString& fname);

    static void CopyDatabaseFile(const QString &original_fname, const QString &new_fname);

    void AddMedia(Album* album);

    void RemoveMedia(Album* album);

    void RemoveMedia(Song* song);

    void infoDatabaseAddress() const;

    QString getDatabaseFName() const {return _database_fname; }

    QString getDatabasePath() const { return _database_path; }

    QVector<Album*> * getAlbums() {return &_albums; }

    Song* getSongByTitle(const QString & title);

    Album* getAlbumByTitle(const QString & title);

    Playlist* getPlaylist() { return &_playlist; }

    int CountAlbums() const { return _albums.size(); }

    bool isSong(const QString& path);

    bool isAlbum(const QString& path);

    bool empty() const { return _albums.size() <= 1 && _albums[0]->getSongs()->empty(); }

    friend class DialogEditLibrary;

    enum ChangeState { NOCHANGE, CHANGE, ADD, REMOVE };

private:

    QSqlDatabase _database;

    QWidget* _parent;

    Playlist _playlist;

    QVector<Album*> _albums;

    QString _database_path;

    QString _database_fname;

    void LoadDatabase();

    void SetupPlaylist();

    bool hide_temp_database_win();

    bool create_temp_database_linux();

};

#endif // LIBRARY_H
