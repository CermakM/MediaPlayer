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

    _library.playlist_updated = false;

    ui->CurrentSongLine->clear();
    ui->CurrentAlbumLine->clear();
    _media_player->stop();
    _playlist->Update();

    qDebug() << "Actual media player status: " << _playlist->Size();

    _media_player->setPlaylist(_playlist->MediaPlaylist());

    if (!_playlist->empty()) {
        ui->CurrentSongLine->setText(_playlist->CurrentMedia()->getTitle());
        ui->CurrentAlbumLine->setText(_playlist->CurrentMedia()->getAlbumTitle());
    }
}


void MainWin::CreateDropArea()
{
    _icon_signal_mapper = new QSignalMapper(this);
    ui->default_placeholder->setVisible(false);

    FlowLayout* flow_layout = new FlowLayout(ui->dropArea);
    ui->dropAreaContent->setLayout(flow_layout);

    if (_library.empty()) {
        ui->default_placeholder->setVisible(true);
        ui->dropArea->setWidget(ui->dropAreaContent);
        return;
    }

    for (Album* const album : *(_library.getAlbums())) {
        if ( album->getTitle() != "-") {
            CreateWidget(album, T_ALBUM);
        }
        else  {
            for (Song& song : *(album->getSongs())) {
                CreateWidget(&song, T_SONG);
            }
        }
    }

    ui->dropArea->setWidget(ui->dropAreaContent);
}


