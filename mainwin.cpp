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
    _icon_signal_mapper = new QSignalMapper(this);
    ui->default_placeholder->setVisible(false);

    if (_library.empty()) {
        ui->default_placeholder->setVisible(true);
        ui->dropAreaContent->setLayout(new QGridLayout(ui->dropArea));
        ui->dropAreaContent->layout()->addWidget(ui->default_placeholder);
        ui->dropArea->setWidget(ui->dropAreaContent);
        return;
    }

    FlowLayout* flow_layout = new FlowLayout(ui->dropArea);
    ui->dropAreaContent->setLayout(flow_layout);


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


void MainWin::CreateWidget(void* const media, Type type) {

    iWidget* new_widget = nullptr;
    if ( type == T_ALBUM )
        new_widget = new iWidget(new Icon(static_cast<Album*>(media)), &_library, ui->dropArea);
    else if ( type == T_SONG)
        new_widget = new iWidget(new Icon(static_cast<Song*>(media)), &_library, ui->dropArea);
    else {
        qDebug() << "Icon type T_NOTYPE specified. Default folder will be created";
        new_widget = new iWidget(this);
    }

    new_widget->getIcon()->setParent(new_widget);
    _icon_widgets.push_back(new_widget);

    ui->dropAreaContent->layout()->addWidget(new_widget);

    _icon_signal_mapper->setMapping(new_widget, new_widget);
    connect(new_widget, SIGNAL(clicked()), _icon_signal_mapper, SLOT(map()));
    connect(new_widget, SIGNAL(double_clicked(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));
}

void MainWin::ConnectSignals()
{

    // Connect Signals and slots
    connect(_media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(_media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);
//    connect(_playlist->MediaPlaylist(), SIGNAL(currentIndexChanged(int)), this, SLOT(on_Media_change()));
    connect(_media_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(on_Media_change(QMediaPlayer::MediaStatus)));

    // Shortcuts
    QShortcut* shortcutAddAlbum = new QShortcut(QKeySequence("Ctrl+A"), this);
    QShortcut* shortcutAddSongs = new QShortcut(QKeySequence("Ctrl+S"), this);
    QShortcut* shortcutEditPlaylist = new QShortcut(QKeySequence("Ctrl+P"), this);
    QShortcut* shortcutEditLibrary = new QShortcut(QKeySequence("Ctrl+L"), this);
    QShortcut* shortcutPlayButton = new QShortcut(QKeySequence("P"), this);
    QShortcut* shortcutStopButton = new QShortcut(QKeySequence("S"), this);
    QShortcut* shortcutForwardButton = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut* shortcutBackwardButton = new QShortcut(QKeySequence(Qt::Key_Left), this);
    QShortcut* shortcutDeselect = new QShortcut(QKeySequence(Qt::Key_Escape), this);

    connect(shortcutAddAlbum, SIGNAL(activated()), this, SLOT(on_actionAddNewAlbum_triggered()));
    connect(shortcutAddSongs, SIGNAL(activated()), this, SLOT(on_actionAddNewSongs_triggered()));
    connect(shortcutEditPlaylist, SIGNAL(activated()), this, SLOT(on_actionEditPlaylist_triggered()));
    connect(shortcutEditLibrary, SIGNAL(activated()), this, SLOT(on_actionEditLibrary_triggered()));
    connect(shortcutPlayButton, SIGNAL(activated()), this, SLOT(on_ButtonPlay_clicked()));
    connect(shortcutStopButton, SIGNAL(activated()), this, SLOT(on_ButtonStop_clicked()));
    connect(shortcutDeselect, SIGNAL(activated()), this, SLOT(on_Icon_deselect()));

    connect(_icon_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));
}

void MainWin::on_Media_change(QMediaPlayer::MediaStatus state) {

    Song* song = nullptr;
    if (_current_song != nullptr)
        song = _current_song;
    else song = _playlist->CurrentMedia();

    if (song == nullptr) return;

    song->isPlaying(false);

    iWidget* current_widget = nullptr;
    QVector<iWidget*> current_widget_vector;
    if (_temporary_window_entered)
        current_widget_vector = _temporary_icon_widgets;
    else
        current_widget_vector = _icon_widgets;

    for (iWidget* const widget : current_widget_vector) {
        if (widget->getTitle() == song->getTitle()) {
            current_widget = widget;
            break;
        }
    }

    if(current_widget == nullptr) return;

    if (state == QMediaPlayer::EndOfMedia || state == QMediaPlayer::LoadingMedia) {
        current_widget->isPlaying(false);
    }
    else if (state == QMediaPlayer::BufferedMedia ) {
        current_widget->isPlaying(true);
        _current_song = song;
    }

    ui->CurrentAlbumLine->setText(song->getAlbumTitle());
    ui->CurrentSongLine->setText(song->getTitle());
}


void MainWin::on_EditPlaylistOver(Album* album, Library::ChangeState change) {

    (void*) album;
    if (change)
        UpdatePlaylist();

    if (_library.empty()) ui->default_placeholder->setVisible(true);
}

void MainWin::on_EditPlaylistOver(bool change)
{
    if (change)
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
    connect(&AlbumBrowser, SIGNAL(change(Album*,Library::ChangeState)), this, SLOT(on_EditPlaylistOver(Album*,Library::ChangeState)));
    connect(&AlbumBrowser, SIGNAL(change(Album*,Library::ChangeState)), this, SLOT(on_Library_change(Album*,Library::ChangeState)));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void MainWin::on_actionAddNewSongs_triggered()
{
    DialogAddSongs SongsBrowser(&_library, this);
    connect(&SongsBrowser, SIGNAL(change(Album*,Library::ChangeState)), this, SLOT(on_EditPlaylistOver(Album*, Library::ChangeState)));
    connect(&SongsBrowser, SIGNAL(change(Album*,Library::ChangeState)), this, SLOT(on_Library_change(Album*,Library::ChangeState)));
    SongsBrowser.setWindowTitle("Add your Songs");
    SongsBrowser.setModal(true);
    SongsBrowser.exec();
}

void MainWin::on_actionEditPlaylist_triggered()
{
    DialogEditPlaylist EditPlaylist(&_library, this);

    connect(&EditPlaylist, SIGNAL(change(bool)), this, SLOT(on_EditPlaylistOver(bool)));

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

void MainWin::on_Library_change(Album* album, Library::ChangeState state)
{
    if (!_library.empty()) ui->default_placeholder->hide();

    iWidget* widget_to_change = nullptr;
    int widget_index = 0;
    if (state == Library::CHANGE) {
        if (album->getTitle() == '-') {
            int widget_count = album->CountSongs();
            QVector<Song>* album_songs = album->getSongs();
            while (widget_count) {
                widget_to_change = _icon_widgets.at(widget_index);
                if (widget_to_change->getAlbumTitle() == '-') {
                    ui->dropAreaContent->layout()->removeWidget(widget_to_change);
                    _icon_widgets.removeAt(widget_index);
                    delete widget_to_change;
                    widget_to_change = nullptr;
                }
                widget_index++; widget_count--;
            }
            for (Song& song : *album_songs){
                CreateWidget(&song, T_SONG);
            }
            return;
        }
        else {
            for (iWidget* const widget : _icon_widgets) {
                if ( widget->getTitle() == album->getTitle()) {
                    widget_to_change = widget;
                }
                widget_index++;
                }
        }
        if (!widget_to_change) {
            qDebug() << "in MainWin::on_Library_change: No remaining widgets to change " + album->getTitle();
            return;
        }

        _icon_widgets.removeAt(widget_index);

        iWidget* new_widget = new iWidget(new Icon(static_cast<Album*>(album)), &_library, ui->dropArea);
        new_widget->getIcon()->setParent(new_widget);
        _icon_widgets.push_back(new_widget);

        delete ui->dropAreaContent->layout()->replaceWidget(widget_to_change, new_widget);

        _icon_signal_mapper->setMapping(new_widget, new_widget);
        connect(new_widget, SIGNAL(clicked()), _icon_signal_mapper, SLOT(map()));
        connect(new_widget, SIGNAL(double_clicked(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));
        return;
    }
    if (state == Library::ADD) {
        CreateWidget(album, Type::T_ALBUM);
        return;
    }
    if (state == Library::REMOVE) {
        for (iWidget* const widget : _icon_widgets) {
            if ( widget->getTitle() == album->getTitle()) {
                ui->dropAreaContent->layout()->removeWidget(widget);
                _icon_widgets.removeAt(widget_index);
                delete widget;
                return;
            }
            widget_index++;
        }
        qDebug() << "in MainWin::on_Library_change: Could not find icon of " + album->getTitle() + " to be removed";
    }
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

    if( d_target->isSelected()) return;

    d_target->getIcon()->setStyleSheet("background: #cccccc");

    QLineEdit* icon_title_editor = d_target->getTitleEditor();

    icon_title_editor->setFont(QFont("Tahoma", 11, QFont::Bold));

    d_target->isSelected(true);
    _selected_icons.push_back(d_target);
}

void MainWin::on_Icon_doubleClick(QWidget *target)
{  
        on_Icon_deselect();

        iWidget* icon = dynamic_cast<iWidget*>(target);

        if (icon->getType() == Type::T_SONG) {
            _media_player->stop();
            _media_player->setMedia(QMediaContent(QUrl::fromLocalFile(icon->getPath())));
            Song* song_to_play = _library.getSongByTitle(icon->getTitle());
//            ui->CurrentAlbumLine->setText(song_to_play->getAlbumTitle());
//            ui->CurrentSongLine->setText(song_to_play->getTitle());
            _current_song = song_to_play;
            _media_player->play();
//            icon->isPlaying(true);
        }
        else if (icon->getType() == Type::T_ALBUM) {
            // Open the Album folder
            _cache_dropAreaContent = ui->dropArea->takeWidget();
            QWidget* temp_dropAreaContent = new QWidget(ui->dropArea) ;
            Album* target_album = _library.getAlbumByTitle(icon->getTitle());

            CreateAlbumContentArea(target_album, temp_dropAreaContent);

            connect(_temporary_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));

            ui->dropArea->setWidget(temp_dropAreaContent);
            ui->TitleLibrary->setText(icon->getTitle());
            _temporary_window_entered = true;
        }
}


void MainWin::on_Icon_deselect()
{
    for (iWidget* icon : _selected_icons ) {
        icon->DefaultAdjustement();
        icon->isSelected(false);
    }

    _selected_icons.clear();
    QVector<iWidget*>().swap(_selected_icons);
}

void MainWin::CreateAlbumContentArea(Album* const target_album, QWidget* drop_area_container)
{
    ui->default_placeholder->setVisible(false);

    if (_library.empty()) {
        ui->default_placeholder->setVisible(true);
        drop_area_container->setLayout(new QGridLayout(ui->dropArea));
        drop_area_container->layout()->addWidget(ui->default_placeholder);
        return;
    }

    FlowLayout* flow_layout = new FlowLayout(drop_area_container);
    drop_area_container->setLayout(flow_layout);

    _temporary_signal_mapper = new QSignalMapper(this);

    for (Song& song : *(target_album->getSongs()) ) {
        iWidget* song_icon = new iWidget(new Icon(&song), &_library, drop_area_container);
        song_icon->getIcon()->setParent(song_icon);
        drop_area_container->layout()->addWidget(song_icon);
        _temporary_icon_widgets.push_back(song_icon);

        _temporary_signal_mapper->setMapping(song_icon, song_icon);
        connect(song_icon, SIGNAL(clicked()), _temporary_signal_mapper, SLOT(map()));
        connect(song_icon, SIGNAL(double_clicked(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));
    }
}

void MainWin::on_ButtonDeselect_clicked()
{
    on_Icon_deselect();
}

void MainWin::on_ButtonRemove_clicked()
{
    while( !_selected_icons.empty()) {
        iWidget* icon = _selected_icons.front();

        ui->dropAreaContent->layout()->removeWidget(icon);
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

void MainWin::on_ButtonHome_clicked()
{
    if (_cache_dropAreaContent == nullptr)
        return;

    on_Icon_deselect();
    while( !_temporary_icon_widgets.empty()) {
        iWidget* widget_to_delete = _temporary_icon_widgets.front();
        _temporary_icon_widgets.removeFirst();
        delete widget_to_delete;
    }
    ui->dropArea->setWidget(_cache_dropAreaContent);

    QVector<iWidget*>().swap(_temporary_icon_widgets);

    delete _temporary_signal_mapper;
    _temporary_signal_mapper = nullptr;

    _temporary_window_entered = false;
    _cache_dropAreaContent = nullptr;

    ui->TitleLibrary->setText("MY LIBRARY");
}
