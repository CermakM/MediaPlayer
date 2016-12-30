#ifndef SONG_H
#define SONG_H

#include <QString>
#include <QDebug>

#include <taglib/tag.h>
#include <taglib/fileref.h>

class Song
{
public:
    Song();

    explicit Song(const QString _path);

    Song(const QString _path, const QString _fname, const QString _title = " ", QString _interpret = " ", QString _album = " ");

    void SetFileName(QString _fName ) { fName = _fName; }

    QString GetFileName() const { return fName; }

    QString GetPath() const { return path; }

    QString GetTitle() const { return title; }

    QString GetInterpret() const { return interpret; }

    QString GetAlbum() const { return album; }

    QString GetYear() const {return year; }

    bool HasAlbum() const { return has_album; }

    friend bool operator== (const Song& s1, const Song& s2);

private:

    QString path;
    QString fName;
    QString title;
    QString interpret;
    QString album;
    QString year;

    bool has_album = true;

};

#endif // SONG_H
