#include "dialogaddalbum.h"
#include "ui_dialogaddalbum.h"

DialogAddAlbum::DialogAddAlbum(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAddAlbum)
{
    ui->setupUi(this);

    addToPlaylist = false;
}

DialogAddAlbum::~DialogAddAlbum()
{
    delete ui;
}

void DialogAddAlbum::on_buttonBox_accepted()
{
    QFile albums("albums.txt");
    if (!albums.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Append)){
        qInfo("Album has not been succesfully opened for writing");
        qDebug() << albums.errorString();
        return;
    }

    QString pathToAdd = ui->PathSelect->text();
    Album myAlbum (pathToAdd);
    QString title = myAlbum.GetTitle();
    QString interpret = myAlbum.GetInterpret();
    QString year = myAlbum.GetYear();

    if (ui->TitleSelect->isModified()) title = ui->TitleSelect->text();

    QString saved_path;
    QTextStream inStream(&albums);
    while(!inStream.atEnd()) {
        saved_path = inStream.readLine();
        if (saved_path == pathToAdd)
            return;
    }

    inStream << pathToAdd << "|" << title << "|" << interpret << "|" << year;
    if ( addToPlaylist ) {
        inStream << "|" << "#";
    }

    inStream << endl;

    qInfo("The new album has been added");

    albums.close();
}

void DialogAddAlbum::on_BrowseButton_clicked()
{
    QString dir_name = QFileDialog::getExistingDirectory(this, tr("Open Directory"));
    ui->PathSelect->clear();
    ui->PathSelect->insert(dir_name);
}

void DialogAddAlbum::on_checkBoxAddToPlaylist_toggled(bool checked)
{
   ui->checkBoxAddToPlaylist->isChecked() ? addToPlaylist = true : addToPlaylist = false;
}
