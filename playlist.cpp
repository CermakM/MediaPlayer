#include "playlist.h"

Playlist::Playlist(QObject *parent)
{
    media_playlist = new QMediaPlaylist(parent);
}

void Playlist::AddMedia(Album* _album) {

        for (Song& song : *(_album->getSongs())) {
            if (playlist.contains(&song)) {
                qDebug() << "Song is already in playlist";
                break;
            }
            song.is_in_playlist = true;
            playlist.push_back(&song);
        }
}

void Playlist::AddMedia(Song* _song) {

    if (playlist.contains(_song)) {
        qDebug() << "Song is already in playlist";
    }
    _song->is_in_playlist = true;
    playlist.push_back(_song);
}

bool Playlist::RemoveMedia(Album* _album) {

    bool was_removed = false;

    QString _album_title = _album->getTitle();
    for (int i = 0; i < playlist.size(); ) {
        Song* song = playlist[i];
        if (song->getAlbumTitle() == _album_title) {
            song->is_in_playlist = false;
            playlist.removeAt(i);
            qDebug() << "Song " << song->getTitle() << " erased";
        }
        else i++;
    }
    was_removed = true;
}


bool Playlist::RemoveMedia(Song* _song) {

    _song->is_in_playlist = false;
    bool was_removed = playlist.removeOne(_song);

    if (was_removed) qDebug() << "Song " << _song->getTitle() << " erased";

    return was_removed;
}


Song* Playlist::CurrentMedia() {

    int current_index = media_playlist->currentIndex();

    return playlist[current_index];
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
