#include "library.h"

Library::Library(QWidget *_parent) : playlist(&database)
{
    parent = _parent;

    Load();

    SetupPlaylist();

}

Library::~Library()
{

}

void Library::Load()
{
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("Media/media.sqlite");
    if (!database.open()) {
        qDebug() << database.lastError();
        return;
    }

    QStringList tableList = database.tables();

    for (QString& table : tableList) {

        QSqlQuery query("select * from '"+table+"'");

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

        albums.push_back(new_album);
    }

    database.close();
}


void Library::AddMedia(Album *_album) {

    if(!database.open()) {
        qDebug() << "Database has not been opened: " << database.lastError();
        return;
    }

    // If new database was created, push Untitled album in the front
    if(albums.empty()) albums.push_back(Album());

    if (_album->getTitle() == "-") {
        for (Song& song : *(_album->getSongs()))
            // Untitled songs stored on 0. position of albums
            albums[0].PushSong(song);
    }

    QSqlQuery query;
    QString table_name = QString("CREATE TABLE IF NOT EXISTS \"%1\" ").arg(_album->getTitle());
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
    for (Song& song : *(_album->getSongs())) {

        if (song.is_in_playlist) playlist.AddMedia(&song);

        query.prepare("INSERT INTO '"+_album->getTitle()+"' (title, interpret, album, year, inPlaylist, path) "
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

    database.close();

    if(songs_added != _album->CountSongs()) {
        Album* current_album = &(albums[albums.indexOf(*_album)]);
        for (Song& song : *(_album->getSongs())) {
            if ( !current_album->contains(song))
                current_album->PushSong(song);
        }
    }

    else albums.push_back(*_album);
}


void Library::SetupPlaylist()
{
    for (Album& album : albums) {
        for(Song& song : *(album.getSongs()))
            if (song.is_in_playlist) playlist.AddMedia(&song);
    }

}


Song* Library::getSongByTitle(const QString & _title)
{
    Song* song_ptr = nullptr;
    for(Album& album : albums) {
        for( Song& song : *(album.getSongs())) {
            if ( song.getTitle() == _title) {
                song_ptr = &song;
                qDebug() << "Song found: " << song.getTitle() << " | Album: " << song.getAlbumTitle();
                return song_ptr;
            }
        }
    }

    return song_ptr;
}

Album* Library::getAlbumByTitle(const QString & _title)
{
    Album* album_ptr = nullptr;
    for (Album& album : albums) {
        if ( album.getTitle() == _title)
            album_ptr = &album;
    }

    return album_ptr;
}


bool Library::isSong(const QString & _path)
{
    QFileInfo fi(_path);

    if (fi.isFile()) {
        QString suffix = fi.suffix();
        qDebug() << suffix;
        if (suffix == ".mp3")
            return true;
    }

    return false;
}


bool Library::isAlbum(const QString & _path)
{

    QDir dir(_path);

    QStringList filters;
    filters << "*.mp3";
    dir.setNameFilters(filters);
    QFileInfoList entry_info = dir.entryInfoList();

    if(entry_info.empty()) return false;

    return true;
}
