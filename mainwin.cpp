#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent) :  
    QMainWindow(parent),
    ui(new Ui::MainWin),
    library(parent)
{
    ui->setupUi(this);
    media_player = new QMediaPlayer(this);
    playlist = library.getPlaylist();

    // Connect Signals and slots
    connect(media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);
    connect(playlist->MediaPlaylist(), SIGNAL(currentIndexChanged(int)), this, SLOT(on_EndOfSong()));

    // Connect shortcuts
    QShortcut* shortcutAddAlbum = new QShortcut(QKeySequence("Ctrl+Shift+A"), this);
    QShortcut* shortcutEditPlaylist = new QShortcut(QKeySequence("Ctrl+Shift+E"), this);
    QShortcut* shortcutPlayButton = new QShortcut(QKeySequence("P"), this);
    QShortcut* shortcutForwardButton = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut* shortcutBackwardButton = new QShortcut(QKeySequence(Qt::Key_Left), this);

    connect(shortcutAddAlbum, SIGNAL(activated()), this, SLOT(on_actionAddNewAlbum_triggered()));
    connect(shortcutEditPlaylist, SIGNAL(activated()), this, SLOT(on_actionEditPlaylist_triggered()));
    connect(shortcutPlayButton, SIGNAL(activated()), this, SLOT(on_PlayMusicButton_clicked()));

    UpdatePlaylist();
}

MainWin::~MainWin()
{
    delete ui;
}

void MainWin::on_EndOfSong() {

    if (playlist->isEmpty()) {
        return;
    }

    Song* song = playlist->CurrentMedia();

    if (song == nullptr) return;

    ui->CurrentAlbumLine->setText(song->getAlbumTitle());
    ui->CurrentSongLine->setText(song->getTitle());
}

void MainWin::UpdatePlaylist() {

    ui->CurrentSongLine->clear();
    ui->CurrentAlbumLine->clear();

    media_player->stop();

    playlist->Update();

    qDebug() << "Actual media player status: " << playlist->Size();

    playlist->setCurrentIndex(0);

    media_player->setPlaylist(playlist->MediaPlaylist());
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
    DialogAddAlbum AlbumBrowser(&library, this);
    connect(&AlbumBrowser, SIGNAL(Change(bool)), this, SLOT(on_EditPlaylistOver(bool)));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist(&library, this);

    connect(&EditPlaylist, &DialogEditPlaylist::Change, this, &MainWin::on_EditPlaylistOver);

    EditPlaylist.setWindowTitle("Edit playlist");
    EditPlaylist.setModal(true);

    EditPlaylist.exec();
}

void MainWin::on_EditPlaylistOver(bool b) {

    if (b)
        UpdatePlaylist();
}
