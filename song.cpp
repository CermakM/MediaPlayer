#include "song.h"

Song::Song() {

    qDebug() << "Empty song has been creaded";

}

Song::Song(const QString _path, Album* _parent)
{
    parent = _parent;
    path = _path;

    qDebug() << "Path of the song about to be add: " << path;

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
    year = year_uint;

    if (album.isEmpty()) album = "-";
    if (interpret.isEmpty()) interpret = "-";
    if (title_string.isEmpty()) {
        // if no title extracted - use the filename
        title = path.right(path.length() - 1 - path.lastIndexOf("/"));
        title = title.left(title.lastIndexOf("\."));
    }
}

Song::Song(const QString& _title, const QString& _path, const QString& _interpret, const QString& _album, const int& _year, bool _inPlaylist)
{
    title = _title;
    path = _path;
    interpret = _interpret;
    album = _album;
    year = _year;
    is_in_playlist = _inPlaylist;

    parent = nullptr;
}

bool operator== (const Song& s1, const Song& s2) {

    if (s1.getAlbumTitle() == s2.getAlbumTitle()\
            && s1.getTitle() == s2.getTitle()\
            && s1.getInterpret() == s2.getInterpret())
        return true;

    return false;
}

