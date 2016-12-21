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

void DialogEditPlaylist::on_AddToPlaylistButton_clicked()
{

}

void DialogEditPlaylist::on_RemoveFromPlaylistButton_clicked()
{

}
