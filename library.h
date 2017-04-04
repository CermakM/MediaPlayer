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
#include <QStringList>

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

    enum ChangeState { ERROR = -1, NOCHANGE = 0, CHANGE, ADD, REMOVE };

    void setDatabaseFileName(const QString& fname);

    static void CopyDatabaseFile(const QString &original_fname, const QString &new_fname);

    ChangeState AddMedia(Album* album);

    ChangeState AddMedia(Song* song);

    QMap<Album *, ChangeState> AddMedia(const QVector<Song *> &song_vector);

    void RemoveMedia(Album* album);

    void RemoveMedia(Song* song);

    void infoDatabaseAddress() const;

    void putInDatabase(QSqlQuery &query, Album* const album, Song &song);

    QString getDatabaseFName() const {return _database_fname; }

    QString getDatabasePath() const { return _database_path; }

    /**
     * @brief takeAlbums removes the content of this library and gives up ownership to the caller
     * @return QVector<Album>*
     * \warning The caller is responsible for deleting the content of the vector
     */
    QVector<Album*> takeAlbums();

    QVector<Album*> * getAlbums() {return &_albums; }

    Song* getSongByTitle(const QString & title);

    Album* getAlbumByTitle(const QString & title);

    Album* at(const int& i) { return _albums[i]; }

    Playlist* getPlaylist() { return &_playlist; }

    int CountAlbums() const { return _albums.size(); }

    static bool isSong(const QString& path);

    static bool isAlbum(const QString& path);

    bool contains(Album* const album);

    bool empty() const { return _albums.size() <= 1 && _albums[0]->getSongs()->empty(); }

    friend class DialogEditLibrary;

    bool playlist_updated = false;

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
