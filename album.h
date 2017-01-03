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

    void setTitle(const QString& _title) { title = _title; }

    void PushSong(const Song& _song) { songs.push_back(_song); }

    int number_of_songs = 0;

private:

    QString path;
    QString title;
    QString interpret;

    QVector<Song> songs;

};

#endif // ALBUM_H
