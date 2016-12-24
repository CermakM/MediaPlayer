#include "album.h"

Album::Album() {

    qDebug() << "An empty album has been created.";
}

Album::Album( const QString _path) {

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
    title = temp_song.GetAlbum();
    interpret = temp_song.GetInterpret();
    year = temp_song.GetYear();

    for ( QFileInfo& song: song_paths) {
        QString song_path = song.absoluteFilePath();
        //QString song_fname = song.fileName();
        //songs.push_back(Song(song_path, song_fname, song_title, interpret, title));
        songs.push_back(Song(song_path));
        numberOfSongs++;
    }
}

Album::Album(const QString _path, QString _title, QString _interpret = " ", QString _year = " ")
{
    path = _path;
    title = _title;
    interpret = _interpret;
    year = _year;
    numberOfSongs = 0;

    // get all .mp3 files in the target dir
    // and add them to the vector

    QDir dir(path);
    QStringList filters;
    filters << "*.mp3";
    dir.setNameFilters(filters);
    QFileInfoList song_paths = dir.entryInfoList();
    //QString song_title = " "; // need to get the title from the file

    for ( QFileInfo& song: song_paths) {
        QString song_path = song.absoluteFilePath();
        //QString song_fname = song.fileName();
        //songs.push_back(Song(song_path, song_fname, song_title, interpret, title));
        songs.push_back(Song(song_path));
        numberOfSongs++;
    }
}
