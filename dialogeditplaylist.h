#ifndef DIALOGEDITPLAYLIST_H
#define DIALOGEDITPLAYLIST_H

#include "album.h"
#include "song.h"

#include <QDialog>
#include <QTreeWidget>
#include <QMediaPlaylist>

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

    void SetAlbumVector(const std::vector<Album>& _album) {
        album_vector = _album;
    }

    void SetPlaylistVector( const std::vector<Song>& _playlist) {
        playlist = _playlist;
    }

    void LoadAlbums();

    void LoadPlaylist();

private slots:
    void on_AddToPlaylistButton_clicked();

    void on_RemoveFromPlaylistButton_clicked();

private:
    Ui::DialogEditPlaylist *ui;

    std::vector<Album> album_vector;
    std::vector<Song> song_vector;
    std::vector<Song> playlist;

};

#endif // DIALOGEDITPLAYLIST_H
