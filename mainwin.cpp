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
        playlist.push_back(current_album);
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

    if (media_playlist.isEmpty()) {
        return;
    }
    QMediaContent current_media = media_playlist.currentMedia();
    QString song_title = current_media.canonicalUrl().path();
    Song _song(song_title);
    ui->CurrentAlbumLine->setText(_song.GetAlbum());
    ui->CurrentSongLine->setText(_song.GetTitle());
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

void MainWin::AddLastAlbum() {

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


    void MainWin::CreateMediaPlaylist() {

    media_player->stop();
    media_player->setMedia(QMediaContent());
    media_playlist.clear();

    if(playlist.empty()) {
        media_player->setPlaylist(&media_playlist);
        ui->CurrentAlbumLine->clear();
        ui->CurrentSongLine->clear();
        return;
    }

    for ( MusicType itm : playlist) {
        if (itm.type() == typeid(Song)) {
            Song _song = boost::get<Song>(itm);
            media_playlist.addMedia(QUrl::fromLocalFile(_song.GetPath()));
        }
        else if(itm.type() == typeid(Album)) {
            Album _album = boost::get<Album>(itm);
            std::vector<Song> _songs = _album.GetSongs();
            for (Song& _song : _songs) {
                media_playlist.addMedia(QUrl::fromLocalFile(_song.GetPath()));
            }
        }
        else {
            qDebug() << "Boost::variant type does not match";
        }
    }

    media_playlist.setCurrentIndex(0);
    media_player->setPlaylist(&media_playlist);
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


void MainWin::on_AddNewAlbum_triggered()
{
    DialogAddAlbum AlbumBrowser;
    connect(&AlbumBrowser, SIGNAL(destroyed()), this, SLOT(AddLastAlbum()));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist;

    connect(&EditPlaylist, &DialogEditPlaylist::PlaylistChanged, this, &MainWin::on_EditPlaylistOver);

    EditPlaylist.setWindowTitle("Edit playlist");
    EditPlaylist.setModal(true);
    EditPlaylist.SetAlbumVector(AlbumList);
    EditPlaylist.SetPlaylistVector(playlist);
    EditPlaylist.LoadAlbums();

    EditPlaylist.exec();
}

void MainWin::on_EditPlaylistOver(bool b) {

    if (b)
        CreateMediaPlaylist();

}


class GetTitleVisitor : public boost::static_visitor<> {

public:
    QString operator() (Song& _song) const { return _song.GetTitle(); }

    QString operator() (Album& _album) const { return _album.GetTitle(); }
};
