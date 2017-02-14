#ifndef MAINWIN_H
#define MAINWIN_H

#include "dialogaddalbum.h"
#include "dialogeditplaylist.h"
#include "dialogaddsongs.h"
#include "dialogeditlibrary.h"
#include "playlist.h"
#include "library.h"

#include <QDir>
#include <QFile>
#include <QMenu>
#include <QWidgetAction>
#include <QTextStream>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QDebug>
#include <QMessageBox>
#include <QShortcut>


#include <vector>
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

    void UpdatePlaylist();

private slots:
    void on_VolumeSlider_valueChanged(int value);

    void on_ProgressSlider_sliderMoved(int position);

    void on_PositionChange( qint64 position );

    void on_DurationChange( qint64 position );

    void on_PlayMusicButton_clicked();

    void on_StopMusicButton_clicked();

    void on_actionAddNewAlbum_triggered();

    void on_actionEditPlaylist_triggered();

    void on_EndOfSong();

    void on_EditPlaylistOver(bool b);

    void on_actionAddNewSongs_triggered();

    void on_ButtonForward_clicked();

    void on_ButtonBackward_clicked();

    void on_actionEditLibrary_triggered();

private:
    Ui::MainWin *ui;

    QMediaPlayer* _media_player;

    Playlist* _playlist;

    Library _library;
};

#endif // MAINWIN_H
