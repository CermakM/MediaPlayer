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

    Album(const QString _path, QString _title, QString _interpret, QString _year);

    std::vector<Song> GetSongs() const { return songs; }

    QString GetTitle() const { return title; }

    QString GetInterpret() const { return interpret; }

    QString GetYear() const { return year; }
private:
    QString path;
    QString title;
    QString interpret;
    QString year;
    int numberOfSongs;

    std::vector<Song> songs;

};

#endif // ALBUM_H
