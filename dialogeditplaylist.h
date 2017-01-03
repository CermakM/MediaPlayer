#ifndef DIALOGEDITPLAYLIST_H
#define DIALOGEDITPLAYLIST_H

#include "album.h"
#include "song.h"
#include "playlist.h"
#include "library.h"

#include <QDialog>
#include <QTreeWidget>
#include <QMediaPlaylist>
#include <QVector>
#include <QFile>
#include <QTranslator>

#include <vector>


namespace Ui {
class DialogEditPlaylist;
}

class DialogEditPlaylist : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditPlaylist(QWidget *parent = 0);
    ~DialogEditPlaylist();

    DialogEditPlaylist(Library *_library, QWidget* parent = 0);

    void LoadLibrary();


private slots:
    void on_AddToPlaylistButton_clicked();

    void on_RemoveFromPlaylistButton_clicked();

    void on_buttonBox_accepted();

signals:

    void Change(bool);


private:
    Ui::DialogEditPlaylist *ui;

    Library* library;

    QVector<QTreeWidgetItem*> new_media;
    QVector<QTreeWidgetItem*> items_to_remove;

};

#endif // DIALOGEDITPLAYLIST_H
