#include "mainwin.h"
#include "ui_mainwin.h"

MainWin::MainWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWin),
    _library(parent)
{
    ui->setupUi(this);
    _media_player = new QMediaPlayer(this);
    _playlist = _library.getPlaylist();

    // Connect Signals and slots
    connect(_media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(_media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);
    connect(_playlist->MediaPlaylist(), SIGNAL(currentIndexChanged(int)), this, SLOT(on_EndOfSong()));

    // Connect shortcuts
    QShortcut* shortcutAddAlbum = new QShortcut(QKeySequence("Ctrl+A"), this);
    QShortcut* shortcutAddSongs = new QShortcut(QKeySequence("Ctrl+S"), this);
    QShortcut* shortcutEditPlaylist = new QShortcut(QKeySequence("Ctrl+P"), this);
    QShortcut* shortcutEditLibrary = new QShortcut(QKeySequence("Ctrl+L"), this);
    QShortcut* shortcutPlayButton = new QShortcut(QKeySequence("P"), this);
    QShortcut* shortcutStopButton = new QShortcut(QKeySequence("S"), this);
    QShortcut* shortcutForwardButton = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut* shortcutBackwardButton = new QShortcut(QKeySequence(Qt::Key_Left), this);

    connect(shortcutAddAlbum, SIGNAL(activated()), this, SLOT(on_actionAddNewAlbum_triggered()));
    connect(shortcutAddSongs, SIGNAL(activated()), this, SLOT(on_actionAddNewSongs_triggered()));
    connect(shortcutEditPlaylist, SIGNAL(activated()), this, SLOT(on_actionEditPlaylist_triggered()));
    connect(shortcutEditLibrary, SIGNAL(activated()), this, SLOT(on_actionEditLibrary_triggered()));
    connect(shortcutPlayButton, SIGNAL(activated()), this, SLOT(on_PlayMusicButton_clicked()));
    connect(shortcutStopButton, SIGNAL(activated()), this, SLOT(on_StopMusicButton_clicked()));

    UpdatePlaylist();

    CreateDropArea();
}

MainWin::~MainWin()
{
    delete ui;
}

void MainWin::UpdatePlaylist() {

    ui->CurrentSongLine->clear();
    ui->CurrentAlbumLine->clear();

    _media_player->stop();

    _playlist->Update();

    qDebug() << "Actual media player status: " << _playlist->Size();

    _playlist->setCurrentIndex(0);

    _media_player->setPlaylist(_playlist->MediaPlaylist());
}


void MainWin::CreateDropArea()
{
    if (!_library.empty()) ui->default_placeholder->setVisible(false);

    QVBoxLayout* drop_layout = new QVBoxLayout();
    ui->dropArea->setLayout(drop_layout);

    /* TEST CODE

    iWidget* sample_widget = new iWidget(ui->dropArea);
    sample_widget->setTitle("sample");
    iWidget* sample_widget1 = new iWidget(ui->dropArea);
    sample_widget1->setTitle("sample1");
    iWidget* sample_widget2 = new iWidget(ui->dropArea);
    sample_widget2->setTitle("sample2");
    drop_layout->addWidget(sample_widget, 0, Qt::AlignTop | Qt::AlignLeft);
    drop_layout->addWidget(sample_widget1, 0, Qt::AlignTop | Qt::AlignLeft);
    drop_layout->addWidget(sample_widget2, 0, Qt::AlignTop | Qt::AlignLeft);

    */

    const int MAX_WIDGET_SIZE = 128;
    int max_widget_count = this->width() / MAX_WIDGET_SIZE * 2;

    QBoxLayout* drop_row = new QBoxLayout(QBoxLayout::LeftToRight);
    drop_row->setSpacing(10);
    drop_layout->addLayout(drop_row);

    for (Album album : *(_library.getAlbums())) {
        if ( album.getTitle() != "-") {
            for (Song song : *(album.getSongs())) {
                CreateWidget(&song, drop_row);
                if (max_widget_count == drop_row->count()) {
                    qDebug() << drop_row->count();
                    CreateNewRow(drop_layout, &drop_row);
                }
            }
        }
        else {
            CreateWidget(&album, drop_row);
            if (max_widget_count == drop_row->count()) {
                CreateNewRow(drop_layout, &drop_row);
            }
        }
    }


    QSpacerItem* right_spacer = new QSpacerItem(20, 0, QSizePolicy::Expanding);
    drop_row->addSpacerItem(right_spacer);
}


void MainWin::CreateWidget(Album* const media, QBoxLayout* drop_row) {
    iWidget* new_widget = new iWidget(new Icon(media), this);
    new_widget->getIcon()->setParent(new_widget);
    _icon_widgets.push_back(new_widget);
    drop_row->addWidget(new_widget, 0, Qt::AlignTop | Qt::AlignLeft);
    drop_row->insertSpacing(-1, 10);
}

void MainWin::CreateWidget(Song* const media, QBoxLayout* const drop_row) {
    iWidget* new_widget = new iWidget(new Icon(media), this);
    new_widget->getIcon()->setParent(new_widget);
    _icon_widgets.push_back(new_widget);
    drop_row->addWidget(new_widget, 0, Qt::AlignTop | Qt::AlignLeft);
    drop_row->insertSpacing(-1, 10);
}

void MainWin::CreateNewRow(QVBoxLayout* drop_layout, QBoxLayout** drop_row) {
    QSpacerItem* right_spacer = new QSpacerItem(20, 0, QSizePolicy::Expanding);
    (*drop_row)->addSpacerItem(right_spacer);
    *drop_row = new QBoxLayout(QBoxLayout::LeftToRight);
    (*drop_row)->setSpacing(10);
    drop_layout->addLayout(*drop_row);
}

void MainWin::on_EndOfSong() {

    if (_playlist->isEmpty()) {
        return;
    }

    Song* song = _playlist->CurrentMedia();

    if (song == nullptr) return;

    ui->CurrentAlbumLine->setText(song->getAlbumTitle());
    ui->CurrentSongLine->setText(song->getTitle());
}


void MainWin::on_EditPlaylistOver(bool b) {

    if (b)
        UpdatePlaylist();

    if (_library.empty()) ui->default_placeholder->setVisible(true);
}

void MainWin::on_VolumeSlider_valueChanged(int value)
{

    if (!ui->VolumeSlider->value()) {
        qInfo("The volume is now off");
        _media_player->pause();
        return;
    }

    _media_player->setVolume(value);

}


void MainWin::on_ProgressSlider_sliderMoved(int position)
{
    _media_player->setPosition(position);
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
    _media_player->state() == QMediaPlayer::PlayingState ?
    _media_player->pause() : _media_player->play();
}

void MainWin::on_StopMusicButton_clicked()
{
    _media_player->stop();
}


void MainWin::on_actionAddNewAlbum_triggered()
{
    DialogAddAlbum AlbumBrowser(&_library, this);
    connect(&AlbumBrowser, SIGNAL(Change(bool)), this, SLOT(on_EditPlaylistOver(bool)));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void MainWin::on_actionAddNewSongs_triggered()
{
    DialogAddSongs SongsBrowser(&_library, this);
    connect(&SongsBrowser, SIGNAL(Change(bool)), this, SLOT(on_EditPlaylistOver(bool)));
    SongsBrowser.setWindowTitle("Add your Songs");
    SongsBrowser.setModal(true);
    SongsBrowser.exec();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist(&_library, this);

    connect(&EditPlaylist, &DialogEditPlaylist::Change, this, &MainWin::on_EditPlaylistOver);

    EditPlaylist.setWindowTitle("Edit playlist");
    EditPlaylist.setModal(true);

    EditPlaylist.exec();
}

void MainWin::on_actionEditLibrary_triggered()
{
    DialogEditLibrary EditLibrary(&_library, this);

    connect(&EditLibrary, SIGNAL(UpdatePlaylist(bool)), this, SLOT(on_EditPlaylistOver(bool)));

    EditLibrary.setModal(true);
    EditLibrary.exec();
}

void MainWin::on_ButtonForward_clicked()
{
    _media_player->playlist()->next();
}

void MainWin::on_ButtonBackward_clicked()
{
    _media_player->playlist()->previous();
}