iWidget* MainWin::CreateWidget(void* const media, Type type, int index) {

    iWidget* new_widget = nullptr;
    if ( type == T_ALBUM ) {
        Album* target_album = static_cast<Album*>(media);
        new_widget = new iWidget(new Icon(target_album), &_library, ui->dropArea);
        for (Song& song : *(target_album->getSongs())) {
            iWidget* child_widget = new iWidget(new Icon(&song), &_library);
            child_widget->getIcon()->setParent(child_widget);
            new_widget->pushChild(child_widget);
        }
}
    else if ( type == T_SONG)
        new_widget = new iWidget(new Icon(static_cast<Song*>(media)), &_library, ui->dropArea);
    else {
        qDebug() << "Icon type T_NOTYPE specified. Default folder will be created";
        new_widget = new iWidget(this);
    }

    new_widget->getIcon()->setParent(new_widget);
    if (index < 0) {
        _icon_widgets.push_back(new_widget);
        ui->dropAreaContent->layout()->addWidget(new_widget);
    }
    else {
        _icon_widgets.insert(index, new_widget);
        dynamic_cast<FlowLayout*>(ui->dropAreaContent->layout() )->addWidget(new_widget);
    }

    _icon_signal_mapper->setMapping(new_widget, new_widget);
    connect(new_widget, SIGNAL(clicked()), _icon_signal_mapper, SLOT(map()));
    connect(new_widget, SIGNAL(double_clicked(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));

    return new_widget;
}

void MainWin::ConnectSignals()
{

    // Connect Signals and slots
    connect(_media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(_media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);
    connect(_media_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(on_Media_change(QMediaPlayer::MediaStatus)));

    // Shortcuts
    QShortcut* shortcutAddAlbum = new QShortcut(QKeySequence("Ctrl+A"), this);
    QShortcut* shortcutAddSongs = new QShortcut(QKeySequence("Ctrl+S"), this);
    QShortcut* shortcutEditPlaylist = new QShortcut(QKeySequence("Ctrl+P"), this);
    QShortcut* shortcutEditLibrary = new QShortcut(QKeySequence("Ctrl+L"), this);
    QShortcut* shortcutPlayButton = new QShortcut(QKeySequence("K"), this);
    QShortcut* shortcutStopButton = new QShortcut(QKeySequence("S"), this);
    QShortcut* shortcutForwardButton = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut* shortcutBackwardButton = new QShortcut(QKeySequence(Qt::Key_Left), this);
    QShortcut* shortcutPositionForward = new QShortcut(QKeySequence(Qt::Key_L), this);
    QShortcut* shortcutPositionBackward = new QShortcut(QKeySequence(Qt::Key_J), this);
    QShortcut* shortcutDeselect = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    QShortcut* shortcutDelete = new QShortcut(QKeySequence(Qt::Key_Delete), this);

    connect(shortcutAddAlbum, SIGNAL(activated()), this, SLOT(on_actionAddNewAlbum_triggered()));
    connect(shortcutAddSongs, SIGNAL(activated()), this, SLOT(on_actionAddNewSongs_triggered()));
    connect(shortcutEditPlaylist, SIGNAL(activated()), this, SLOT(on_actionEditPlaylist_triggered()));
    connect(shortcutEditLibrary, SIGNAL(activated()), this, SLOT(on_actionEditLibrary_triggered()));
    connect(shortcutPlayButton, SIGNAL(activated()), this, SLOT(on_ButtonPlay_clicked()));
    connect(shortcutStopButton, SIGNAL(activated()), this, SLOT(on_ButtonStop_clicked()));
    connect(shortcutForwardButton, SIGNAL(activated()), this, SLOT(on_ButtonForward_clicked()));
    connect(shortcutBackwardButton, SIGNAL(activated()), this, SLOT(on_ButtonBackward_clicked()));
    connect(shortcutPositionForward, SIGNAL(activated()), this, SLOT(on_ProgressSlider_FastForward()));
    connect(shortcutPositionBackward, SIGNAL(activated()), this, SLOT(on_ProgressSlider_FastBackward()));
    connect(shortcutDeselect, SIGNAL(activated()), this, SLOT(on_Icon_deselect()));
    connect(shortcutDelete, SIGNAL(activated()), this, SLOT(on_Icon_remove()));


    connect(_icon_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));
    connect(ui->dropAreaContent, SIGNAL(dropped(const QMimeData*)), this, SLOT(on_Media_drop(const QMimeData*)));
}

void MainWin::on_Media_change(QMediaPlayer::MediaStatus status) {

    Song* song = _playlist->CurrentMedia();

    if (song == nullptr) return;

    song->isPlaying(false);

    qDebug() << "Media change:";

    if (status == QMediaPlayer::LoadingMedia) {
        iWidget* current_widget = nullptr;
        for (iWidget* const widget : _icon_widgets) {
            if (widget->getTitle() == song->getTitle()) {
                _current_song_widget = widget;
                break;
            }
            else {
                for (iWidget* const child_widget : *(widget->getChildWidgets())) {
                    if (child_widget->getTitle() == song->getTitle()) {
                        current_widget = child_widget;
                        break;
                    }
                }
            }
            // Break the cycle if widget is found - saves some time
            if (current_widget != nullptr) break;
        }
        if(current_widget == nullptr) return;

        _current_song_widget = current_widget;
        _current_song = song;

        _current_song_widget->isPlaying(false);

        qDebug() << "\t... Changing media " + _current_song_widget->getTitle();
    }
    else if (status == QMediaPlayer::EndOfMedia) {
        _current_song_widget->isPlaying(false);
        qDebug() << "\t... End of media or stopped media " + _current_song_widget->getTitle();
    }
    else if (status == QMediaPlayer::BufferedMedia || status == QMediaPlayer::LoadedMedia) {
        _current_song_widget->isPlaying(true);
        _current_song = song;
        qDebug() << "\t... Loaded media " + _current_song_widget->getTitle();
    }

    else {

        ui->CurrentAlbumLine->setText("-");
        ui->CurrentSongLine->setText("End of Playlist .. Press PLAY to repeat");
        return;
    }

    ui->CurrentAlbumLine->setText(song->getAlbumTitle());
    ui->CurrentSongLine->setText(song->getTitle());
}


void MainWin::on_EditPlaylistOver(Album* album, Library::ChangeState change) {

    (void*) album;
    if (change)
        UpdatePlaylist();
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

void MainWin::on_ProgressSlider_FastForward()
{
    _media_player->setPosition(_media_player->position() + 5000);
}

void MainWin::on_ProgressSlider_FastBackward()
{
    _media_player->setPosition(_media_player->position() - 5000);
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
    if (state == Library::CHANGE) {
        if (album->getTitle() == '-') {
            QVector<Song>* album_songs = album->getSongs();
            for (iWidget* const widget : _icon_widgets) {
                widget_to_change = widget;
                if (widget_to_change->getAlbumTitle() == '-') {
                    ui->dropAreaContent->layout()->removeWidget(widget_to_change);
                    _icon_widgets.removeOne(widget);
                    delete widget_to_change;
                    widget_to_change = nullptr;
                }
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
                    break;
                }
            }
        }
        if (widget_to_change) {
            qDebug() << "in MainWin::on_Library_change: Widget: " + album->getTitle() + " has been deleted";
            _icon_widgets.removeOne(widget_to_change);
        }
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
        int index_of_new_widget = 0;
        for (int i = 1; i < _icon_widgets.size(); i++) {
            if (_icon_widgets.at(i)->getTitle().at(i) > _icon_widgets.at(i - 1)->getTitle().at(0) &&
                _icon_widgets.at(i)->getType() == T_ALBUM) {
                index_of_new_widget = i;
                break;
            }
        }
        CreateWidget(album, Type::T_ALBUM, index_of_new_widget);
        return;
    }
    if (state == Library::REMOVE) {
        for (iWidget* const widget : _icon_widgets) {
            if ( widget->getTitle() == album->getTitle()) {
                ui->dropAreaContent->layout()->removeWidget(widget);
                _icon_widgets.removeOne(widget);
                delete widget;
                return;
            }
        }
        qDebug() << "in MainWin::on_Library_change: Could not find icon of " + album->getTitle() + " to be removed";
    }
}

void MainWin::on_ButtonForward_clicked()
{
    _current_song_widget->isPlaying(false);
    if (_media_player->playlist()) _media_player->playlist()->next();
}

void MainWin::on_ButtonBackward_clicked()
{
    _current_song_widget->isPlaying(false);
    if (_media_player->playlist()) _media_player->playlist()->previous();
}

void MainWin::on_Media_drop(const QMimeData* mime_data)
{
    QList<QUrl> url_list = mime_data->urls();

    for (QUrl const& url : url_list) {
        if (Library::isAlbum(url.toLocalFile())) {
            Album* album = new Album(url.toLocalFile());
            Library::ChangeState library_status = _library.AddMedia(album);
            if ( library_status == Library::CHANGE) {
                on_Library_change(album, Library::CHANGE);
            }
            else if ( library_status == Library::ADD ) {
                on_Library_change(album, Library::ADD);
            }
        }
        else if (Library::isSong(url.toLocalFile())) {
            if (_library.AddMedia(new Song(url.toLocalFile())) == Library::CHANGE) {
                on_Library_change(_library.at(0), Library::CHANGE);
            }
        }
        else {
            qDebug() << "Dropped media not recognized as Album or Song";
        }
    }
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

        iWidget* target_icon = dynamic_cast<iWidget*>(target);

        if (target_icon->getType() == Type::T_SONG) {
            _media_player->stop();
            if (_current_song_widget) _current_song_widget->isPlaying(false);
            Song* song_to_play = _library.getSongByTitle(target_icon->getTitle());
            // Add a sample song to play (remove after it ends)
            _playlist->PlaySample(song_to_play);
            _current_song_widget = target_icon;
            _current_song = song_to_play;
            _media_player->play();
        }
        else if (target_icon->getType() == Type::T_ALBUM) {
            // Open the Album folder
            _cache_dropAreaContent = ui->dropArea->takeWidget();
            DragArea* temp_dropAreaContent = new DragArea(ui->dropArea);
            temp_dropAreaContent->setAcceptDrops(false);

            CreateAlbumContentArea(target_icon, temp_dropAreaContent);

            connect(_temporary_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));

            ui->dropArea->setWidget(temp_dropAreaContent);
            ui->TitleLibrary->setText(target_icon->getTitle());

            _current_album_widget = target_icon;
            _temporary_window_entered = true;
        }
}

