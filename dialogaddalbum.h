#ifndef DIALOGADDALBUM_H
#define DIALOGADDALBUM_H

#include "album.h"
#include "library.h"

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

    DialogAddAlbum(Library* _library, QWidget *parent = 0);

private slots:
    void on_buttonBox_accepted();

    void on_BrowseButton_clicked();

    void on_checkBoxAddToPlaylist_toggled(bool checked);

signals:

    void change(Album* = nullptr, Library::ChangeState = Library::NOCHANGE);
    void change(bool);

private:
    Ui::DialogAddAlbum *ui;

    bool _addToPlaylist;

    Library* _library;
};

#endif // DIALOGADDALBUM_H
