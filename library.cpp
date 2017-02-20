#include "library.h"

Library::Library(QWidget *parent) :
    _playlist(&_database)
{
    _parent = parent;

    setDatabaseFileName(DB_FILENAME);
    LoadDatabase();
    SetupPlaylist();
}

Library::Library(const Library& other)
    :_playlist(&_database)
{
    this->_parent = other._parent;
    this->_albums = other._albums;
    this->_playlist = other._playlist;

    this->_database = other._database;
}

Library::~Library()
{
//    // Check for the existence of temp database file
//    QFile db_temp_file;
//    QString working_dir = QDir::currentPath();
//    QDir::setCurrent("/Media");
//    db_temp_file.setFileName(TEMP_DB_FILENAME);
//    if (db_temp_file.exists()) {
//        db_temp_file.remove();
//    }
//    QDir::setCurrent(working_dir);
}

void Library::setDatabaseFileName(const QString &fname)
{
   _database_path = "Media/" + fname;
   _database_fname = fname;
   _database.setDatabaseName(_database_path);
}

void Library::LoadDatabase()
{
    std::fstream dbFile(_database_path.toStdString(), std::ios::in);
    if (!dbFile.good()) {
        qDebug() << "Database file does not exist... creating new one " ;
        dbFile.open("Media/" + DB_FILENAME.toStdString(), std::ios::out);
        dbFile.close();
    }

    _database = QSqlDatabase::addDatabase("QSQLITE");
    _database.setDatabaseName(_database_path);
    if (!_database.open()) {
        qDebug() << _database.lastError();
        return;
    }

    QStringList tableList = _database.tables();

    if (tableList.empty()) {
        // Push a default Untitled(-) album at the beginning of the database
        QSqlQuery query;

        QString table_name = QString("CREATE TABLE IF NOT EXISTS '-'");
        QString create_statement = table_name + "(title TEXT NOT NULL UNIQUE, "
                                                "interpret TEXT, "
                                                "album TEXT, "
                                                "year INTEGER, "
                                                "inPlaylist BOOL NOT NULL DEFAULT 0, "
                                                "path TEXT PRIMARY KEY NOT NULL )";

        if(!query.prepare(create_statement)) {
            qDebug() << "Query not prepared successfully" << query.lastError();
            return;
        }

        if(!query.exec()) {
            qDebug() << "Query not executed successfully" << query.lastError();
            return;
        }
    }

    tableList = _database.tables();

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


void Library::SetupPlaylist()
{
    for (Album& album : _albums) {
        for(Song& song : *(album.getSongs()))
            if (song.is_in_playlist) _playlist.AddMedia(&song);
    }

}

void Library::CopyDatabaseFile(const QString &original_fname, const QString &new_fname)
{
    QFile dbFile;
    QString working_dir = QDir::currentPath();
    if(!QDir::setCurrent( working_dir + "/Media")) {
        qDebug() << "Library::Library -> Directory has not been changed successfully";
    }
    dbFile.setFileName(original_fname);
    if (!dbFile.copy(new_fname)) {
        qDebug() << "Warning! Could not copy the database to the temporary file: " << dbFile.errorString() << endl;
    }
    // Probe to deduce the platform and hide the temporary database file
    #ifdef WINDOWS
       qDebug() << "Windows platform specified but no code provided yet";
       return;
//     hide_temp_database_win();
    #endif
    QDir::setCurrent(working_dir);
}

bool Library::hide_temp_database_win()
{
    std::setlocale(LC_ALL, "");

//      LPCWSTR wstr = L"Media/.temp_media.sqlite";
//      wchar_t array[DB_FILENAME.size()];
//      LPCWSTR wstr = L"Media/" + DB_FILENAME.toWCharArray(array);
//      DWORD attr = GetFileAttributes(wstr);
//      SetFileAttributes(wstr, attr + FILE_ATTRIBUTE_HIDDEN);

    return true;
}

void Library::AddMedia(Album *album) {

    if(!_database.open()) {
        qDebug() << "Database has not been opened: " << _database.lastError();
        return;
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
        qDebug() << "Query not prepared successfully" << query.lastError();
        return;
    }

    if(!query.exec()) {
        qDebug() << "Query not executed successfully" << query.lastError();
        return;
    }

    QVector<Song> songs_to_add;
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

        // Query is not executed if a song is already in the album
        if (!query.exec()) {
            qDebug() << query.lastError();
        }
        else songs_to_add.push_back(song);
    }

    _database.close();

    // If there are duplicite songs, filter them
    if ( album->CountSongs() - songs_to_add.size()) {
        Album* current_album = &(_albums[_albums.indexOf(*album)]);
        for (const Song& song : songs_to_add) {
            current_album->PushSong(song);
        }
    }
    else if (album->getTitle() == "-") {
        for (Song& song : *(album->getSongs()))
            // Untitled songs stored on 0. position of albums
            _albums[0].PushSong(song);
    }
    else {
        _albums.push_back(*album);
    }
}

void Library::RemoveMedia(Album *album)
{

    if(!_database.open()) {
        qDebug() << "in Library::RemoveMedia : " << _database.lastError();
        return;
    }

    QSqlQuery query;
    if (!query.exec("DROP TABLE IF EXISTS '"+album->getTitle()+"'")) {
        qDebug() << "in Library::RemoveMedia : " << query.lastError();
        return;
    }

    _database.close();

    _albums.removeAt(_albums.indexOf(*album));
}

void Library::RemoveMedia(Song *song)
{
    if(!_database.open()) {
        qDebug() << "in Library::RemoveMedia : " << _database.lastError();
        return;
    }
    Album* const album_ptr= song->getAlbum();

    QSqlQuery query;
    if (!query.exec("DELETE FROM '"+album_ptr->getTitle()+"' where title='"+song->getTitle()+"'")) {
        qDebug() << "in Library::RemoveMedia : " << query.lastError();
        return;
    }

    _database.close();


    QVector<Song>* song_vec = album_ptr->getSongs();
    const int index_of_song = song_vec->indexOf(*song);
    song_vec->removeAt(index_of_song);
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

