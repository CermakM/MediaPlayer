#ifndef DIALOGADDSONGS_H
#define DIALOGADDSONGS_H

#include "library.h"

#include <QDialog>
#include <QFileDialog>

namespace Ui {
class DialogAddSongs;
}

class DialogAddSongs : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAddSongs(Library* _library, QWidget *parent = 0);
    ~DialogAddSongs();

private slots:
    void on_BrowseButton_clicked();

    void on_RemoveButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::DialogAddSongs *ui;

    Library* library;


    QVector<Song> song_vector;
};

#endif // DIALOGADDSONGS_H
