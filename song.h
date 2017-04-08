#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QDebug>
#include <QFile>

#include <taglib/tag.h>
#include <taglib/fileref.h>

class Album;
class iWidget;

class Song
{
public:
    Song() {}

    explicit Song(const QString path, Album *parent = nullptr);

    Song(const QString &title, const QString &path, const QString &interpret = "-", const QString &album = "-", const int& year = 0, bool inPlaylist = false);

    void setAlbumTitle(const QString& album_title) { _album_title = album_title; }

    void setParent(Album* const parent) { _parent = parent; }

    void setInterpret( const QString& interpret) { _interpret = interpret; }

    void setWidget(iWidget* const widget) { _widget = widget; }

    iWidget* getWidget() const { return _widget; }

    QString getPath() const { return _path; }

    QString getTitle() const { return _title; }

    QString getInterpret() const { return _interpret; }

    int getYear() const {return _year; }

    QString getAlbumTitle() const { return _album_title; }

    Album*  getAlbum() { return _parent; }

    bool* isPlaying() { return &_is_playing; }

    bool isPlaying(bool state) { return _is_playing = state; }

    bool* isInPlaylist() { return &_is_in_playlist; }

    bool isInPlaylist(bool state) { return _is_in_playlist = state; }

    friend bool operator== (const Song& s1, const Song& s2);

private:

    Album*  _parent;
    iWidget* _widget = nullptr;

    QString _path;
    QString _title;
    QString _interpret;
    QString _album_title;
    int _year;

    bool _is_playing = false;
    bool _is_in_playlist = false;
};

Q_DECLARE_METATYPE(Song)

#endif // SONG_H
