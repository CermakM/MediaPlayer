#include "album.h"

Album::Album() {

    path = "";
    title = "Untitled";
    interpret = "";

    qDebug() << "An empty album has been created.";
}

Album::Album(QVector<Song>& _songs)
{
    if (_songs.isEmpty()) {
        qDebug() << "Empty song vector provided...";
        return;
    }

    QString album_path = _songs[0].getPath();

    album_path = album_path.left(album_path.lastIndexOf("/"));

    path = album_path;
    title = _songs[0].getAlbumTitle();
    interpret = _songs[0].getInterpret();

    songs = _songs;
    number_of_songs = _songs.size();
}

Album::Album(const QString &_path) {

    path = _path;
    // get all .mp3 files in the target dir
    // and add them to the vector

    QDir dir(path);
    QStringList filters;
    filters << "*.mp3";
    dir.setNameFilters(filters);
    QFileInfoList song_paths = dir.entryInfoList();

    // Get the atributes for the album from the mp3 file
    Song temp_song(song_paths[0].absoluteFilePath());
    title = temp_song.getAlbumTitle();
    interpret = temp_song.getInterpret();

    for ( QFileInfo& song: song_paths) {
        QString song_path = song.absoluteFilePath();
        //QString song_fname = song.fileName();
        //songs.push_back(Song(song_path, song_fname, song_title, interpret, title));
        songs.push_back(Song(song_path));
        number_of_songs++;
    }
}
