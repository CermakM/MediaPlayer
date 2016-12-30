#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent) :  
    QMainWindow(parent),
    ui(new Ui::MainWin)
{
    ui->setupUi(this);
    media_player = new QMediaPlayer(this);

    // Connect Signals and slots
    connect(media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);
    connect(playlist.MediaPlaylist(), SIGNAL(currentIndexChanged(int)), this, SLOT(on_EndOfSong()));

    // Connect shortcuts
    QShortcut* shortcutAddAlbum = new QShortcut(QKeySequence("Ctrl+Shift+A"), this);
    QShortcut* shortcutEditPlaylist = new QShortcut(QKeySequence("Ctrl+Shift+E"), this);
    QShortcut* shortcutPlayButton = new QShortcut(QKeySequence("P"), this);
    QShortcut* shortcutForwardButton = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut* shortcutBackwardButotn = new QShortcut(QKeySequence(Qt::Key_Left), this);

    connect(shortcutAddAlbum, SIGNAL(activated()), this, SLOT(on_actionAddNewAlbum_triggered()));
    connect(shortcutEditPlaylist, SIGNAL(activated()), this, SLOT(on_actionEditPlaylist_triggered()));
    connect(shortcutPlayButton, SIGNAL(activated()), this, SLOT(on_PlayMusicButton_clicked()));

    LoadAlbums();

    LoadSongs();

    UpdatePlaylist();
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
        albumList.push_back(current_album);
        playlist.AddMedia(current_album);
    }
    else albumList.push_back(current_album);
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
    songList.push_back(current_song);
    if (line.last() == "#") playlist.AddMedia(current_song);
}


void MainWin::on_EndOfSong() {

    if (playlist.isEmpty()) {
        return;
    }
    Song* song = playlist.CurrentMedia();

    ui->CurrentAlbumLine->setText(song->GetAlbum());
    ui->CurrentSongLine->setText(song->GetTitle());
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

void MainWin::AddLastAlbum(bool b) {

    if (!b) return;

    qInfo("Refreshing playlist..");
    QFile infile("albums.txt");
    infile.open(QIODevice::ReadOnly);
    if (!infile.size()) {
        qDebug() << "Album list is empty";
        return;
    }
    QString last_line;
    QTextStream instream(&infile);
    while (!(instream.atEnd())) {
        last_line = instream.readLine();
    }

    QStringList last_line_split = last_line.split('|');
    AddAlbumToAlbums(last_line_split);

    qDebug() << "Playlist has been refreshed";
    infile.close();
}


void MainWin::UpdatePlaylist() {

    ui->CurrentSongLine->clear();
    ui->CurrentAlbumLine->clear();

    media_player->stop();

    playlist.Update();

    qDebug() << "Actual media player status: " << playlist.Size();

    playlist.setCurrentIndex(0);

    media_player->setPlaylist(playlist.MediaPlaylist());
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
    media_player->state() == QMediaPlayer::PlayingState ?
    media_player->pause() : media_player->play();
}

void MainWin::on_StopMusicButton_clicked()
{
    media_player->stop();
}


void MainWin::on_actionAddNewAlbum_triggered()
{
    DialogAddAlbum AlbumBrowser;
    connect(&AlbumBrowser, SIGNAL(Change(bool)), this, SLOT(AddLastAlbum(bool)));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist(albumList, songList, playlist, this);

    connect(&EditPlaylist, &DialogEditPlaylist::Change, this, &MainWin::on_EditPlaylistOver);

    EditPlaylist.setWindowTitle("Edit playlist");
    EditPlaylist.setModal(true);

    EditPlaylist.exec();
}

void MainWin::on_EditPlaylistOver(bool b) {

    if (b)
        UpdatePlaylist();
}


class GetTitleVisitor : public boost::static_visitor<> {

public:
    QString operator() (Song& _song) const { return _song.GetTitle(); }

    QString operator() (Album& _album) const { return _album.GetTitle(); }
};
