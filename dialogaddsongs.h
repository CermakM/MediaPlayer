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
    explicit DialogAddSongs(Library* library, QWidget *parent = 0);
    ~DialogAddSongs();

signals:
    void change(Album*, Library::ChangeState);
    void change(bool);

private slots:
    void on_BrowseButton_clicked();

    void on_RemoveButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::DialogAddSongs *ui;

    Library* _library;

    QVector<Song*> _song_vector;
};

#endif // DIALOGADDSONGS_H
