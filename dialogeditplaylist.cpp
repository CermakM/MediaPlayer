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


void DialogEditPlaylist::LoadAlbums() {

    // Create top level item - album
    for (Album& temp_album : album_vector) {
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
    QTreeWidgetItem* itm = ui->treePlaylist->currentItem();
    Album _album = GetAlbumByTitle(itm->text(0));
}

Album DialogEditPlaylist::GetAlbumByTitle(const QString& _title) {

    for (Album& _album : album_vector) {
        if ( _album.GetTitle() == _title)
            return _album;
    }

    qDebug() << "Album " << _title << " not found ";
    return Album();
}

void DialogEditPlaylist::on_buttonBox_accepted()
{
    if ( new_items.empty()) return;

    QFile infile("albums.txt");
    infile.open(QFile::ReadOnly);
    QString line;
    QStringList list;
    while(!(line = infile.readLine()).isEmpty())
        list.append(line);

    infile.close();

    for (QTreeWidgetItem* itm : new_items) {
        Album _album = GetAlbumByTitle(itm->text(0));
        _album.is_in_playlist = true;

    // Search the title in list file and replace the last symbol

        for(QString& line : list) {
            if(line.contains(_album.GetTitle(), Qt::CaseInsensitive)) {
                line.replace("-", "#");
            }
        }

        for (Song& _song : _album.GetSongs())
            playlist.push_back(_song);
    }

    infile.open(QFile::WriteOnly | QFile::Truncate);

    QTextStream outStream(&infile);
    for( const QString& line : list)
        outStream << line;

    infile.close();
}
