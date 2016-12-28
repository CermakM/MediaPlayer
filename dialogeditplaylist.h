#ifndef DIALOGEDITPLAYLIST_H
#define DIALOGEDITPLAYLIST_H

#include "album.h"
#include "song.h"
#include "common.h"

#include <QDialog>
#include <QTreeWidget>
#include <QMediaPlaylist>
#include <QList>
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

    Album* GetAlbumByTitle(const QString&);

    void SetAlbumVector( std::vector<Album>& _album) {
        album_vector = &_album;
    }

    void SetPlaylistVector( std::vector<MusicType>& _playlist) {
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
    std::vector<MusicType> * playlist;

    QList<QTreeWidgetItem*> new_items;
    QList<QTreeWidgetItem*> items_to_remove;

};

#endif // DIALOGEDITPLAYLIST_H
