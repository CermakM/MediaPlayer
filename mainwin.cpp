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

    CreateDropArea();

    ConnectSignals();

    UpdatePlaylist();
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
    FlowLayout* flow_layout = new FlowLayout(ui->dropArea);
    ui->dropAreaContent->setLayout(flow_layout);

    if (!_library.empty()) ui->default_placeholder->setVisible(false);

    _icon_signal_mapper = new QSignalMapper(this);

    for (Album* const album : *(_library.getAlbums())) {
        if ( album->getTitle() == "-") {
            for (Song& song : *(album->getSongs())) {
                CreateWidget(&song, T_SONG);
            }
        }
        else {
            CreateWidget(album, T_ALBUM);
        }
    }

    ui->dropArea->setWidget(ui->dropAreaContent);
}

void MainWin::CreateWidget(void* const media,  Type type) {

    iWidget* new_widget = nullptr;
    if ( type == T_ALBUM )
        new_widget = new iWidget(new Icon(static_cast<Album*>(media)), ui->dropArea);
    else if ( type == T_SONG)
        new_widget = new iWidget(new Icon(static_cast<Song*>(media)), ui->dropArea);
    else {
        qDebug() << "Icon type T_NOTYPE specified. Default folder will be created";
        new_widget = new iWidget(new Icon(), this);
    }

    new_widget->getIcon()->setParent(new_widget);
    _icon_widgets.push_back(new_widget);

    ui->dropAreaContent->layout()->addWidget(new_widget);

    _icon_signal_mapper->setMapping(new_widget, new_widget);
    connect(new_widget, SIGNAL(clicked()), _icon_signal_mapper, SLOT(map()));
    connect(new_widget, SIGNAL(double_clicked()), _icon_signal_mapper, SLOT(map()));
}

//void MainWin::CreateWidget(Album* const media, QBoxLayout* drop_row) {
//    iWidget* new_widget = new iWidget(new Icon(media), this);
//    new_widget->getIcon()->setParent(new_widget);
//    _icon_widgets.push_back(new_widget);
//    drop_row->addWidget(new_widget, 0, Qt::AlignTop | Qt::AlignLeft);
//    drop_row->insertSpacing(-1, 10);

//    _icon_signal_mapper->setMapping(new_widget, new_widget);
//    connect(new_widget, SIGNAL(clicked()), _icon_signal_mapper, SLOT(map()));
//    connect(new_widget, SIGNAL(double_clicked()), _icon_signal_mapper, SLOT(map()));
//}

//void MainWin::CreateWidget(Song* const media, QBoxLayout* const drop_row) {
//    iWidget* new_widget = new iWidget(new Icon(media), this);
//    new_widget->getIcon()->setParent(new_widget);
//    _icon_widgets.push_back(new_widget);
//    drop_row->addWidget(new_widget, 0, Qt::AlignTop | Qt::AlignLeft);
//    drop_row->insertSpacing(-1, 10);

//    _icon_signal_mapper->setMapping(new_widget, new_widget);
//    connect(new_widget, SIGNAL(clicked()), _icon_signal_mapper, SLOT(map()));
//    connect(new_widget, SIGNAL(double_clicked()), _icon_signal_mapper, SLOT(map()));
//}

void MainWin::ConnectSignals()
{

    // Connect Signals and slots
    connect(_media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(_media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);
    connect(_playlist->MediaPlaylist(), SIGNAL(currentIndexChanged(int)), this, SLOT(on_EndOfSong()));


    // Shortcuts
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
    connect(shortcutPlayButton, SIGNAL(activated()), this, SLOT(on_ButtonPlay_clicked()));
    connect(shortcutStopButton, SIGNAL(activated()), this, SLOT(on_ButtonStop_clicked()));

    connect(_icon_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));
    connect(_icon_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));
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


void MainWin::on_ButtonPlay_clicked()
{
    _media_player->state() == QMediaPlayer::PlayingState ?
    _media_player->pause() : _media_player->play();
}

void MainWin::on_ButtonStop_clicked()
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
    if (_media_player->playlist()) _media_player->playlist()->next();
}

void MainWin::on_ButtonBackward_clicked()
{
    if (_media_player->playlist()) _media_player->playlist()->previous();
}

void MainWin::on_Icon_click(QWidget *target)
{
    iWidget* d_target = dynamic_cast<iWidget*>(target);

    d_target->getIcon()->setStyleSheet("background: #cccccc");

    QLineEdit* icon_title_editor = d_target->getTitleEditor();

    icon_title_editor->setFocus();
    icon_title_editor->raise();
    icon_title_editor->setFont(QFont("Tahoma", 11, QFont::Bold));

    _selected_icons.push_back(d_target);
}

void MainWin::on_Icon_doubleClick(QWidget *target)
{  
        iWidget* icon = dynamic_cast<iWidget*>(target);

        if (icon->getType() == Type::T_SONG) {
            _media_player->stop();
            _media_player->setMedia(QMediaContent(QUrl::fromLocalFile(icon->getPath())));
            Song* song_to_play = _library.getSongByTitle(icon->getTitle());
            ui->CurrentAlbumLine->setText(song_to_play->getAlbumTitle());
            ui->CurrentSongLine->setText(song_to_play->getTitle());
            _media_player->play();
        }
        else if (icon->getType() == Type::T_ALBUM){
            // Open the Album folder
        }
}


void MainWin::on_ButtonDeselect_clicked()
{
    for (iWidget* icon : _selected_icons ) {
        icon->DefaultAdjustement();
    }

    _selected_icons.clear();
    QVector<iWidget*>().swap(_selected_icons);
}

void MainWin::on_ButtonRemove_clicked()
{
    while( !_selected_icons.empty()) {
        iWidget* icon = _selected_icons.front();

        ui->dropArea->layout()->removeWidget(icon);
        if (icon->getType() == Type::T_ALBUM) {
            _library.RemoveMedia(_library.getAlbumByTitle(icon->getTitle()));
        }
        else {
            _library.RemoveMedia(_library.getSongByTitle(icon->getTitle()));
        }
        _selected_icons.removeFirst();
        delete icon;
    }

    QVector<iWidget*>().swap(_selected_icons);
}