void MainWin::CreateAlbumContentArea(iWidget* const target_widget, DragArea *drop_area_container)
{
    ui->default_placeholder->setVisible(false);

    _current_album_widget = target_widget;

    FlowLayout* flow_layout = new FlowLayout(drop_area_container);
    drop_area_container->setLayout(flow_layout);

    if (_library.empty()) {
        ui->default_placeholder->setVisible(true);
        drop_area_container->layout()->addWidget(ui->default_placeholder);
        return;
    }

    _temporary_signal_mapper = new QSignalMapper(this);

    for (iWidget* const song_icon : *(target_widget->getChildWidgets()) ) {
        song_icon->setParent(ui->dropArea);
        drop_area_container->layout()->addWidget(song_icon);
        _temporary_signal_mapper->setMapping(song_icon, song_icon);
        connect(song_icon, SIGNAL(clicked()), _temporary_signal_mapper, SLOT(map()));
        connect(song_icon, SIGNAL(double_clicked(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));
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

void MainWin::on_Icon_remove()
{
    if (_selected_icons.empty()) return;

    while( !_selected_icons.empty()) {
        iWidget* icon = _selected_icons.front();

        ui->dropAreaContent->layout()->removeWidget(icon);
        if (icon->getType() == Type::T_ALBUM) {
            _library.RemoveMedia(_library.getAlbumByTitle(icon->getTitle()));
        }
        else {
            _library.RemoveMedia(_library.getSongByTitle(icon->getTitle()));
        }

        _icon_widgets.removeOne(icon);
        _selected_icons.removeFirst();
        delete icon;
    }

    QVector<iWidget*>().swap(_selected_icons);

    on_EditPlaylistOver(_library.playlist_updated);
}

void MainWin::on_Icon_AddToPlaylist()
{
    if (_selected_icons.empty()) return;

    for (iWidget* const icon : _selected_icons) {

        if (icon->getType() == Type::T_ALBUM) {
            _playlist->AddMedia(_library.getAlbumByTitle(icon->getTitle()));
        }
        else {
            _playlist->AddMedia(_library.getSongByTitle(icon->getTitle()));
        }
    }

    on_EditPlaylistOver(true);
}


void MainWin::on_ButtonDeselect_clicked()
{
    on_Icon_deselect();
}

void MainWin::on_ButtonRemove_clicked()
{
    on_Icon_remove();
}

void MainWin::on_ButtonHome_clicked()
{
    if (_cache_dropAreaContent == nullptr)
        return;

    on_Icon_deselect();

    for (iWidget* const child_widget : *(_current_album_widget->getChildWidgets())) {
        child_widget->setParent(0);
    }

    ui->dropArea->setWidget(_cache_dropAreaContent);

//    QVector<iWidget*>().swap(_temporary_icon_widgets);

    delete _temporary_signal_mapper;
    _temporary_signal_mapper = nullptr;

    _temporary_window_entered = false;
    _current_album_widget = nullptr;
    _cache_dropAreaContent = nullptr;

    ui->TitleLibrary->setText("MY LIBRARY");
}

void MainWin::on_pushButton_clicked()
{
    on_Icon_AddToPlaylist();
}

void MainWin::on_ButtonRefresh_clicked()
{
    while (!_icon_widgets.empty())
        delete _icon_widgets.takeFirst();

    _selected_icons.clear();
    QVector<iWidget*>().swap(_selected_icons);

    // Default placeholder will not be deleted
    ui->dropAreaContent->layout()->removeWidget(ui->default_placeholder);
    ui->default_placeholder->setParent(ui->dropArea);

    delete ui->dropAreaContent->layout();
    delete ui->dropArea->takeWidget();
    delete _icon_signal_mapper;

    ui->dropAreaContent = new DragArea(ui->dropArea);
    CreateDropArea();

    connect(_icon_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));
    connect(ui->dropAreaContent, SIGNAL(dropped(const QMimeData*)), this, SLOT(on_Media_drop(const QMimeData*)));

    UpdatePlaylist();
}
