#include "library.h"

Library::Library(QWidget *parent) :
    _playlist(&_database)
{
    _parent = parent;

    setDatabaseFileName(DB_FILENAME);
    LoadDatabase();
    SetupPlaylist();
}

Library::Library(const Library& other):
    _playlist(other._playlist)
{
    this->_parent = other._parent;
    this->_albums = other._albums;
    this->_database = QSqlDatabase::cloneDatabase(other._database, "QSQLITE");
}

Library::~Library()
{
    qInfo() << "Library::~Library destructor called" ;
    _playlist.Clear();
    while(!_albums.empty()) {
        delete _albums.takeFirst();
    }
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
        QSqlQuery query(_database);

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

        tableList = _database.tables();
    }

    for (QString& table : tableList) {

        QSqlQuery query(_database);

        query.exec("SELECT * FROM '"+table+"'");

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

        Album* new_album = new Album(songs);

        new_album->setTitle(table);

        _albums.push_back(new_album);
    }

    _database.close();
}


void Library::SetupPlaylist()
{
    for (Album* const album : _albums) {
        for(Song& song : *(album->getSongs()))
            if (*song.isInPlaylist()) _playlist.AddMedia(&song);
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

Library::ChangeState Library::AddMedia(Album *album) {

    if(!_database.open()) {
        qDebug() << "Database has not been opened: " << _database.lastError();
        return ERROR;
    }

    QSqlQuery query(_database);

    QString table_name = QString("CREATE TABLE IF NOT EXISTS \"%1\" ").arg(album->getTitle());
    QString create_statement = table_name + "(title TEXT NOT NULL UNIQUE, "
                                            "interpret TEXT, "
                                            "album TEXT, "
                                            "year INTEGER, "
                                            "inPlaylist BOOL NOT NULL DEFAULT 0, "
                                            "path TEXT PRIMARY KEY NOT NULL )";


    this->infoDatabaseAddress();

    if(!query.prepare(create_statement)) {
        qDebug() << "Query not prepared successfully" << query.lastError();
        return ERROR;
    }

    if(!query.exec()) {
        qDebug() << "Query not executed successfully" << query.lastError();
        return ERROR;
    }

    QVector<Song> songs_to_add;
    for (Song& song : *(album->getSongs())) {

        if (*song.isInPlaylist()) _playlist.AddMedia(&song);

        QSqlQuery query(_database);

        putInDatabase(query, album, song);

        // Query is not executed if a song is already in the album
        if (!query.exec()) {
            qDebug() << query.lastError();
        }
        else songs_to_add.push_back(song);
    }

    _database.close();

    // If there are duplicite songs, filter them
    if ( _albums.contains(album) ) {
        Album* current_album = _albums[_albums.indexOf(album)];
        if (songs_to_add.empty())
            return Library::NOCHANGE;

        for (Song& song : songs_to_add) {
            current_album->PushSong(song);
        }
        return Library::CHANGE;
    }
    else if (album->getTitle() == "-") {
        for (Song& song : *(album->getSongs()))
            // Untitled songs stored on 0. position of albums
            _albums[0]->PushSong(song);
        return Library::CHANGE;
    }
    else {
        // Search for the index to put album to
        int album_index = 0;
        for (int i = 0; i < _albums.size(); i ++) {
            Album* const temp_album = _albums.at(i);
            // If the first one is further in alphabet
            if (temp_album->getTitle().compare(album->getTitle()) > 0) {
                break;
            }
            ++album_index;
        }
        _albums.insert(album_index, album);
        return Library::ADD;
    }
}

void Library::putInDatabase(QSqlQuery& query, Album* const album, Song& song)
{
    query.prepare("INSERT INTO '"+album->getTitle()+"' (title, interpret, album, year, inPlaylist, path) "
                                                     "VALUES (:title, :interpret, :album, :year, :inPlaylist, :path)");
    query.bindValue(":title", song.getTitle());
    query.bindValue(":interpret", song.getInterpret());
    query.bindValue(":album", song.getAlbumTitle());
    query.bindValue(":year", song.getYear());
    query.bindValue(":inPlaylist", *song.isInPlaylist());
    query.bindValue(":path", song.getPath());
}


Library::ChangeState Library::AddMedia(Song *song)
{
    QVector<Song*> temp_vector { song };

    return AddMedia(temp_vector).begin().value();
}

QMap<Album*, Library::ChangeState> Library::AddMedia(QVector<Song*> const& song_vector)
{
    QMap<Album*, Library::ChangeState> map_changes;
    for(Song* const song : song_vector) {
        Album* album = getAlbumByTitle(song->getAlbumTitle());
        if (album == nullptr) {
            Album* album = new Album();
            album->PushSong(*song);
            album->setInterpret(song->getInterpret());
            album->setTitle(song->getAlbumTitle());
            if (!map_changes.contains(album))
                map_changes[album] = AddMedia(album);
        }
        else if (!album->contains(*song)){
            album->PushSong(*song);
            album->setInterpret(song->getInterpret());
            album->setTitle(song->getAlbumTitle());

            if ( !_database.open()) {
                qDebug() << "in Library::AddMedia(QVector<Song*> const&): Database "
                            "has not been opened " << _database.lastError();
            }

            QSqlQuery query(_database);

            putInDatabase(query, album, *song);

            if(!query.exec()) {
                qDebug() << "Query not executed successfully" << query.lastError();
            }

            _database.close();
            if (!map_changes.contains(album))
                map_changes[album] = Library::CHANGE;
        }
        else map_changes[nullptr] = Library::NOCHANGE;
    }

    return map_changes;
}

void Library::RemoveMedia(Album *album)
{
    if (album->getTitle() == '-') {
        qDebug() << "Trying to remove default album for songs without an album, which is not possible";
        return;
    }

    _playlist.RemoveMedia(album);

    if(!_database.open()) {
        qDebug() << "in Library::RemoveMedia : " << _database.lastError();
        return;
    }

    if(!album) {
        qDebug() << "Error in Library::RemoveMieda(Album*) : could not find a corresponding media for null";
        return;
    }


    QSqlQuery query(_database);
    if (!query.exec("DROP TABLE IF EXISTS '"+album->getTitle()+"'")) {
        qDebug() << "in Library::RemoveMedia : " << query.lastError();
        return;
    }

    _database.close();
   delete _albums.takeAt(_albums.indexOf(album));
}

void Library::RemoveMedia(Song *song)
{
    if(!song) {
        qDebug() << "Error in Library::RemoveMieda(Song*) : could not find a corresponding media for null";
        return;
    }

    _playlist.RemoveMedia(song);

    Album* album_ptr= song->getAlbum();

    if(!album_ptr) {
        qDebug() << "Laughing: every song has to have an album... enjoy the segmentation fault!";
    }

    if(!_database.open()) {
        qDebug() << "in Library::RemoveMedia : " << _database.lastError();
        return;
    }

    QSqlQuery query(_database);
    if (!query.exec("DELETE FROM '"+album_ptr->getTitle()+"' where title='"+song->getTitle()+"'")) {
        qDebug() << "in Library::RemoveMedia : " << query.lastError();
        return;
    }

    _database.close();

    QVector<Song>* song_vec = album_ptr->getSongs();
    song_vec->removeOne(*song);

}

void Library::infoDatabaseAddress() const
{
    qDebug() << "\tDatabase stored in library: " << _database << " on adress " << &_database;
    qDebug() << "\tDatabase address stored in playlist: " << *_playlist.DatabaseAddress() << " on adress " << _playlist.DatabaseAddress();
}

Song* Library::getSongByTitle(const QString & title)
{
    Song* song_ptr = nullptr;
    for(Album* const album : _albums) {
        for( Song& song : *(album->getSongs())) {
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
    for (Album* const album : _albums) {
        if ( album->getTitle() == title)
            album_ptr = album;
    }

    return album_ptr;
}

bool Library::isSong(const QString & path)
{
    QFileInfo fi(path);

    if (fi.isFile()) {
        QString suffix = fi.suffix();
        if (suffix == "mp3")
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

bool Library::contains(Album * const album)
{
    QVector<Album*>::iterator it = _albums.begin();

    while (it != _albums.end()) {
        if ((*it)->getTitle() == album->getTitle() && (*it)->getInterpret() == album->getInterpret()) {
            return true;
        }
        ++it;
    }
    return false;
}
