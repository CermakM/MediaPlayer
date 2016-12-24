#ifndef ALBUM_H
#define ALBUM_H

#include "song.h"

#include <QDir>
#include <QDebug>
#include <QString>

#include <vector>

class Album
{
public:
    Album();

    Album(const QString _path);

    Album(const QString _path, QString _title, QString _interpret, QString _year);

    std::vector<Song> GetSongs() const { return songs; }

    QString GetTitle() const { return title; }

    QString GetInterpret() const { return interpret; }

    QString GetYear() const { return year; }

    int numberOfSongs;

    bool is_in_playlist = false;

private:
    QString path;
    QString title;
    QString interpret;
    QString year;

    std::vector<Song> songs;

};

#endif // ALBUM_H
