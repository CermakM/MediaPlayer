#include "dialogaddsongs.h"
#include "ui_dialogaddsongs.h"

DialogAddSongs::DialogAddSongs(Library* _library, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddSongs)
{
    ui->setupUi(this);

    library = _library;

    setWindowTitle("Add your Songs");
}

DialogAddSongs::~DialogAddSongs()
{
    delete ui;
}

void DialogAddSongs::on_BrowseButton_clicked()
{
    QStringList files_to_add = QFileDialog::getOpenFileNames(this, tr("Select one or more files"), QDir::root().absolutePath(), "Audio (*.mp3)");

    if(files_to_add.isEmpty()) return;

    for (QString& file_path : files_to_add) {
        Song song(file_path);
        song_vector.push_back(song);

        QTreeWidgetItem* itm = new QTreeWidgetItem(ui->treeWidget);
        itm->setText(0, song.getTitle());
        itm->setText(1, song.getInterpret());
        itm->setText(2, song.getAlbumTitle());
        itm->setText(3, QString::number(song.getYear()));
        itm->setText(4, song.getPath());

        ui->treeWidget->addTopLevelItem(itm);
    }
}


void DialogAddSongs::on_RemoveButton_clicked()
{
    int index = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
    delete ui->treeWidget->currentItem();

    song_vector.removeAt(index);
}


void DialogAddSongs::on_buttonBox_accepted()
{
    for(Song& song : song_vector) {
        Album empty_album;
        empty_album.PushSong(song);
        empty_album.setInterpret(song.getInterpret());
        empty_album.setTitle(song.getAlbumTitle());

        library->AddMedia(&empty_album);
    }
}
