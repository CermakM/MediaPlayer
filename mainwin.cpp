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

    _default_action = new CustomActionRecent("No recently played songs", this);
    _default_action->setDisabled(true);
    ui->menuRecent->addAction(_default_action);
    ui->menuRecent->setDefaultAction(_default_action);
    ui->default_placeholder->setParent(ui->dropArea);

    CreateDropArea();

    ConnectSignals();

    UpdatePlaylist();

    _cache_dropAreaContent = ui->dropArea->widget();
}

MainWin::~MainWin()
{
    delete _default_action;
    while (!_recent_actions.empty()) {
        delete _recent_actions.takeFirst();
    }
    delete ui;
}

void MainWin::CreateGraphics()
{
    // TODO: Create at least a background you dumbass...
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
    delete ui->dropArea->takeWidget();
    _icon_signal_mapper = new QSignalMapper(this);
    ui->dropAreaContent = new DragArea();

    ui->default_placeholder->setVisible(false);

    FlowLayout* flow_layout = new FlowLayout(ui->dropAreaContent);
    ui->dropAreaContent->setLayout(flow_layout);
    ui->dropAreaContent->setParent(ui->dropArea);

    if (_library.empty()) {
        ui->default_placeholder->setVisible(true);
        ui->dropArea->setWidget(ui->dropAreaContent);
        return;
    }

    for (Album* const album : *(_library.getAlbums())) {
        if ( album->getTitle() != "-") {
            CreateWidget(album, T_ALBUM);
        }
    }
    // Songs widgets are created last
    Album* const album = _library.getAlbums()->first();
    for (Song& song : *(album->getSongs())) {
        CreateWidget(&song, T_SONG);
    }

    ui->dropArea->setWidget(ui->dropAreaContent);
    connect(ui->dropAreaContent, SIGNAL(selected(QRect&)), this, SLOT(on_Icon_rectangularSelection(QRect&)));
    connect(ui->dropAreaContent, SIGNAL(clicked(QMouseEvent*)), this, SLOT(on_DragArea_pressEvent(QMouseEvent*)));
}


