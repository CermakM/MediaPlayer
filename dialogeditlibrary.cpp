#include "dialogeditlibrary.h"
#include "ui_dialogeditlibrary.h"

DialogEditLibrary::DialogEditLibrary(Library * const library, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::DialogEditLibrary)
{
    ui->setupUi(this);
    _library =  library;
    _library->infoDatabaseAddress();

    _pseudo_library = new Library(*_library);

    Library::CopyDatabaseFile(DB_FILENAME, TEMP_DB_FILENAME);
     _pseudo_library->setDatabaseFileName(TEMP_DB_FILENAME);

    qInfo() << "Library _pseudo_library database: "; _pseudo_library->infoDatabaseAddress();
    qInfo() << "Library _library database:"; _library->infoDatabaseAddress();

    LoadLibrary();
}

DialogEditLibrary::~DialogEditLibrary() {

    delete ui;
}


void DialogEditLibrary::LoadLibrary() {

    CreateTree(_library->getAlbums());
}


void DialogEditLibrary::CreateTree(QVector<Album*> * const album_vec) {

    for(Album* const album : *album_vec) {
        CreateTreeItem(album);
    }
}


void DialogEditLibrary::CreateTreeItem(Album* const album, QTreeWidgetItem* const tree_album_item) {

    QTreeWidgetItem* album_item = tree_album_item;
    QTreeWidgetItem* song_item  = nullptr;

    if (album->getSongs()->empty()) {
        qDebug() << "in DialogEditLibrary::CreateTreeItem: Empty album has been provided";
        return;
    }

    if (album->getTitle() != "-" && !album_item) {
        album_item = new QTreeWidgetItem(ui->AlbumsTree);
        album_item->setText(0, album->getTitle());
        album_item->setText(1, album->getInterpret());

        // Add the top level item to the tree
        ui->AlbumsTree->addTopLevelItem(album_item);
    }

    for (Song& temp_song : *(album->getSongs())) {
        album_item? song_item = new QTreeWidgetItem(album_item) : song_item = new QTreeWidgetItem(ui->SongsTree);
        // Create child item
        song_item->setText(0, temp_song.getTitle());
        song_item->setText(1, temp_song.getInterpret());
        song_item->setText(2, temp_song.getAlbumTitle());
        song_item->setText(3, QString::number(temp_song.getYear()));
        song_item->setText(4, temp_song.getPath());

        // Add the child item
        album_item? album_item->addChild(song_item) : ui->SongsTree->addTopLevelItem(song_item);
    }
}


void DialogEditLibrary::UpdateTree(bool changed)
{
    if (!changed) return;

    ui->AlbumsTree->clear();
    ui->SongsTree->clear();

    // It's faster to redraw the whole tree
    CreateTree(_pseudo_library->getAlbums());

    _change = true;
}

void DialogEditLibrary::on_AddAlbumsButton_clicked()
{
    // Create empty library without loading the database to make user changes easier
    // In preview mode no changes will be written into database
    DialogAddAlbum AlbumBrowser(_pseudo_library, this);
    connect(&AlbumBrowser, SIGNAL(change(bool)), this, SLOT(UpdateTree(bool)));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void DialogEditLibrary::on_AddSongsButton_clicked()
{
    // Create empty library without loading the database to make user changes easier
    // In preview mode no changes will be written into database
    DialogAddSongs SongsBrowser(_pseudo_library, this);
    connect(&SongsBrowser, SIGNAL(change(bool)), this, SLOT(UpdateTree(bool)));
    SongsBrowser.setWindowTitle("Add your Songs");
    SongsBrowser.setModal(true);
    SongsBrowser.exec();
}

void DialogEditLibrary::on_RemoveButton_clicked()
{
    QList<QTreeWidgetItem*> selected_albums = ui->AlbumsTree->selectedItems();
    QList<QTreeWidgetItem*> selected_songs = ui->SongsTree->selectedItems();

    for (auto const& album_item : selected_albums) {
        Album* album = _pseudo_library->getAlbumByTitle(album_item->text(0));
        delete album_item;
        _pseudo_library->RemoveMedia(album);

    }
    for (auto const& song_item : selected_songs) {
        Song* song = _pseudo_library->getSongByTitle(song_item->text(0));
        delete song_item;
        _pseudo_library->RemoveMedia(song);
    }

    _change = true;

    _pseudo_library->infoDatabaseAddress();
}

void DialogEditLibrary::on_buttonBox_accepted()
{
    QString working_dir = QDir::currentPath();
    QDir::setCurrent(working_dir + "/Media");
    QFile ftoRemove;
    ftoRemove.setFileName(_library->getDatabaseFName());
    if (!ftoRemove.remove()) {
        qDebug() << "In on_buttonBox_accepted : " << ftoRemove.errorString();
    }
    else qDebug() << "File " << ftoRemove.fileName() << " removed successfully" << endl;
    if (!QFile::rename(_pseudo_library->getDatabaseFName(), _library->getDatabaseFName())) {
        qDebug() << "In on_buttonBox_accepted : Could not rename database file";
    }
    QDir::setCurrent(working_dir);

    *_library = *_pseudo_library;
    _library->setDatabaseFileName(DB_FILENAME);


    qInfo() << "Library _library database: "; _library->infoDatabaseAddress();
    qInfo() << "Library _pseudo_library database: "; _pseudo_library->infoDatabaseAddress();

    emit UpdatePlaylist(_change);
}


void DialogEditLibrary::on_buttonBox_rejected()
{
    QString working_dir = QDir::currentPath();
    QDir::setCurrent(working_dir + "/Media");
    QFile::remove(_pseudo_library->getDatabaseFName());
    QDir::setCurrent(working_dir);

    qInfo() << "Library _pseudo_library database: "; _pseudo_library->infoDatabaseAddress();
    qInfo() << "Library _library database:"; _library->infoDatabaseAddress();
    _pseudo_library->takeAlbums();
    delete _pseudo_library;

    emit UpdatePlaylist(true);
}
