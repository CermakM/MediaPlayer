#include "dialogaddalbum.h"
#include "ui_dialogaddalbum.h"

DialogAddAlbum::DialogAddAlbum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddAlbum)
{
    ui->setupUi(this);

    _library = nullptr;
    _addToPlaylist = false;
}

DialogAddAlbum::~DialogAddAlbum()
{
    delete ui;
}


DialogAddAlbum::DialogAddAlbum(Library *library, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddAlbum)
{
    ui->setupUi(this);

    _addToPlaylist = false;

    _library = library;
}

void DialogAddAlbum::on_buttonBox_accepted()
{
    QString pathToAdd = ui->PathSelect->text();

    if(pathToAdd.isEmpty()) {
        return; // no change has happened
    }

    Library::ChangeState change_state;
    Album temp_album = Album(pathToAdd);
    Album* album = _library->getAlbumByTitle(temp_album.getTitle());

    if (album != nullptr) {
        qDebug() << "Album is already in playlist";
    }
    else {
        album = new Album(temp_album);
    }
    if(_addToPlaylist) {
        for (Song& song : *(album->getSongs())) {
            song.isInPlaylist(true);
        }
    }
    if (ui->TitleSelect->isModified()) {
        QString title = ui->TitleSelect->text();
        album->setTitle(title);
    }

    change_state = _library->AddMedia(album);

    qInfo("The new album has been added");

    emit change(album, change_state);
    emit change(bool(change_state));
}

void DialogAddAlbum::on_BrowseButton_clicked()
{
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    ui->PathSelect->clear();
    ui->PathSelect->insert(dir_name);
}

void DialogAddAlbum::on_checkBoxAddToPlaylist_toggled(bool checked)
{
    _addToPlaylist = checked;
}
