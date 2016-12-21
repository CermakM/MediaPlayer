#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent) :  
    QMainWindow(parent),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);
    media_player = new QMediaPlayer(this);

    connect(media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);

    LoadAlbums();

    LoadSongs();

}

MainWin::~MainWin()
{
    delete ui;
}


void MainWin::LoadSongs() {

    QFile infile ("songs.txt");
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << infile.errorString();
        return;
    }

    QString song_path, song_title = " ", song_fname, song_interpret = " ", song_album = " ";

    QStringList item;
    QTextStream inputstream(&infile);
    while(!inputstream.atEnd()) {
            item = inputstream.readLine().split("|");
            song_path = item[0];
            song_title = item[1];
            song_fname = item[2];
            song_interpret = item[3];
            song_album = item[4];
            Song current_song(song_path, song_title, song_fname, song_interpret, song_album);
            SongList.push_back(current_song);
            if (item.last() == "#") playlist.push_back(current_song.GetPath());
    }
    infile.close();
}

void MainWin::LoadAlbums() {

    QFile infile("albums.txt");
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << infile.errorString();
        return;
    }

    QString album_path, album_title = " ", album_interpret = " ", album_year = " ";
    QStringList item;
    QTextStream instream(&infile);
    while(!instream.atEnd()) {
            item = instream.readLine().split('|');
            album_path = item[0];
            album_title = item[1];
            album_interpret = item[2];
            album_year = item[3];
            Album current_album(album_path, album_title, album_interpret, album_year);
            AlbumList.push_back(current_album);
            // Check the last char for playlist-sign
            if (item.last() == "#") {
                for ( Song& song : current_album.GetSongs())
                playlist.push_back(song.GetPath());
            }
    }

    infile.close();
}

void MainWin::LoadSongFromPlaylist() {

    // Load the song from playlist
    Song current_song(playlist.front());
    media_player->setMedia(QUrl::fromLocalFile(current_song.GetPath()));
    playlist.push_back(playlist.front());
    playlist.erase(playlist.begin(), playlist.begin()+1);

    qDebug() << media_player->errorString();
}


void MainWin::on_VolumeSlider_valueChanged(int value)
{

    if (!ui->VolumeSlider->value()) {
        qInfo("The volume is now off");
        media_player->pause();
        return;
    }

    media_player->setVolume(value);

}


void MainWin::on_ProgressSlider_sliderMoved(int position)
{
    media_player->setPosition(position);
}


void MainWin::on_PositionChange(qint64 position)
{
    ui->ProgressSlider->setValue(position);
}


void MainWin::on_DurationChange(qint64 position)
{
    ui->ProgressSlider->setMaximum(position);
}


void MainWin::on_PlayMusicButton_clicked()
{
    media_player->play();

}

void MainWin::on_StopMusicButton_clicked()
{
    media_player->stop();
}


void MainWin::on_AddNewAlbum_triggered()
{
    DialogAddAlbum AlbumBrowser;
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist;
    EditPlaylist.setWindowTitle("Edit playlist");
    EditPlaylist.setModal(true);
    EditPlaylist.exec();
}
