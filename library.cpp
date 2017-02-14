#include "library.h"

Library::Library(QWidget *parent) : _playlist(&_database)
{
    _parent = parent;

    Load();

    SetupPlaylist();

}

Library::~Library()
{

}

void Library::Load()
{
    _database = QSqlDatabase::addDatabase("QSQLITE");
    _database.setDatabaseName("Media/media.sqlite");
    if (!_database.open()) {
        qDebug() << _database.lastError();
        return;
    }

    QStringList tableList = _database.tables();

    for (QString& table : tableList) {

        QSqlQuery query("SELECT * FROM '"+table+"'");

        query.exec();

        QString path, title, interpret, album;
        int year;
        bool inPlaylist;

        QVector<Song> songs;

        while(query.next()) {

            QSqlRecord rec = query.record();

            title = rec.value("title").toString();
            interpret = rec.value("interpret").toString();
            album = rec.value("album").toString();
            year = rec.value("year").toInt();
            inPlaylist = rec.value("inPlaylist").toBool();
            path = rec.value("path").toString();

            songs.push_back(Song(title, path, interpret, album, year, inPlaylist));
        }

        Album new_album(songs);
        new_album.setTitle(table);

        _albums.push_back(new_album);
    }

    _database.close();
}


void Library::AddMedia(Album *album) {

    if(!_database.open()) {
        qDebug() << "Database has not been opened: " << _database.lastError();
        return;
    }

    // If new database was created, push Untitled album in the front
    if(_albums.empty()) _albums.push_back(Album());

    if (album->getTitle() == "-") {
        for (Song& song : *(album->getSongs()))
            // Untitled songs stored on 0. position of albums
            _albums[0].PushSong(song);
    }

    QSqlQuery query;
    QString table_name = QString("CREATE TABLE IF NOT EXISTS \"%1\" ").arg(album->getTitle());
    QString create_statement = table_name + "(title TEXT NOT NULL UNIQUE, "
                                            "interpret TEXT, "
                                            "album TEXT, "
                                            "year INTEGER, "
                                            "inPlaylist BOOL NOT NULL DEFAULT 0, "
                                            "path TEXT PRIMARY KEY NOT NULL )";

    if(!query.prepare(create_statement)) {
        qDebug() << "Query not prepared succesfully" << query.lastError();
        return;
    }

    if(!query.exec()) {
        qDebug() << query.lastError();
        return;
    }

    int songs_added = 0;
    for (Song& song : *(album->getSongs())) {

        if (song.is_in_playlist) _playlist.AddMedia(&song);

        query.prepare("INSERT INTO '"+album->getTitle()+"' (title, interpret, album, year, inPlaylist, path) "
                                                         "VALUES (:title, :interpret, :album, :year, :inPlaylist, :path)");
        query.bindValue(":title", song.getTitle());
        query.bindValue(":interpret", song.getInterpret());
        query.bindValue(":album", song.getAlbumTitle());
        query.bindValue(":year", song.getYear());
        query.bindValue(":inPlaylist", song.is_in_playlist);
        query.bindValue(":path", song.getPath());

        if (!query.exec()) qDebug() << query.lastError();
        else songs_added++;
    }

    _database.close();

    if(songs_added != album->CountSongs()) {
        Album* current_album = &(_albums[_albums.indexOf(*album)]);
        for (Song& song : *(album->getSongs())) {
            if ( !current_album->contains(song))
                current_album->PushSong(song);
        }
    }

    else _albums.push_back(*album);
}


void Library::SetupPlaylist()
{
    for (Album& album : _albums) {
        for(Song& song : *(album.getSongs()))
            if (song.is_in_playlist) _playlist.AddMedia(&song);
    }

}


Song* Library::getSongByTitle(const QString & title)
{
    Song* song_ptr = nullptr;
    for(Album& album : _albums) {
        for( Song& song : *(album.getSongs())) {
            if ( song.getTitle() == title) {
                song_ptr = &song;
                qDebug() << "Song found: " << song.getTitle() << " | Album: " << song.getAlbumTitle();
                return song_ptr;
            }
        }
    }

    return song_ptr;
}

Album* Library::getAlbumByTitle(const QString & title)
{
    Album* album_ptr = nullptr;
    for (Album& album : _albums) {
        if ( album.getTitle() == title)
            album_ptr = &album;
    }

    return album_ptr;
}


bool Library::isSong(const QString & path)
{
    QFileInfo fi(path);

    if (fi.isFile()) {
        QString suffix = fi.suffix();
        qDebug() << suffix;
        if (suffix == ".mp3")
            return true;
    }

    return false;
}


bool Library::isAlbum(const QString & path)
{

    QDir dir(path);

    QStringList filters;
    filters << "*.mp3";
    dir.setNameFilters(filters);
    QFileInfoList entry_info = dir.entryInfoList();

    if(entry_info.empty()) return false;

    return true;
}
