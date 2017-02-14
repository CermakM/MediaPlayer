#include "album.h"

Album::Album() {

    _path = "";
    _title = "-";
    _interpret = "";

    qDebug() << "An empty album has been created.";
}

Album::~Album() {

}

Album::Album(QVector<Song>& songs)
{
    if (songs.isEmpty()) {
        qDebug() << "Empty song vector provided...";
        return;
    }

    QString album_path = songs[0].getPath();

    album_path = album_path.left(album_path.lastIndexOf("/"));

    _path = album_path;
    _title = songs[0].getAlbumTitle();
    _interpret = songs[0].getInterpret();

    _songs = songs;

    SearchForIcons(_path);
}

Album::Album(const QString &path) {

    _path = path;
    // get all .mp3 files in the target dir
    // and add them to the vector

    QDir dir(_path);
    QStringList filters;
    filters << "*.mp3";
    dir.setNameFilters(filters);
    QFileInfoList song_paths = dir.entryInfoList();

    // Get the atributes for the album from the mp3 file
    Song temp_song(song_paths[0].absoluteFilePath());
    _title = temp_song.getAlbumTitle();
    _interpret = temp_song.getInterpret();

    for ( QFileInfo& song: song_paths) {
        QString song_path = song.absoluteFilePath();
        //QString song_fname = song.fileName();
        //songs.push_back(Song(song_path, song_fname, song_title, interpret, title));
        _songs.push_back(Song(song_path));
    }

    SearchForIcons(_path);
}

void Album::setIcon(QIcon &icon)
{

    _current_icon = icon;
}

void Album::setIcon(const QString &path)
{

    _current_icon = QIcon(QPixmap(path));
}

void Album::setIcon(const int& i) {

    if (i >= _icons.size()) return;

    QString path_to_icon = _icons[i].absoluteFilePath();
    QPixmap pixmap(path_to_icon);


    _current_icon = QIcon(pixmap);
}

void Album::SearchForIcons(const QString &path)
{
    QDir dir(path);
    if(!dir.exists()) {
        qDebug() << "in Album::createIcons: Cannot find directory" << path ;
    }

    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.jpeg" << "*.tiff";
    dir.setNameFilters(filters);

    _icons = dir.entryInfoList();
    if (_icons.isEmpty()) return;

    // Set the 1st icon by default as current icon
    _current_icon = QIcon(_icons[0].absoluteFilePath());

}

void Album::setTitle(const QString &title)
{
     _title = title;

     for (Song& song : _songs) {
         song.setAlbumTitle(_title);
     }
}

QIcon *Album::CurrentIcon()
{
    return _current_icon.isNull() ? nullptr : &_current_icon;
}

bool operator ==(const Album &a1, const Album &a2)
{
    return a1.getTitle() == a2.getTitle() && a1.getInterpret() == a2.getInterpret();
}
