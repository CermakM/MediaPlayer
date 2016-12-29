#include "playlist.h"

Playlist::Playlist(QObject *parent)
{
    media_playlist = new QMediaPlaylist(parent);
}

bool Playlist::isSong(Media _media) {

    return _media.type() == typeid(Song);
}


bool Playlist::isAlbum(Media _media) {

    return _media.type() == typeid(Album);
}


void Playlist::AddMedia(Media _media) {

    if (isAlbum(_media)) {
        Album& album = boost::get<Album>(_media);
        for (Song& song : album.GetSongs()) {
            if (playlist.contains(song)) {
                qDebug() << "Song is already in playlist";
                break;
            }
            playlist.push_back(song);
        }
    }

    else if(isSong(_media)) {
        Song& song = boost::get<Song>(_media);
        if (playlist.contains(song)) {
            qDebug() << "Song is already in playlist";
        }
        playlist.push_back(song);
    }

    else qDebug() << "Type mismatch";

    Update();
}


bool Playlist::RemoveMedia(Media _media) {

    bool was_removed = false;

    if(isAlbum(_media)) {
        Album& album = boost::get<Album>(_media);
        QString album_title = album.GetTitle();
        QVector<int> indexes;
        for (Song& song : playlist) {
            if (song.GetAlbum() == album_title)
               indexes.push_back(playlist.indexOf(song));
        }

        for (int i : indexes) {
            playlist.removeAt(i);
        }
    }

    else if (isSong(_media)) {
        was_removed = playlist.removeOne(boost::get<Song>(_media));
    }

    else qDebug() << "Type mismatch";

    Update();

    return was_removed;
}


Song* Playlist::CurrentMedia() {

    int current_index = media_playlist->currentIndex();

    return &playlist[current_index];
}


void Playlist::Clear() {

    playlist.clear();

    Update();
}


void Playlist::Update() {

    for(Song& song : playlist) {
        media_playlist->addMedia(QMediaContent(QUrl::fromLocalFile(song.GetPath())));
    }
}
