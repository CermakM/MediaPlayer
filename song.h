#ifndef SONG_H
#define SONG_H

#include <QString>

class Song
{
public:
    explicit Song(const QString _path);

    Song(const QString _path, const QString _fname, const QString _title = " ", QString _interpret = " ", QString _album = " ");

    void SetFileName(QString _fName ) { fName = _fName; }

    QString GetFileName() const { return fName; }

    QString GetPath() const { return path; }

    QString GetTitle() const { return title; }

    QString GetInterpret() const { return interpret; }

    QString GetAlbum() const { return album; }

    bool HasAlbum() const { return has_album; }

private:

    QString path;
    QString fName;
    QString title;
    QString interpret;
    QString album;

    bool has_album = false;

};

#endif // SONG_H
