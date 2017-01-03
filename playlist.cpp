#include "playlist.h"

Playlist::Playlist(QSqlDatabase *_database, QObject *parent)
{
    media_playlist = new QMediaPlaylist(parent);
    database = _database;
}

bool Playlist::AddMedia(Album* _album) {

    if( !database->open()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While opening database :" << database->lastError();
        return false;
    }

    QSqlQuery query;
    QString album_title = _album->getTitle();
    query.prepare("UPDATE '"+album_title+"'"
                  "SET inPlaylist = 1");

    if( !query.exec()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While executing query :" << query.lastError();
        return false;
    }

    database->close();

    for (Song& song : *(_album->getSongs())) {
        if (playlist.contains(&song)) {
            qDebug() << "Playlist::AddMedia(Album*)... Song is already in playlist \ninterrupting...";
            break;
        }
        song.is_in_playlist = true;
        playlist.push_back(&song);
    }

    return true;
}

bool Playlist::AddMedia(Song* _song) {

    if( !database->open()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While opening database :" << database->lastError();
        return false;
    }

    QSqlQuery query;
    QString album_title = _song->getAlbumTitle();
    query.prepare("UPDATE '"+album_title+"'"
                  "SET inPlaylist = 1 WHERE title = '"+_song->getTitle()+"'");

    if( !query.exec()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While executing query :" << query.lastError();
        return false;
    }

    database->close();

    if (playlist.contains(_song)) {
        qDebug() << "Playlist::AddMedia(Song*)'... Song is already in playlist \ninterrupting...";
        return false;
    }
    _song->is_in_playlist = true;
    playlist.push_back(_song);

    return true;
}

bool Playlist::RemoveMedia(Album* _album) {

    bool was_removed = false;

    if( !database->open()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While opening database :" << database->lastError();
        return false;
    }

    QSqlQuery query;
    QString album_title = _album->getTitle();
    query.prepare("UPDATE '"+album_title+"'"
                  "SET inPlaylist = 0");

    if( !query.exec()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While executing query :" << query.lastError();
        return false;
    }

    database->close();

    for (int i = 0; i < playlist.size(); ) {
        Song* song = playlist[i];
        if (song->getAlbumTitle() == album_title) {
            song->is_in_playlist = false;
            playlist.removeAt(i);
            qDebug() << "Song " << song->getTitle() << " erased";
            was_removed = true;
        }
        else i++;
    }

    return was_removed;
}


bool Playlist::RemoveMedia(Song* _song) {

    if( !database->open()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While opening database :" << database->lastError();
        return false;
    }

    QSqlQuery query;
    QString album_title = _song->getAlbumTitle();
    query.prepare("UPDATE '"+album_title+"'"
                  "SET inPlaylist = 0 WHERE title = '"+_song->getTitle()+"'");

    if( !query.exec()) {
        qDebug() << "Playlist::RemoveMedia(Album*) : While executing query :" << query.lastError();
        return false;
    }

    database->close();

    _song->is_in_playlist = false;
    bool was_removed = playlist.removeOne(_song);

    if (was_removed) qDebug() << "Song " << _song->getTitle() << " erased";

    return was_removed;
}


Song* Playlist::CurrentMedia() {

    int current_index = media_playlist->currentIndex();

    return (*this)[current_index];
}


void Playlist::Clear() {

    playlist.clear();

    media_playlist->clear();
}


void Playlist::Update() {

    media_playlist->clear();

    for(Song* song : playlist) {
        media_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(song->getPath())));
    }
}

Song *Playlist::operator[](int i)
{
    return (i >= 0 && i < playlist.size()) ? playlist[i] : nullptr;
}
