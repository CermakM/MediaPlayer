#include "dialogeditplaylist.h"
#include "ui_dialogeditplaylist.h"

DialogEditPlaylist::DialogEditPlaylist(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditPlaylist)
{
    ui->setupUi(this);

    album_vector = nullptr;
    song_vector = nullptr;
    playlist = nullptr;
}

DialogEditPlaylist::~DialogEditPlaylist()
{
    delete ui;
}

DialogEditPlaylist::DialogEditPlaylist(std::vector<Album> & _album, std::vector<Song> & _song, Playlist & _playlist, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogEditPlaylist)
{
    ui->setupUi(this);

    album_vector = &_album;
    song_vector = &_song;
    playlist = &_playlist;

    LoadAlbums();
}


void DialogEditPlaylist::LoadAlbums() {

    // Create top level item - album
    for (Album& temp_album : *album_vector) {
        bool album_goes_to_playlist = temp_album.is_in_playlist;
        QTreeWidgetItem* topItm = new QTreeWidgetItem(ui->treeAlbum);
        topItm->setText(0, temp_album.GetTitle());
        topItm->setText(1, temp_album.GetInterpret());
        topItm->setText(2, temp_album.GetYear());
        // Add the top level item to the tree
        ui->treeAlbum->addTopLevelItem(topItm);
        for (Song& temp_song : temp_album.GetSongs()) {
            // Create child item
            QTreeWidgetItem* childItm = new QTreeWidgetItem(topItm);
            childItm->setText(0, temp_song.GetTitle());
            // Add the child item
            topItm->addChild(childItm);
        }

        if (album_goes_to_playlist) ui->treePlaylist->addTopLevelItem(topItm->clone());
    }
}

void DialogEditPlaylist::on_AddToPlaylistButton_clicked()
{
    QTreeWidgetItem* current_item = ui->treeAlbum->currentItem();
    // Check by title if the album / song is in playlist already
    QList<QTreeWidgetItem*> qlist = ui->treePlaylist->findItems(current_item->text(0), Qt::MatchExactly);
    if (!qlist.empty()) return;

    ui->treePlaylist->addTopLevelItem(current_item->clone());
    new_albums.push_back(current_item);
}

void DialogEditPlaylist::on_RemoveFromPlaylistButton_clicked()
{
   QTreeWidgetItem* current_item = ui->treePlaylist->currentItem();
   current_item->setHidden(true);
   int index = 0;
   for (QTreeWidgetItem* itm : new_albums) {
       if (itm->text(0) == current_item->text(0)) { // item is already in add-list
           new_albums.removeAt(index);
           delete current_item;
           return;
       }
       index++;
   }

   albums_to_remove.push_back(current_item);
}

/// will be library method
Album* DialogEditPlaylist::GetAlbumByTitle(const QString& _title) {

    Album* album_ptr = nullptr;
    for (Album& _album : *album_vector) {
        if ( _album.GetTitle() == _title)
            album_ptr = &_album;
    }

    return album_ptr;
}

/// will be library method
Song* DialogEditPlaylist::GetSongByTitle(const QString& _title) {

    Song* song_ptr = nullptr;
    for( Song& _song : *song_vector) {
        if ( _song.GetTitle() == _title)
            song_ptr = &_song;
    }

    return song_ptr;
}

void DialogEditPlaylist::on_buttonBox_accepted()
{
    // If no change - return
    if ( new_albums.empty() && albums_to_remove.empty()) return;

    QFile infile("albums.txt");
    infile.open(QFile::ReadOnly);
    QString line;
    QStringList list;
    while(!(line = infile.readLine()).isEmpty())
        list.append(line);

    infile.close();

    // Add new albums to playlist
    for (QTreeWidgetItem* itm : new_albums) {
        Album* _album = GetAlbumByTitle(itm->text(0));

        qDebug() << "Itme to be added: " << _album->GetTitle();

        _album->is_in_playlist = true;
        playlist->AddMedia(*_album);

    // Search the title in list file and replace the last symbol
        for(QString& line : list) {
            if(line.contains(_album->GetTitle(), Qt::CaseInsensitive)) {
                line.replace("-", "#");
            }
        }
    }

    // Remove selected items from playlist
    for(QTreeWidgetItem* itm : albums_to_remove) {
        Album* album_to_remove = GetAlbumByTitle(itm->text(0));

        qDebug() << "Item to be removed: " << album_to_remove->GetTitle();

        album_to_remove->is_in_playlist = false;

        playlist->RemoveMedia(*album_to_remove);

        for(QString& line : list) {
            if(line.contains(album_to_remove->GetTitle(), Qt::CaseInsensitive) \
                    && line.contains(album_to_remove->GetYear(), Qt::CaseSensitive)) {
                line.replace("#", "-");
            }
        }

        delete itm;
    }

    infile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream outStream(&infile);
    for( const QString& line : list)
        outStream << line;

    infile.close();

    emit Change(true);
}
