#include "dialogeditlibrary.h"
#include "ui_dialogeditlibrary.h"

DialogEditLibrary::DialogEditLibrary(Library *library, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::DialogEditLibrary)
{
    ui->setupUi(this);
    _library =  library;
    _pseudo_library = new Library(*_library);

    LoadLibrary();
}

DialogEditLibrary::~DialogEditLibrary() {

    delete ui;
    delete _pseudo_library;
}


void DialogEditLibrary::LoadLibrary() {

    CreateTree(_library->getAlbums());
}


void DialogEditLibrary::CreateTree(QVector<Album> * const album_vec) {

    for(Album& album : *album_vec) {
        CreateTreeItem(album);
    }
}


void DialogEditLibrary::CreateTreeItem(Album& album, QTreeWidgetItem* const tree_album_item) {

    QTreeWidgetItem* album_item = tree_album_item;
    QTreeWidgetItem* song_item  = nullptr;

    if (album.getTitle() != "-" && !album_item) {
        album_item = new QTreeWidgetItem(ui->AlbumsTree);
        album_item->setText(0, album.getTitle());
        album_item->setText(1, album.getInterpret());

        // Add the top level item to the tree
        ui->AlbumsTree->addTopLevelItem(album_item);
    }

    for (Song& temp_song : *(album.getSongs())) {
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


void DialogEditLibrary::Update(bool changed)
{
    if (!changed) return;

    ui->AlbumsTree->clear();
    ui->SongsTree->clear();

    CreateTree(_pseudo_library->getAlbums());
}

void DialogEditLibrary::on_AddAlbumsButton_clicked()
{
    // Create empty library without loading the database to make user changes easier
    // In preview mode no changes will be written into database
    DialogAddAlbum AlbumBrowser(_pseudo_library, this);
    connect(&AlbumBrowser, SIGNAL(Change(bool)), this, SLOT(Update(bool)));
    AlbumBrowser.setWindowTitle("Add your Album");
    AlbumBrowser.setModal(true);
    AlbumBrowser.exec();
}

void DialogEditLibrary::on_AddSongsButton_clicked()
{
    // Create empty library without loading the database to make user changes easier
    // In preview mode no changes will be written into database
    DialogAddSongs SongsBrowser(_pseudo_library, this);
    connect(&SongsBrowser, SIGNAL(Change(bool)), this, SLOT(Update(bool)));
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
}

void DialogEditLibrary::on_buttonBox_accepted()
{
    *_library = *_pseudo_library;

    FILE *inFile, *outFile;

    std::string instr = _pseudo_library->_database_path.toStdString();
    inFile = fopen(instr.c_str(), "rb");
    std::string outstr = _library->_database_path.toStdString();
    outFile = fopen(outstr.c_str(), "wb");

    if (inFile == NULL || outFile == NULL) {
        qDebug() << "Error opening database files";
        return;
    }

    auto getFileSize = [](FILE *file)->long {
      fseek(file, 0, SEEK_END);
      long lSize = ftell(file);
      rewind(file);
      return lSize;
    };

    long lCount = getFileSize(inFile);
    void *buffer = malloc(lCount*sizeof(char));

    fread(buffer, 1, lCount, inFile);
    fwrite(buffer, 1, lCount, outFile);

    fclose(inFile);
    fclose(outFile);

}

