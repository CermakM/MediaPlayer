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

//#include <filesystem>

namespace Ui {
class DialogEditLibrary;
}

class DialogEditLibrary : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditLibrary(Library* const library, QWidget* parent = 0);
    ~DialogEditLibrary();

    void LoadLibrary();

    void CreateTree(QVector<Album>* const album_vec);

    void CreateTreeItem(Album &album, QTreeWidgetItem * const tree_album_item = nullptr);

signals:
    void UpdatePlaylist(bool b);

private slots:
    void on_AddSongsButton_clicked();

    void on_RemoveButton_clicked();

    void on_buttonBox_accepted();

    void on_AddAlbumsButton_clicked();

    void UpdateTree(bool changed);

    void on_buttonBox_rejected();

private:
    Ui::DialogEditLibrary *ui;

    Library* _library;
    Library* _pseudo_library;

    bool _change = false;
};

#endif // DIALOGEDITLIBRARY_H
