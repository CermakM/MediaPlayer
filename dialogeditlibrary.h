#ifndef DIALOGEDITLIBRARY_H
#define DIALOGEDITLIBRARY_H

#include "album.h"
#include "song.h"
#include "library.h"
#include "playlist.h"
#include "dialogaddalbum.h"
#include "dialogaddsongs.h"

#include <QDialog>
#include <QVector>
#include <QTreeWidget>
#include <QFileDialog>
#include <QRegularExpression>

namespace Ui {
class DialogEditLibrary;
}

class DialogEditLibrary : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditLibrary(Library* library, QWidget* parent = 0);
    ~DialogEditLibrary();

    void LoadLibrary();

    void CreateTree(QVector<Album>* const album_vec);

    void CreateTreeItem(Album &album, QTreeWidgetItem * const tree_album_item = nullptr);

private slots:
    void on_AddSongsButton_clicked();

    void on_RemoveButton_clicked();

    void on_buttonBox_accepted();

    void on_AddAlbumsButton_clicked();

    void Update(bool changed);

private:
    Ui::DialogEditLibrary *ui;

    Library* _library;
    Library* _pseudo_library;

    QVector<QTreeWidgetItem*> _new_media;
    QVector<QTreeWidgetItem*> _items_to_remove;
};

#endif // DIALOGEDITLIBRARY_H
