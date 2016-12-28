#ifndef DIALOGADDALBUM_H
#define DIALOGADDALBUM_H

#include "album.h"

#include <QDebug>
#include <QMessageBox>
#include <QDialog>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>


namespace Ui {
class DialogAddAlbum;
}

class DialogAddAlbum : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddAlbum(QWidget *parent = 0);
    ~DialogAddAlbum();


private slots:
    void on_buttonBox_accepted();

    void on_BrowseButton_clicked();

    void on_checkBoxAddToPlaylist_toggled(bool checked);

signals:
    void Change(bool);


private:
    Ui::DialogAddAlbum *ui;

    bool addToPlaylist;
};

#endif // DIALOGADDALBUM_H
