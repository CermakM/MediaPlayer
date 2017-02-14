#ifndef DIALOGEDITLIBRARY_H
#define DIALOGEDITLIBRARY_H

#include "album.h"
#include "song.h"
#include "library.h"
#include "playlist.h"

#include <QDialog>
#include <QVector>
#include <QTreeWidget>
#include <QFile>

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

private slots:
    void on_AddButton_clicked();

    void on_RemoveButton_clicked();

    void on_buttonBox_accepted();

private:
    Ui::DialogEditLibrary *ui;

    Library* _library;

    QVector<QTreeWidgetItem*> _new_media;
    QVector<QTreeWidgetItem*> _items_to_remove;
};

#endif // DIALOGEDITLIBRARY_H
