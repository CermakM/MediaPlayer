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

void DialogEditPlaylist::LoadPlaylist() {

    for (Song& song : playlist) {
        QTreeWidgetItem* topItm = new QTreeWidgetItem(ui->treePlaylist);
        topItm->setText(0, song.GetTitle());
        topItm->setText(1, song.GetInterpret());
        if (song.HasAlbum())
            topItm->setText(2, song.GetAlbum());
        ui->treePlaylist->addTopLevelItem(topItm);
    }
}

void DialogEditPlaylist::LoadAlbums() {

    // Create top level item - album
    for (Album& temp_album : album_vector) {
        QTreeWidgetItem* topItm = new QTreeWidgetItem(ui->treeAlbum);
        topItm->setText(0, temp_album.GetTitle());
        topItm->setText(1, temp_album.GetInterpret());
        topItm->setText(2, temp_album.GetYear());
        // Add the top level item to the tree
        ui->treeAlbum->addTopLevelItem(topItm);
        for (Song& temp_song : temp_album.GetSongs()) {
            // Create child item
            QTreeWidgetItem* childItm = new QTreeWidgetItem(topItm);
            childItm->setText(0, temp_song.GetFileName()); // later on - use GetTitle() when there is a title of the song
            // Add the child item
            topItm->addChild(childItm);
        }
    }

}

void DialogEditPlaylist::on_AddToPlaylistButton_clicked()
{

}

void DialogEditPlaylist::on_RemoveFromPlaylistButton_clicked()
{

}
