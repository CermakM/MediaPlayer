#ifndef DIALOGEDITPLAYLIST_H
#define DIALOGEDITPLAYLIST_H

#include <QDialog>

namespace Ui {
class DialogEditPlaylist;
}

class DialogEditPlaylist : public QDialog
{
    Q_OBJECT

public:
    explicit DialogEditPlaylist(QWidget *parent = 0);
    ~DialogEditPlaylist();

private slots:
    void on_AddToPlaylistButton_clicked();

    void on_RemoveFromPlaylistButton_clicked();

private:
    Ui::DialogEditPlaylist *ui;
};

#endif // DIALOGEDITPLAYLIST_H
