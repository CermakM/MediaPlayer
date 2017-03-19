#ifndef ALBUM_H
#define ALBUM_H

#include "song.h"
#include "media.h"

#include <QDir>
#include <QFile>
#include <QtSql>
#include <QDebug>
#include <QString>
#include <QVector>
#include <QIcon>

class Album : public Media
{
public:
    Album();
    ~Album();

    Album(QVector<Song>& songs);

    Album(const QString& path);

    QVector<Song>* getSongs() { return &_songs; }

    QFileInfoList* getIcons() {return &_icons; }

    void setIcon(QIcon &icon);

    void setIcon(const QString& path);

    void setIcon(const int& i);

    void SearchForIcons(const QString& path);

    void setTitle(const QString& title);

    void PushSong(const Song& song) { _songs.push_back(song); }

    int CountSongs() const {return _songs.size(); }

    bool contains(Song& song) { return _songs.contains(song); }

    QIcon* CurrentIcon();

    friend bool operator == (const Album& a1, const Album& a2);

private:

    QVector<Song> _songs;
    QFileInfoList _icons;
    QIcon _current_icon;
};

Q_DECLARE_METATYPE(Album)

#endif // ALBUM_H
