#ifndef DIALOGEDITPLAYLIST_H
#define DIALOGEDITPLAYLIST_H

#include "album.h"
#include "song.h"
#include "playlist.h"

#include <QDialog>
#include <QTreeWidget>
#include <QMediaPlaylist>
#include <QVector>
#include <QFile>

#include <vector>


namespace Ui {
class DialogEditPlaylist;
}

class DialogEditPlaylist : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditPlaylist(QWidget *parent = 0);
    ~DialogEditPlaylist();

    DialogEditPlaylist(std::vector<Album>&, std::vector<Song>&, Playlist&, QWidget* parent = 0);

    Album* GetAlbumByTitle(const QString&);

    Song* GetSongByTitle(const QString&);

    void PassAlbum( std::vector<Album>& _album) {
        album_vector = &_album;
    }

    void PassSong( std::vector<Song>& _song) {
        song_vector = &_song;
    }

    void PassPlaylist( Playlist& _playlist) {
        playlist = &_playlist;
    }

    void LoadAlbums();


private slots:
    void on_AddToPlaylistButton_clicked();

    void on_RemoveFromPlaylistButton_clicked();

    void on_buttonBox_accepted();

signals:

    void Change(bool);


private:
    Ui::DialogEditPlaylist *ui;

    std::vector<Album> * album_vector;
    std::vector<Song> * song_vector;
    Playlist * playlist;

    QVector<QTreeWidgetItem*> new_albums;
    QVector<QTreeWidgetItem*> albums_to_remove;

};

#endif // DIALOGEDITPLAYLIST_H
