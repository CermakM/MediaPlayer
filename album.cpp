#include "album.h"

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
    QString song_title = " "; // need to get the title from the file

    for ( QFileInfo& song: song_paths) {
        QString song_path = song.absoluteFilePath();
        QString song_fname = song.fileName();
        qDebug() << song_path << " | " << song_fname << endl;
        songs.push_back(Song(song_path, song_fname, song_title, interpret, title));
        numberOfSongs++;
    }
}