iWidget* MainWin::CreateWidget(void* const media, Type type, int index) {

    iWidget* new_widget = nullptr;
    if ( type == T_ALBUM ) {
        Album* target_album = static_cast<Album*>(media);
        new_widget = new iWidget(new Icon(target_album), &_library, ui->dropAreaContent);
        for (Song& song : *(target_album->getSongs())) {
            iWidget* child_widget = new iWidget(new Icon(&song), &_library);
            child_widget->getIcon()->setParent(child_widget);
            song.setWidget(child_widget);
            new_widget->pushChild(child_widget);
        }
}
    else if ( type == T_SONG)
        new_widget = new iWidget(new Icon(static_cast<Song*>(media)), &_library, ui->dropAreaContent);
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
    connect(new_widget, SIGNAL(right_clicked(iWidget*)), this, SLOT(on_Icon_rightClick(iWidget*)));
    connect(new_widget, SIGNAL(double_clicked(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));

    return new_widget;
}

void MainWin::CreateWidgetMenu(iWidget * const target)
{
    iWidgetMenu icon_widget_menu(this);

    icon_widget_menu.addAction("Play", this, SLOT(on_Icon_doubleClick()));
    if (Type::T_ALBUM == target->getType()) {
        icon_widget_menu.addAction("Peak", this, SLOT(on_Icon_doubleClick()));
    }
    icon_widget_menu.addAction("Add to Playlist", this, SLOT(on_Icon_AddToPlaylist()));
    icon_widget_menu.addAction("Remove from Playlist", this, SLOT(on_Icon_RemoveFromPlaylist()));
    icon_widget_menu.addAction("Remove", this, SLOT(on_Icon_removeSelected()));
    icon_widget_menu.addAction("Properties", this, SLOT(on_Icon_Properties()));

    icon_widget_menu.exec(QCursor::pos());
}

void MainWin::CreateAlbumContentArea(iWidget* const target_widget, DragArea *drop_area_container)
{
    ui->default_placeholder->setVisible(false);

    _current_album_widget = target_widget;

    FlowLayout* flow_layout = new FlowLayout(drop_area_container);
    drop_area_container->setLayout(flow_layout);
    drop_area_container->setParent(ui->dropArea);

    if (_library.empty()) {
        ui->default_placeholder->setVisible(true);
        drop_area_container->layout()->addWidget(ui->default_placeholder);
        return;
    }

    _temporary_signal_mapper = new QSignalMapper(this);

    for (iWidget* const song_icon : *(target_widget->getChildWidgets()) ) {
        song_icon->setParent(drop_area_container);
        drop_area_container->layout()->addWidget(song_icon);
        _temporary_signal_mapper->setMapping(song_icon, song_icon);
        connect(song_icon, SIGNAL(clicked()), _temporary_signal_mapper, SLOT(map()));
        connect(song_icon, SIGNAL(right_clicked(iWidget*)), this, SLOT(on_Icon_rightClick(iWidget*)));
        connect(song_icon, SIGNAL(double_clicked(QWidget*)), this, SLOT(on_Icon_doubleClick(QWidget*)));
    }

    connect(drop_area_container, SIGNAL(selected(QRect&)), this, SLOT(on_Icon_rectangularSelection(QRect&)));
    connect(drop_area_container, SIGNAL(clicked(QMouseEvent*)), this, SLOT(on_DragArea_pressEvent(QMouseEvent*)));
}

void MainWin::ShowAlbums(bool on)
{
    for (iWidget* const icon : _icon_widgets) {
        if (Type::T_ALBUM == icon->getType()) {
            icon->setVisible(on);
        }
    }
}

void MainWin::ShowSongs(bool on)
{
    for (iWidget* const icon : _icon_widgets) {
        if (Type::T_SONG == icon->getType()) {
            icon->setVisible(on);
        }
    }
}


void MainWin::ConnectSignals()
{

    // Connect Signals and slots
    connect(_media_player, &QMediaPlayer::positionChanged, this, &MainWin::on_PositionChange);
    connect(_media_player, &QMediaPlayer::durationChanged, this, &MainWin::on_DurationChange);
    connect(_media_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(on_Media_change(QMediaPlayer::MediaStatus)));

    connect(ui->actionShowAll, SIGNAL(changed()), this, SLOT(on_actionShowAll_change()));

    // Shortcuts
    QShortcut* shortcutAbout = new QShortcut(QKeySequence(Qt::Key_F1), this);
    QShortcut* shortcutAddAlbum = new QShortcut(QKeySequence("Ctrl+Shift+A"), this);
    QShortcut* shortcutAddSongs = new QShortcut(QKeySequence("Ctrl+Shift+S"), this);
    QShortcut* shortcutEditPlaylist = new QShortcut(QKeySequence("Ctrl+Shift+P"), this);
    QShortcut* shortcutEditLibrary = new QShortcut(QKeySequence("Ctrl+Shift+L"), this);
    QShortcut* shortcutSearch = new QShortcut(QKeySequence("Ctrl+F"), this);
    QShortcut* shortcutMute = new QShortcut(QKeySequence("M"), this);
    QShortcut* shortcutPlayButton = new QShortcut(QKeySequence("K"), this);
    QShortcut* shortcutStopButton = new QShortcut(QKeySequence("S"), this);
    QShortcut* shortcutForwardButton = new QShortcut(QKeySequence(Qt::Key_Right), this);
    QShortcut* shortcutBackwardButton = new QShortcut(QKeySequence(Qt::Key_Left), this);
    QShortcut* shortcutPositionForward = new QShortcut(QKeySequence(Qt::Key_L), this);
    QShortcut* shortcutPositionBackward = new QShortcut(QKeySequence(Qt::Key_J), this);
    QShortcut* shortcutDeselect = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    QShortcut* shortcutDelete = new QShortcut(QKeySequence(Qt::Key_Delete), this);
    QShortcut* shortcutHome = new QShortcut(QKeySequence(Qt::Key_Backspace), this);

    connect(shortcutAbout, SIGNAL(activated()), this, SLOT(on_actionAbout_triggered()));
    connect(shortcutAddAlbum, SIGNAL(activated()), this, SLOT(on_actionAddNewAlbum_triggered()));
    connect(shortcutAddSongs, SIGNAL(activated()), this, SLOT(on_actionAddNewSongs_triggered()));
    connect(shortcutEditPlaylist, SIGNAL(activated()), this, SLOT(on_actionEditPlaylist_triggered()));
    connect(shortcutEditLibrary, SIGNAL(activated()), this, SLOT(on_actionEditLibrary_triggered()));
    connect(shortcutSearch, SIGNAL(activated()), this, SLOT(on_actionSearch_triggered()));
    connect(shortcutMute, SIGNAL(activated()), this, SLOT(on_VolumeSlider_mute()));
    connect(shortcutPlayButton, SIGNAL(activated()), this, SLOT(on_ButtonPlay_clicked()));
    connect(shortcutStopButton, SIGNAL(activated()), this, SLOT(on_ButtonStop_clicked()));
    connect(shortcutForwardButton, SIGNAL(activated()), this, SLOT(on_ButtonForward_clicked()));
    connect(shortcutBackwardButton, SIGNAL(activated()), this, SLOT(on_ButtonBackward_clicked()));
    connect(shortcutPositionForward, SIGNAL(activated()), this, SLOT(on_ProgressSlider_FastForward()));
    connect(shortcutPositionBackward, SIGNAL(activated()), this, SLOT(on_ProgressSlider_FastBackward()));
    connect(shortcutDeselect, SIGNAL(activated()), this, SLOT(on_Icon_deselect()));
    connect(shortcutDelete, SIGNAL(activated()), this, SLOT(on_Icon_removeSelected()));
    connect(shortcutHome, SIGNAL(activated()), this, SLOT(on_ButtonHome_clicked()));


    connect(_icon_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));
    connect(ui->dropAreaContent, SIGNAL(dropped(const QMimeData*)), this, SLOT(on_Media_drop(const QMimeData*)));
}

