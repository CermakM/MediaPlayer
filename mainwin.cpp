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


void MainWin::ProcessAlbumLine(QStringList& line) {

    QString album_path, album_title = " ", album_interpret = " ", album_year = " ";

    album_path = line[0];
    if(album_path.isEmpty()) return;
    album_title = line[1];
    album_interpret = line[2];
    album_year = line[3];
    Album current_album(album_path, album_title, album_interpret, album_year);
    AlbumList.push_back(current_album);
    // Check the last char for playlist-sign
    if (line.last() == "#") {
        for ( Song& song : current_album.GetSongs())
            playlist.push_back(song.GetPath());
    }
}

void MainWin::ProcessSongLine(QStringList& line) {

    QString song_path, song_title = " ", song_fname, song_interpret = " ", song_album = " ";

    song_path = line[0];
    if (song_path.isEmpty()) return;
    song_title = line[1];
    song_fname = line[2];
    song_interpret = line[3];
    song_album = line[4];
    Song current_song(song_path, song_title, song_fname, song_interpret, song_album);
    SongList.push_back(current_song);
    if (line.last() == "#") playlist.push_back(current_song.GetPath());
}


void MainWin::LoadSongs() {

    QFile infile ("songs.txt");
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening songs: " << infile.errorString();
        return;
    }

    QTextStream instream(&infile);
    QStringList line;

    while (!instream.atEnd()) {
        line = instream.readLine().split("|");
        ProcessSongLine(line);
    }

    infile.close();
}

void MainWin::LoadAlbums() {

    QFile infile("albums.txt");
    if (!infile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Error opening albums: " <<  infile.errorString();
        return;
    }

    QTextStream instream(&infile);
    QStringList line;
    while (!instream.atEnd()) {
        line = instream.readLine().split("|");
        ProcessAlbumLine(line);
    }
    infile.close();
}

void MainWin::LoadSongFromPlaylist() {

    // Check if the playlist is empty, if so, ask to add a song
    if(playlist.empty()) {
        QMessageBox::warning(this, tr("Empty Playlist"), tr("Add a song to your playlist first"), QMessageBox::Ok);
        return;
    }

    // Load a song from playlist
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
    LoadSongFromPlaylist();
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

    qDebug() << "Is this part of code even executed?" << endl;
    QFile infile("albums.txt");
    infile.open(QIODevice::ReadOnly);
    infile.seek(infile.size() -1);
    QTextStream instream (&infile);
    QStringList last_line = instream.readLine().split('|');

    ProcessAlbumLine(last_line);

    infile.close();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist;
    EditPlaylist.setWindowTitle("Edit playlist");
    EditPlaylist.setModal(true);
    EditPlaylist.exec();
}
