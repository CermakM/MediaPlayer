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

    Album newAlbum (pathToAdd);

    if(_addToPlaylist) {
        for (Song& song : *(newAlbum.getSongs())) {
            song.is_in_playlist = true;
        }
    }
    if (ui->TitleSelect->isModified()) {
        QString title = ui->TitleSelect->text();
        newAlbum.setTitle(title);
    }

    _library->AddMedia(&newAlbum);

    qInfo("The new album has been added");

    emit Change(true);
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
    // ui->checkBoxAddToPlaylist->isChecked() ? addToPlaylist = true : addToPlaylist = false;
}
