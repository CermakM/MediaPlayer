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
    connect(&media_playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(on_EndOfSong()));

    LoadAlbums();

    LoadSongs();

    CreateMediaPlaylist();

    on_EndOfSong();
}

MainWin::~MainWin()
{
    delete ui;
}


void MainWin::AddAlbumToAlbums(QStringList& line) {

    QString album_path, album_title = " ", album_interpret = " ", album_year = " ";

    album_path = line[0];
    if(album_path.isEmpty()) return;
    album_title = line[1];
    album_interpret = line[2];
    album_year = line[3];
    Album current_album(album_path, album_title, album_interpret, album_year);

    // Check the last char for playlist-sign
    if (line.last() == "#") {
        current_album.is_in_playlist = true;
        AlbumList.push_back(current_album);
        for ( Song& song : current_album.GetSongs())
            playlist.push_back(song);
    }
    else AlbumList.push_back(current_album);
}

void MainWin::AddSongToSongs(QStringList& line) {

    QString song_path, song_title = " ", song_fname, song_interpret = " ", song_album = " ";

    song_path = line[0];
    if (song_path.isEmpty()) return;
    song_title = line[1];
    song_fname = line[2];
    song_interpret = line[3];
    song_album = line[4];
    Song current_song(song_path, song_title, song_fname, song_interpret, song_album);
    SongList.push_back(current_song);
    if (line.last() == "#") playlist.push_back(current_song);
}


void MainWin::on_EndOfSong() {
    Song current_song(playlist[media_playlist.currentIndex()]);
    ui->CurrentAlbumLine->setText(current_song.GetAlbum());
    ui->CurrentSongLine->setText(current_song.GetTitle());
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
        AddSongToSongs(line);
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
        AddAlbumToAlbums(line);
    }
    infile.close();
}

void MainWin::AddLastAlbumToPlaylist() {

    qInfo("Refreshing playlist..");
    QFile infile("albums.txt");
    infile.open(QIODevice::ReadOnly);
    if (!infile.size()) {
        qDebug() << "Album list is empty";
        return;
    }
    QString last_line, line;
    while (!(line = infile.readLine()).isEmpty()) {
        last_line = line;
    }

    QStringList last_line_split = last_line.split('|');
    AddAlbumToAlbums(last_line_split);

    qDebug() << "Playlist has been refreshed";
    infile.close();
}


void MainWin::CreateMediaPlaylist() {

    for ( Song& _song : playlist) {
        media_playlist.addMedia(QUrl::fromLocalFile(_song.GetPath()));
    }
    media_playlist.setCurrentIndex(1);
    media_player->setPlaylist(&media_playlist);
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

    // qDebug() << media_player->errorString();
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
    try {
    ui->ProgressSlider->setValue(position);
    }
    catch (...) {}
}


void MainWin::on_DurationChange(qint64 position)
{
    ui->ProgressSlider->setMaximum(position);
}


void MainWin::on_PlayMusicButton_clicked()
{
    media_player->state() == QMediaPlayer::PlayingState ?
    media_player->pause() : media_player->play();
}

void MainWin::on_StopMusicButton_clicked()
{
    media_player->stop();
}


void MainWin::on_AddNewAlbum_triggered()
{
    DialogAddAlbum AlbumBrowser;
    connect(&AlbumBrowser, SIGNAL(destroyed()), this, SLOT(AddLastAlbumToPlaylist()));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist;
    EditPlaylist.setWindowTitle("Edit playlist");
    EditPlaylist.setModal(true);
    EditPlaylist.SetAlbumVector(AlbumList);
    EditPlaylist.SetPlaylistVector(playlist);
    EditPlaylist.LoadAlbums();
   // EditPlaylist.LoadPlaylist();
    EditPlaylist.exec();
}
