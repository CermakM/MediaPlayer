#include "playlist.h"

Playlist::Playlist(QSqlDatabase *database, QObject *parent)
{
    _media_playlist = new QMediaPlaylist(parent);
    _database = database;
}

Playlist::Playlist(const Playlist& other)
{
    /*
    this->_media_playlist = new QMediaPlaylist();
    this->_media_playlist->setParent(other._media_playlist->parent());
    this->_media_playlist->addMedia( other._media_playlist->media());
    */
    this->_media_playlist = other._media_playlist;
    this->_database = other._database;
    this->_playlist = other._playlist;
}

Playlist::~Playlist() {

    qInfo() << "Playlist::~Playlist destructor called";
    //delete _media_playlist;
}

bool Playlist::AddMedia(Album* album) {

    if( !_database->open()) {
        qDebug() << "Playlist::AddMedia(Album*) : While opening database :" << _database->lastError();
        return false;
    }

    QSqlQuery query (*_database);
    QString album_title = album->getTitle();
    QString query_string = QString("UPDATE \"%1\" SET inPlaylist = 1").arg(album_title);

    query.prepare(query_string);

    if( !query.exec()) {
        qDebug() << "Album title: " << album_title << endl;
        qDebug() << "Playlist::AddMedia(Album*) : While executing query :" << query.lastError();
        return false;
    }

    _database->close();

    for (Song& song : *(album->getSongs())) {
        if (_playlist.contains(&song)) {
            qDebug() << "Playlist::AddMedia(Album*)... Song is already in playlist \ninterrupting...";
            break;
        }
        song.isInPlaylist(true);
        _playlist.push_back(&song);
    }

    return true;
}

bool Playlist::AddMedia(Song* song) {

    if( !_database->open()) {
        qDebug() << "Playlist::AddMedia(Song*) : While opening database :" << _database->lastError();
        return false;
    }

    QSqlQuery query(*_database);
    QString album_title = song->getAlbumTitle();
    QString query_string = QString("UPDATE \"%1\" SET inPlaylist = 1 WHERE title = \"%2\"")
                           .arg(album_title).arg(song->getTitle());

    query.prepare(query_string);

    if( !query.exec()) {
        qDebug() << "Playlist::AddMedia(Song*) : While executing query :" << query.lastError();
        return false;
    }

    _database->close();

    if (_playlist.contains(song)) {
        qDebug() << "Playlist::AddMedia(Song*)'... Song is already in playlist \ninterrupting...";
        return false;
    }
    song->isInPlaylist(true);
    _playlist.push_back(song);

    return true;
}

void Playlist::PlaySample(Song* song)
{
    if (_playlist.contains(song)) {
        _media_playlist->setCurrentIndex(_playlist.indexOf(song));
        return;
    }

    _cache_index = _media_playlist->currentIndex();
    _sample_song_index = _playlist.size();
    _playlist.push_back(song);
    _media_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(song->getPath())));
    _media_playlist->setCurrentIndex(_sample_song_index);
}

void Playlist::RemoveSample()
{
    if (_sample_song_index < 0) {
        qDebug() << "in Playlist::RemoveSample: Sample media has not been set up";
        return;
    }
    _playlist.removeAt(_sample_song_index);
    _media_playlist->removeMedia(_sample_song_index);
    _media_playlist->setCurrentIndex(_cache_index);
}

bool Playlist::RemoveMedia(Album* album) {

    bool was_removed = false;

    if( !_database->open()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While opening database :" << _database->lastError();
        return false;
    }

    QSqlQuery query (*_database);
    QString album_title = album->getTitle();
    QString query_string = QString("UPDATE \"%1\" SET inPlaylist = 0").arg(album_title);

    query.prepare(query_string);

    if( !query.exec()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While executing query :" << query.lastError();
        return false;
    }

    _database->close();

    for (int i = 0; i < _playlist.size(); ) {
        Song* song = _playlist[i];
        if (song->getAlbumTitle() == album_title) {
            song->isInPlaylist(false);
            _playlist.removeAt(i);
            qDebug() << "Song " << song->getTitle() << " erased";
            was_removed = true;
        }
        else i++;
    }
    return was_removed;
}


bool Playlist::RemoveMedia(Song* song) {

    if( !_database->open()) {
        qDebug() << "Playlist::RemoveMedia(Song*) : While opening database :" << _database->lastError();
        return false;
    }

    QSqlQuery query(*_database);
    QString album_title = song->getAlbumTitle();
    QString query_string = QString("UPDATE \"%1\" SET inPlaylist = 0 WHERE title = \"%2\"")
                           .arg(album_title).arg(song->getTitle());

    query.prepare(query_string);

    if( !query.exec()) {
        qDebug() << "Playlist::RemoveMedia(Song*) : While executing query :" << query.lastError();
        return false;
    }

    _database->close();

    song->isInPlaylist(false);
    bool was_removed = _playlist.removeOne(song);

    if (was_removed) qDebug() << "Song " << song->getTitle() << " erased";

    return was_removed;
}


Song* Playlist::CurrentMedia() {

    int current_index = _media_playlist->currentIndex();

    return (*this)[current_index];
}


void Playlist::Clear() {

    _playlist.clear();

    _media_playlist->clear();
}


void Playlist::Update() {

    _media_playlist->clear();

    for(Song* const song : _playlist) {
        _media_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(song->getPath())));
    }

    if (!empty()) setCurrentIndex(0);
}

Song* Playlist::operator[](int i)
{
    return (i >= 0 && i < _playlist.size()) ? _playlist[i] : nullptr;
}
