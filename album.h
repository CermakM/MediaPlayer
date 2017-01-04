#ifndef ALBUM_H
#define ALBUM_H

#include "song.h"

#include <QDir>
#include <QFile>
#include <QtSql>
#include <QDebug>
#include <QString>
#include <QVector>

class Album
{
public:
    Album();

    Album(QVector<Song>& _songs);

    Album(const QString& _path);

    QVector<Song>* getSongs() { return &songs; }

    QString getPath() const {return path; }

    QString getTitle() const { return title; }

    QString getInterpret() const { return interpret; }

    void setTitle(const QString& _title);

    void setInterpret( const QString& _interpret) { interpret = _interpret; }
    void PushSong(const Song& _song) { songs.push_back(_song); }

    int CountSongs() const {return songs.size(); }

    bool contains(Song& _song) { return songs.contains(_song); }

    friend bool operator == (const Album& a1, const Album& a2);

private:

    QString path;
    QString title;
    QString interpret;

    QVector<Song> songs;

};

#endif // ALBUM_H
