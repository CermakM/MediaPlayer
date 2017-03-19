#ifndef MEDIA_H
#define MEDIA_H

#include <QString>

class Media
{
public:

    void setInterpret( const QString& interpret) { _interpret = interpret; }

    QString getPath() const { return _path; }

    QString getTitle() const { return _title; }

    QString getInterpret() const { return _interpret; }

protected:
    QString _path;
    QString _title;
    QString _interpret;
};

#endif // MEDIA_H