void MainWin::AddRecentSong(iWidget* const target)
{
    // Works as a stack of size DEFAULT_RECENT_COUNT
    ui->menuRecent->defaultAction()->setVisible(false);
    CustomActionRecent* action = new CustomActionRecent(target, this);

    for (auto const& c_action : _recent_actions) {
        if ( c_action->text() == action->text()) {
            delete action;
            return;
        }
    }

    connect(action, SIGNAL(triggered(iWidget*)), this, SLOT(on_actionRecent_triggered(iWidget*)));

   if ( !_recent_actions.empty()) {
       for (CustomActionRecent* const current_action : _recent_actions) {
            ui->menuRecent->removeAction(current_action);
       }
       if (_recent_actions.size() == DEFAULT_RECENT_COUNT) {
           delete _recent_actions.takeLast();
       }
   }

   _recent_actions.push_front(action);
   for (CustomActionRecent* const current_action : _recent_actions) {
        ui->menuRecent->addAction(current_action);
   }
}

void MainWin::resizeEvent(QResizeEvent *event)
{
    ui->dropAreaContent->setGeometry(ui->dropArea->geometry());
}

void MainWin::on_Media_change(QMediaPlayer::MediaStatus status) {

    Song* song = _playlist->CurrentMedia();

    if (song == nullptr && !_playlist->empty()) {
        ui->CurrentAlbumLine->setText("-");
        ui->CurrentSongLine->setText("End of Playlist .. Press PLAY to repeat");
        return;
    }
    if (song == nullptr)
        return;

    song->isPlaying(false);

    qDebug() << "Media change:";

    if (status == QMediaPlayer::LoadingMedia) {
        iWidget* current_widget = nullptr;
        for (iWidget* const widget : _icon_widgets) {
            if (widget->getTitle() == song->getTitle() && widget->getType() == T_SONG) {
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


void MainWin::on_EditPlaylistOver(Album*, Library::ChangeState change) {

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
        _media_player->setMuted(true);
        qInfo("The volume is now off");
    }
    else
        _media_player->setMuted(false);

    _media_player->setVolume(value);
}

void MainWin::on_VolumeSlider_mute()
{
    if (!_media_player->isMuted()) {
        _cache_volume = ui->VolumeSlider->value();
        ui->VolumeSlider->setValue(0);
         _media_player->setMuted(true);
    }
    else {
        ui->VolumeSlider->setValue(_cache_volume);
        _media_player->setMuted(false);
    }
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
        iWidget* new_widget = new iWidget(new Icon(album), &_library, ui->dropAreaContent);
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
        on_ButtonRefresh_clicked();
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
    if(_current_song_widget) _current_song_widget->isPlaying(false);
    if (_media_player->playlist()) _media_player->playlist()->next();
}

void MainWin::on_ButtonBackward_clicked()
{
    if(_current_song_widget) _current_song_widget->isPlaying(false);
    if (_media_player->playlist()) _media_player->playlist()->previous();
}

void MainWin::on_Media_drop(const QMimeData* mime_data)
{
    QList<QUrl> url_list = mime_data->urls();
    Library::ChangeState  library_status;
    for (QUrl const& url : url_list) {
        if (Library::isAlbum(url.toLocalFile())) {
            Album temp_album(url.toLocalFile());
            // Checks if the album already exists in library and return pointer
            Album* album = _library.getAlbumByTitle(temp_album.getTitle());
            if (!album)
                album = new Album(temp_album);

            library_status = _library.AddMedia(album);
            if (Library::CHANGE == library_status) {
                on_Library_change(album, Library::CHANGE);
            }
            else if (Library::ADD == library_status) {
                on_Library_change(album, Library::ADD);
            }
            else if (Library::NOCHANGE == library_status ) {
                qDebug() << "Album is already in library";
            }
        }
        else if (Library::isSong(url.toLocalFile())) {
            library_status = _library.AddMedia(new Song(url.toLocalFile()));
            if (Library::CHANGE == library_status) {
                on_Library_change(_library.at(0), Library::CHANGE);
            }
            else if ( Library::NOCHANGE == library_status) {
                qDebug() << "Song is already in library";
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

    d_target->isSelected(true);
    _selected_icons.push_back(d_target);
}

void MainWin::on_Icon_rightClick(iWidget *target)
{
    on_Icon_deselect();
    on_Icon_click(target);
    CreateWidgetMenu(target);
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
            _playlist->AddSample(song_to_play);
            _current_song_widget = target_icon;
            _current_song = song_to_play;
            _media_player->play();

            // Add the song widget to recents
            AddRecentSong(target_icon);

        }
        else if (target_icon->getType() == Type::T_ALBUM) {
            // Open the Album folder
            ui->dropArea->takeWidget();
            DragArea* temp_dropAreaContent = new DragArea();
            temp_dropAreaContent->setAcceptDrops(false);

            CreateAlbumContentArea(target_icon, temp_dropAreaContent);

            connect(_temporary_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));

            ui->dropArea->setWidget(temp_dropAreaContent);
            ui->TitleLibrary->setText(target_icon->getInterpret() + "  -  " + target_icon->getAlbumTitle());

            _current_album_widget = target_icon;
            _temporary_window_entered = true;
        }
}

void MainWin::on_Icon_doubleClick()
{
    if (!_selected_icons.empty())
        on_Icon_doubleClick(_selected_icons.first());
}

void MainWin::on_Icon_Properties()
{
    if (!_selected_icons.empty())
        qDebug() << "Properties of: " << _selected_icons.front()->getTitle();
}

void MainWin::on_Icon_Play()
{
    if ( _selected_icons.empty() )
        return;

    for ( iWidget* icon : _selected_icons) {
        if ( Type::T_ALBUM == icon->getType()) {
            for (iWidget* child : *icon->getChildWidgets()) {
                Song* song = _library.getSongByTitle(child->getTitle());
                if (!song) _playlist->AddSample(song);
            }
            _media_player->play();
        }
        else if (Type::T_SONG == icon->getType()) {
            _playlist->AddSample(_library.getSongByTitle(icon->getTitle()));
            _media_player->play();
        }
    }
}

void MainWin::on_Icon_rectangularSelection(QRect& rect)
{
    QVector<iWidget*> widgets;
    if (_temporary_window_entered)
        widgets = *_current_album_widget->getChildWidgets();
    else
        widgets = _icon_widgets;
    for (iWidget* const widget : widgets) {
//        QRect widget_rect = widget->rect();
//        widget_rect.moveTopLeft(widget->pos());
        QPoint widget_center = widget->pos() + QPoint(widget->width() /2, widget->height() /2);
        if ( /*rect.intersects(widget_rect) */ rect.contains(widget_center)) {
            on_Icon_click(widget);
        }
    }
}

void MainWin::on_Icon_deselect()
{
    for (iWidget* icon : _selected_icons ) {
        icon->isSelected(false);
    }

    _selected_icons.clear();
    QVector<iWidget*>().swap(_selected_icons);
}

void MainWin::on_Icon_removeSelected()
{
    if (_selected_icons.empty()) return;

    while( !_selected_icons.empty()) {
        iWidget* icon = _selected_icons.front();

        ui->dropAreaContent->layout()->removeWidget(icon);
        if (icon->getAlbumWidget())
            icon->getAlbumWidget()->removeChild(icon);
        _icon_widgets.removeOne(icon);

        if (icon->getType() == Type::T_ALBUM) {
            _library.RemoveMedia(_library.getAlbumByTitle(icon->getTitle()));
        }
        else {
            _library.RemoveMedia(_library.getSongByTitle(icon->getTitle()));

        }

        _selected_icons.removeFirst();
        icon->deleteLater();
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

void MainWin::on_Icon_RemoveFromPlaylist()
{
    if (_selected_icons.empty()) return;

    for (iWidget* const icon : _selected_icons) {

        if (icon->getType() == Type::T_ALBUM) {
            _playlist->RemoveMedia(_library.getAlbumByTitle(icon->getTitle()));
        }
        else {
            _playlist->RemoveMedia(_library.getSongByTitle(icon->getTitle()));
            if (_current_song_widget && _current_song_widget == icon) _current_song_widget->isPlaying(false);
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
    on_Icon_removeSelected();
}

void MainWin::on_ButtonHome_clicked()
{
    if (_cache_dropAreaContent == nullptr && !_temporary_window_entered)
        return;

    on_Icon_deselect();

    for (iWidget* const child_widget : *(_current_album_widget->getChildWidgets())) {
        child_widget->setParent(0);
    }

    ui->dropArea->setWidget(_cache_dropAreaContent);

    delete _temporary_signal_mapper;
    _temporary_signal_mapper = nullptr;

    _temporary_window_entered = false;
    _current_album_widget = nullptr;

    ui->TitleLibrary->setText("MY LIBRARY");
}

void MainWin::on_pushButton_clicked()
{
    on_Icon_AddToPlaylist();
}

void MainWin::on_ButtonRefresh_clicked()
{
    if (_current_song_widget) {
        _current_song_widget->isPlaying(false);
        _current_song_widget = nullptr;
    }
    _current_album_widget = nullptr;

    while (!_icon_widgets.empty())
        delete _icon_widgets.takeFirst();

    _selected_icons.clear();
    QVector<iWidget*>().swap(_selected_icons);

    delete ui->dropAreaContent->layout();
    delete _icon_signal_mapper;

    CreateDropArea();

    connect(_icon_signal_mapper, SIGNAL(mapped(QWidget*)), this, SLOT(on_Icon_click(QWidget*)));
    connect(ui->dropAreaContent, SIGNAL(dropped(const QMimeData*)), this, SLOT(on_Media_drop(const QMimeData*)));

    _playlist->RemoveAllSamples();

    UpdatePlaylist();
}

void MainWin::on_actionAbout_triggered()
{
    DialogAbout dialog_about(this);
    dialog_about.setModal(true);

    dialog_about.exec();
}

void MainWin::on_actionShortcuts_triggered()
{
    DialogShortcutcs dialog_shortcuts(this);
    dialog_shortcuts.setModal(true);

    dialog_shortcuts.exec();
}

void MainWin::on_actionRecent_triggered(iWidget *target)
{
    on_Icon_doubleClick(target);
}

void MainWin::on_DragArea_pressEvent(QMouseEvent *event)
{
    if (Qt::RightButton == event->buttons() && !_selected_icons.empty()) {

        iWidgetMenu icon_widget_menu(this);

        icon_widget_menu.addAction("Play", this, SLOT(on_Icon_Play()));

        // Check if there is a song among selected widgets
        bool contains_song = false;
        for (iWidget* icon : _selected_icons ) {
            if (Type::T_SONG == icon->getType()) {
                contains_song = true;
                break;
            }
        }
        if (!contains_song) icon_widget_menu.addAction("Peak", this, SLOT(on_Icon_doubleClick()));
        icon_widget_menu.addAction("Add to Playlist", this, SLOT(on_Icon_AddToPlaylist()));
        icon_widget_menu.addAction("Remove from Playlist", this, SLOT(on_Icon_RemoveFromPlaylist()));
        icon_widget_menu.addAction("Remove", this, SLOT(on_Icon_removeSelected()));
        // TODO:
        // icon_widget_menu.addAction("Properties", this, SLOT(on_Icon_Properties()));

        icon_widget_menu.exec(QCursor::pos());
    }
    else {
        on_Icon_deselect();
    }
}

void MainWin::on_actionShowAll_triggered(bool checked)
{
   on_actionShowAlbums_triggered(checked);
   on_actionShowSongs_triggered(checked);
}

void MainWin::on_actionShowAll_change()
{
    on_ButtonHome_clicked();
    if (!ui->actionShowAll->isChecked()) {
        ui->statusBar->showMessage("Warning:    some filters are turned off - some widget might be hidden!");
    }
    else
        ui->statusBar->clearMessage();
}

void MainWin::on_actionShowAlbums_triggered(bool checked)
{

    ui->actionShowAlbums->setChecked(checked);
    ShowAlbums(checked);
    if (ui->actionShowSongs->isChecked()) ui->actionShowAll->setChecked(checked);
}

void MainWin::on_actionShowSongs_triggered(bool checked)
{
    ui->actionShowSongs->setChecked(checked);
    ShowSongs(checked);
    if (ui->actionShowAlbums->isChecked()) ui->actionShowAll->setChecked(checked);
}

void MainWin::on_actionSearch_triggered()
{
    DialogSearch dialog_search(_icon_widgets, this);
    dialog_search.setModal(false);
    connect(&dialog_search, SIGNAL(selected(iWidget*)), this, SLOT(on_Icon_search(iWidget*)));

    dialog_search.exec();
}

void MainWin::on_Icon_search(iWidget *target)
{
    if (Type::T_ALBUM == target->getType() || "-" == target->getAlbumTitle()) {
        on_ButtonHome_clicked();
        on_Icon_click(target);
    }
    else {
        iWidget* album_widget = target->getAlbumWidget();
        on_Icon_doubleClick(album_widget);
        on_Icon_click(target);
    }
}


void MainWin::on_actionRandomSong_triggered()
{
    if (_playlist->empty()) {
        // TODO: inform user
        return;
    }
    int widget_count = _playlist->Size();
    int rand_song = rand() % widget_count;
    Song* song = _playlist->at(rand_song);
    on_Icon_doubleClick(song->getWidget());
}
