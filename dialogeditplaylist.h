#ifndef DIALOGEDITPLAYLIST_H
#define DIALOGEDITPLAYLIST_H

#include "album.h"
#include "song.h"
#include "library.h"
#include "playlist.h"

#include <QDialog>
#include <QTreeWidget>
#include <QVector>
#include <QFile>
#include <QTranslator>

namespace Ui {
class DialogEditPlaylist;
}

class DialogEditPlaylist : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditPlaylist(QWidget *parent = 0);
    ~DialogEditPlaylist();

    DialogEditPlaylist(Library *library, QWidget* parent = 0);

    void LoadLibrary();


protected slots:
    void on_AddButton_clicked();

    void on_RemoveButton_clicked();

    void on_buttonBox_accepted();

signals:

    void Change(bool);


private:
    Ui::DialogEditPlaylist *ui;

    Library* _library;

    QVector<QTreeWidgetItem*> _new_media;
    QVector<QTreeWidgetItem*> _items_to_remove;

};

#endif // DIALOGEDITPLAYLIST_H
