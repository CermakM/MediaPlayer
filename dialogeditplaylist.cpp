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
}

DialogEditPlaylist::DialogEditPlaylist(Library* _library, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogEditPlaylist)
{
    ui->setupUi(this);

    library = _library;

    LoadLibrary();
}


void DialogEditPlaylist::LoadLibrary() {

    for (Album& temp_album : *(library->Albums())) {
        QTreeWidgetItem* topItm = new QTreeWidgetItem(ui->treeAlbum);
        topItm->setText(0, temp_album.getTitle());
        topItm->setText(1, temp_album.getInterpret());

        // Add the top level item to the tree
        ui->treeAlbum->addTopLevelItem(topItm);
        QTreeWidgetItem* topPlistItm(topItm->clone());

        for (Song& temp_song : *(temp_album.getSongs())) {
            // Create child item
            QTreeWidgetItem* childItm = new QTreeWidgetItem(topItm);
            childItm->setText(0, temp_song.getTitle());
            childItm->setText(1, temp_song.getInterpret());
            childItm->setText(2, temp_song.getAlbumTitle());
            childItm->setText(3, QString::number(temp_song.getYear()));
            childItm->setText(4, temp_song.getPath());

            // Add the child item
            topItm->addChild(childItm);

            if(temp_song.is_in_playlist) topPlistItm ->addChild(childItm->clone());
        }

        if(topPlistItm->childCount()) ui->treePlaylist->addTopLevelItem(topPlistItm);
        else delete topPlistItm;
    }
}

void DialogEditPlaylist::on_AddToPlaylistButton_clicked()
{
    QTreeWidgetItem* current_media = ui->treeAlbum->currentItem();

    // Check by title if the album / song is in playlist already
    QList<QTreeWidgetItem*> qlist = ui->treePlaylist->findItems(current_media->text(0), Qt::MatchExactly | Qt::MatchRecursive);
    if (!qlist.empty()) return;

    ui->treePlaylist->addTopLevelItem(current_media->clone());

    new_media.push_back(current_media);
}

void DialogEditPlaylist::on_RemoveFromPlaylistButton_clicked()
{
   QTreeWidgetItem* current_item = ui->treePlaylist->currentItem();
   current_item->setHidden(true);

   if (new_media.contains(current_item)) {
       new_media.removeOne(current_item);
       delete current_item;
       return;
   }

   items_to_remove.push_back(current_item);
}


void DialogEditPlaylist::on_buttonBox_accepted()
{
    // If no change - return
    if ( new_media.empty() && items_to_remove.empty()) return;

//    /
//    /
//    /     SQL handling
//    /
//    /
//    /

    // Add new media to playlist
    for (QTreeWidgetItem* itm : new_media) {
        if(itm->childCount()) {
            QString album_title = itm->text(0);
            Album* album = library->getAlbumByTitle(album_title);

            qDebug() << "Add to playlist... \nAlbum to be added: " << album->getTitle();

            library->getPlaylist()->AddMedia(album);
        }
        else {
            QString song_title = itm->text(0);
            Song* song = library->getSongByTitle(song_title);

            qDebug() << "Add to playlist... \nSong to be added:" << song->getTitle();

            library->getPlaylist()->AddMedia(song);
        }
    }

    // Remove selected items from playlist

    for(QTreeWidgetItem* itm : items_to_remove) {

        // Check if song has been selected or whole album
        if (!itm->childCount()) {
            Song* song_to_remove = library->getSongByTitle(itm->text(0));
            //if (song_to_remove->is_in_playlist)
                library->getPlaylist()->RemoveMedia(song_to_remove);
        }
        else {
            Album* album_to_remove = library->getAlbumByTitle(itm->text(0));

            qDebug() << "Item to be removed: " << album_to_remove->getTitle();

            library->getPlaylist()->RemoveMedia(album_to_remove);


        }
        delete itm;
    }

//    /
//    /
//    /     SQL handling
//    /
//    /
//    /

    emit Change(true);
}
