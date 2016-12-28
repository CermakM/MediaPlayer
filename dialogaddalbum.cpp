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
    QString pathToAdd = ui->PathSelect->text();

    if(pathToAdd.isEmpty()) {
        return; // no change has happened
    }

    QFile albums("albums.txt");
    if (!albums.open(QIODevice::ReadOnly | QIODevice::Text)){
        qInfo("Album has not been succesfully opened for writing");
        qDebug() << albums.errorString();
        return;
    }

    Album myAlbum (pathToAdd);
    QString title = myAlbum.GetTitle();
    QString interpret = myAlbum.GetInterpret();
    QString year = myAlbum.GetYear();

    if (ui->TitleSelect->isModified()) title = ui->TitleSelect->text();

    QString saved_path;
    QTextStream inStream(&albums);
    while(!inStream.atEnd()) {
        saved_path = inStream.readLine();
        if (saved_path.contains(pathToAdd))
            QMessageBox::warning(this, tr("Duplicated path"), tr("The album you\'ve selected is already in your list"), QMessageBox::Ok);
            return;
    }

    albums.close();

    if (!albums.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        qInfo("Album has not been succesfully opened for writing");
        qDebug() << albums.errorString();
        return;
    }

    inStream << pathToAdd << "|" << title << "|" << interpret << "|" << year;
    if ( addToPlaylist ) {
        inStream << "|" << "#";
    }
    else inStream << "|" << "-";

    inStream << endl;

    qInfo("The new album has been added");

    albums.close();

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
   ui->checkBoxAddToPlaylist->isChecked() ? addToPlaylist = true : addToPlaylist = false;
}
