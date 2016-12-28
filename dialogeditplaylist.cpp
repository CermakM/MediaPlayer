#include "dialogeditplaylist.h"
#include "ui_dialogeditplaylist.h"

DialogEditPlaylist::DialogEditPlaylist(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditPlaylist)
{
    ui->setupUi(this);
}

DialogEditPlaylist::~DialogEditPlaylist()
{
    delete ui;

    album_vector = nullptr;
    song_vector = nullptr;
    playlist = nullptr;
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
    new_items.push_back(current_item);
}

void DialogEditPlaylist::on_RemoveFromPlaylistButton_clicked()
{
   QTreeWidgetItem* current_item = ui->treePlaylist->currentItem();
   current_item->setHidden(true);
   int index = 0;
   for (QTreeWidgetItem* itm : new_items) {
       if (itm->text(0) == current_item->text(0)) { // item is already in add-list
           new_items.removeAt(index);
           delete current_item;
           return;
       }
       index++;
   }

   items_to_remove.push_back(current_item);
}

Album* DialogEditPlaylist::GetAlbumByTitle(const QString& _title) {

    Album* album_ptr = nullptr;
    for (Album& _album : *album_vector) {
        if ( _album.GetTitle() == _title)
            album_ptr = &_album;
    }

    return album_ptr;
}


void DialogEditPlaylist::on_buttonBox_accepted()
{
    // If no change - return
    if ( new_items.empty() && items_to_remove.empty()) return;

    QFile infile("albums.txt");
    infile.open(QFile::ReadOnly);
    QString line;
    QStringList list;
    while(!(line = infile.readLine()).isEmpty())
        list.append(line);

    infile.close();

    // Add new albums to playlist
    for (QTreeWidgetItem* itm : new_items) {
        Album* _album = GetAlbumByTitle(itm->text(0));

        qDebug() << "Itme to be added: " << _album->GetTitle();

        _album->is_in_playlist = true;
        playlist->push_back(*_album);

    // Search the title in list file and replace the last symbol
        for(QString& line : list) {
            if(line.contains(_album->GetTitle(), Qt::CaseInsensitive)) {
                line.replace("-", "#");
            }
        }
    }

    // Remove selected items from playlist
    for(QTreeWidgetItem* itm : items_to_remove) {
        Album* album_to_remove = GetAlbumByTitle(itm->text(0));

        qDebug() << "Item to be removed: " << album_to_remove->GetTitle();

        album_to_remove->is_in_playlist = false;
        // get the indexes of items to be removed
        std::vector<int> indexes;
        int index = 0;
        for (MusicType _itm : *playlist) {
            Album _album = boost::get<Album>(_itm);
            if (album_to_remove->GetTitle() == _album.GetTitle()) {
                indexes.push_back(index);
            }
            index++;
        }

        for (int _index : indexes)
            playlist->erase(playlist->begin() + _index);

        for(QString& line : list) {
            if(line.contains(album_to_remove->GetTitle(), Qt::CaseInsensitive)) {
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
