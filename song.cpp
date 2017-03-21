#include "song.h"

Song::Song(const QString path, Album* parent)
{
    _parent = parent;
    _path = path;

    qDebug() << "Path of the song about to be add: " << path;

    if(_path.isEmpty()) {
        qDebug() << "Empty song has been created";
        return;
    }

    // read the tags from the mp3 file
    TagLib::FileRef file(_path.toUtf8());
    TagLib::String title_string = file.tag()->title();
    TagLib::String artist_string = file.tag()->artist();
    TagLib::String album_string = file.tag()->album();
    TagLib::uint year_uint = file.tag()->year();


    _title = QString::fromStdWString(title_string.toWString());
    _interpret = QString::fromStdWString(artist_string.toWString());
    _album_title = QString::fromStdWString(album_string.toWString());
    _year = year_uint;

    if (_album_title.isEmpty()) _album_title = "-";
    if (_interpret.isEmpty()) _interpret = "-";
    if (title_string.isEmpty()) {
        // if no title extracted - use the filename
        _title = _path.right(_path.length() - 1 - _path.lastIndexOf("/"));
        _title = _title.left(_title.lastIndexOf("\."));
    }
}

Song::Song(const QString& title, const QString& path, const QString& interpret, const QString& album, const int& year, bool inPlaylist)
{
    _title = title;
    _path = path;
    _interpret = interpret;
    _album_title = album;
    _year = year;
    is_in_playlist = inPlaylist;

    _parent = nullptr;
}

bool operator== (const Song& s1, const Song& s2) {

    if (s1.getAlbumTitle() == s2.getAlbumTitle()\
            && s1.getTitle() == s2.getTitle()\
            && s1.getInterpret() == s2.getInterpret())
        return true;

    return false;
}

