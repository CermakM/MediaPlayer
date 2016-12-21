#ifndef MAINWIN_H
#define MAINWIN_H

#include "dialogaddalbum.h"
#include "dialogeditplaylist.h"
#include "album.h"
#include "song.h"

#include <QFile>
#include <QTextStream>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QDebug>

#include <vector>
#include <unordered_map>
#include <fstream>



namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWin(QWidget *parent = 0);
    ~MainWin();

    void LoadAlbums();

    void LoadSongs();

    void LoadSongFromPlaylist();

private slots:
    void on_VolumeSlider_valueChanged(int value);

    void on_ProgressSlider_sliderMoved(int position);

    void on_PositionChange( qint64 position );

    void on_DurationChange( qint64 position );

    void on_PlayMusicButton_clicked();

    void on_StopMusicButton_clicked();

    // Creates a new window
    void on_AddNewAlbum_triggered();

    void on_actionEditPlaylist_triggered();

private:
    Ui::MainWin *ui;
    QMediaPlayer* media_player;

    std::vector<Album> AlbumList;
    std::vector<Song> SongList;
    std::vector<Song> playlist;
};

#endif // MAINWIN_H
