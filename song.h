#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QDebug>

#include <taglib/tag.h>
#include <taglib/fileref.h>

class Album;

class Song
{
public:
    Song();

    explicit Song(const QString path, Album *parent = nullptr);

    Song(const QString &title, const QString &path, const QString &interpret = "-", const QString &album = "-", const int& year = 0, bool inPlaylist = false);

    QString getPath() const { return _path; }

    QString getTitle() const { return _title; }

    QString getInterpret() const { return _interpret; }

    QString getAlbumTitle() const { return _album; }

    void setAlbumTitle(const QString& album_title) { _album = album_title; }

    int getYear() const {return _year; }

    Album*  getAlbum() { return _parent; }

    void setParent(Album* const parent) { _parent = parent; }

    bool is_in_playlist = false;

    friend bool operator== (const Song& s1, const Song& s2);

private:

    QString _path;

    QString _title;
    QString _interpret;
    QString _album;
    int _year;

    Album* _parent;

};

Q_DECLARE_METATYPE(Song)

#endif // SONG_H
