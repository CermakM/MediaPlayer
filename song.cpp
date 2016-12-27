#include "song.h"

Song::Song() {
    qDebug() << "Empty song has been creaded";
}

Song::Song(const QString _path)
{
    path = _path;

    if(path.isEmpty()) {
        qDebug() << "Empty song has been created";
        return;
    }

    // read the tags from the mp3 file
    TagLib::FileRef file(path.toUtf8());
    TagLib::String title_string = file.tag()->title();
    TagLib::String artist_string = file.tag()->artist();
    TagLib::String album_string = file.tag()->album();
    TagLib::uint year_uint = file.tag()->year();

    title = QString::fromStdWString(title_string.toWString());
    interpret = QString::fromStdWString(artist_string.toWString());
    album = QString::fromStdWString(album_string.toWString());
    year = QString::number(year_uint);

}

Song::Song(const QString _path, const QString _fname, const QString _title, QString _interpret, QString _album)
{
    path = _path;
    fName = _fname;
    title = _title;
    interpret = _interpret;
    album = _album;

    if (album.isEmpty()) has_album = false;

}
