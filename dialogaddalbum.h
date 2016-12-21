#ifndef DIALOGADDALBUM_H
#define DIALOGADDALBUM_H

#include <QDebug>
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

private:
    Ui::DialogAddAlbum *ui;
};

#endif // DIALOGADDALBUM_H
