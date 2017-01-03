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

    explicit Song(const QString _path, Album *_parent = nullptr);

    Song(const QString &_title, const QString &_path, const QString &_interpret = "-", const QString &_album = "-", const int& year = 0, bool inPlaylist = false);

    QString getPath() const { return path; }

    QString getTitle() const { return title; }

    QString getInterpret() const { return interpret; }

    QString getAlbumTitle() const { return album; }

    int getYear() const {return year; }

    Album*  getAlbum() { return parent; }

    void setParent(Album* const _parent) { parent = _parent; }

    bool is_in_playlist = false;

    friend bool operator== (const Song& s1, const Song& s2);

private:

    QString path;

    QString title;
    QString interpret;
    QString album;
    int year;

    Album* parent;

};

#endif // SONG_H
