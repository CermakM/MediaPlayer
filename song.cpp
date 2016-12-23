#include "song.h"

Song::Song(const QString _path)
{
    path = _path;

    // read the title etc.. from the mp3 file

}

Song::Song(const QString _path, const QString _fname, const QString _title, QString _interpret, QString _album)
{
    path = _path;
    fName = _fname;
    title = _title;
    interpret = _interpret;
    album = _album;

    if (!album.isEmpty()) has_album = true;

}
