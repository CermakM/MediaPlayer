#include "dialogeditplaylist.h"
#include "ui_dialogeditplaylist.h"

DialogEditPlaylist::DialogEditPlaylist(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogEditPlaylist)
{
    ui->setupUi(this);
    _library = nullptr;
}

DialogEditPlaylist::~DialogEditPlaylist()
{
    delete ui;
}

DialogEditPlaylist::DialogEditPlaylist(Library* library, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogEditPlaylist)
{
    ui->setupUi(this);

    _library = library;

    LoadLibrary();
}


void DialogEditPlaylist::LoadLibrary() {

    for (Album* const temp_album : *(_library->getAlbums())) {
        QTreeWidgetItem* topItm = new QTreeWidgetItem(ui->treeAlbum);
        topItm->setText(0, temp_album->getTitle());
        topItm->setText(1, temp_album->getInterpret());

        // Add the top level item to the tree
        ui->treeAlbum->addTopLevelItem(topItm);
        QTreeWidgetItem* topPlistItm(topItm->clone());

        for (Song& temp_song : *(temp_album->getSongs())) {
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

void DialogEditPlaylist::on_AddButton_clicked()
{
    QTreeWidgetItem* current_media = ui->treeAlbum->currentItem();

    if (current_media == nullptr) return;

    // Check by title if the album / song is in playlist already
    QList<QTreeWidgetItem*> qlist = ui->treePlaylist->findItems(current_media->text(0), Qt::MatchExactly | Qt::MatchRecursive);
    if (!qlist.empty()) return;

    ui->treePlaylist->addTopLevelItem(current_media->clone());

    _new_media.push_back(current_media);
}

void DialogEditPlaylist::on_RemoveButton_clicked()
{
   QTreeWidgetItem* current_item = ui->treePlaylist->currentItem();
   current_item->setHidden(true);

   if (_new_media.contains(current_item)) {
       _new_media.removeOne(current_item);
       delete current_item;
       return;
   }

   _items_to_remove.push_back(current_item);
}


void DialogEditPlaylist::on_buttonBox_accepted()
{
    // If no change - return
    if ( _new_media.empty() && _items_to_remove.empty()) return;

    // Add new media to playlist
    for (QTreeWidgetItem* itm : _new_media) {
        if(itm->childCount()) {
            QString album_title = itm->text(0);
            Album* album = _library->getAlbumByTitle(album_title);

            qDebug() << "Add to playlist... \nAlbum to be added: " << album->getTitle();

            _library->getPlaylist()->AddMedia(album);
        }
        else {
            QString song_title = itm->text(0);
            Song* song = _library->getSongByTitle(song_title);

            qDebug() << "Add to playlist... \nSong to be added:" << song->getTitle();

            _library->getPlaylist()->AddMedia(song);
        }
    }

    // Remove selected items from playlist

    for(QTreeWidgetItem* itm : _items_to_remove) {

        // Check if song has been selected or whole album
        if (!itm->childCount()) {
            Song* song_to_remove = _library->getSongByTitle(itm->text(0));
            //if (song_to_remove->is_in_playlist)
                _library->getPlaylist()->RemoveMedia(song_to_remove);
        }
        else {
            Album* album_to_remove = _library->getAlbumByTitle(itm->text(0));

            qDebug() << "Item to be removed: " << album_to_remove->getTitle();

            _library->getPlaylist()->RemoveMedia(album_to_remove);


        }
        delete itm;
    }

    emit change(true);
}
