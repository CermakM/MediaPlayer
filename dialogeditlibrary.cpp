#include "dialogeditlibrary.h"
#include "ui_dialogeditlibrary.h"

DialogEditLibrary::DialogEditLibrary(Library *library, QWidget *parent) :
   QDialog(parent),
   ui(new Ui::DialogEditLibrary)
{
    ui->setupUi(this);
    _library =  library;

    LoadLibrary();
}

DialogEditLibrary::~DialogEditLibrary() {

    delete ui;
}


void DialogEditLibrary::LoadLibrary() {

    for (Album& temp_album : *(_library->Albums())) {
        QTreeWidgetItem* album_item = nullptr;
        QTreeWidgetItem* song_item = nullptr;

        if (temp_album.getTitle() != "-") {
            album_item = new QTreeWidgetItem(ui->AlbumsTree);
            album_item->setText(0, temp_album.getTitle());
            album_item->setText(1, temp_album.getInterpret());

            // Add the top level item to the tree
            ui->AlbumsTree->addTopLevelItem(album_item);
        }

        for (Song& temp_song : *(temp_album.getSongs())) {
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
}

void DialogEditLibrary::on_AddButton_clicked()
{

}

void DialogEditLibrary::on_RemoveButton_clicked()
{

}

void DialogEditLibrary::on_buttonBox_accepted()
{

}
