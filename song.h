#ifndef SONG_H
#define SONG_H

#include "media.h"

#include <QString>
#include <QDebug>

#include <taglib/tag.h>
#include <taglib/fileref.h>

class Album;

class Song : public Media
{
public:
    Song();

    explicit Song(const QString path, Album *parent = nullptr);

    Song(const QString &title, const QString &path, const QString &interpret = "-", const QString &album = "-", const int& year = 0, bool inPlaylist = false);

    QString getAlbumTitle() const { return _album_title; }

    void setAlbumTitle(const QString& album_title) { _album_title = album_title; }

    void setParent(Album* const parent) { _parent = parent; }

    int getYear() const {return _year; }

    Album*  getAlbum() { return _parent; }

    friend bool operator== (const Song& s1, const Song& s2);

    bool is_in_playlist = false;

private:

    Album* _parent;
    QString _album_title;
    int _year;
};

Q_DECLARE_METATYPE(Song)

#endif // SONG_H
