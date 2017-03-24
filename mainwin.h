#ifndef MAINWIN_H
#define MAINWIN_H

#include "dialogaddalbum.h"
#include "dialogeditplaylist.h"
#include "dialogaddsongs.h"
#include "dialogeditlibrary.h"
#include "playlist.h"
#include "library.h"
#include "iwidget.h"
#include "flowlayout.h"

#include <QDir>
#include <QFile>
#include <QMenu>
#include <QWidgetAction>
#include <QTextStream>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QDebug>
#include <QMessageBox>
#include <QShortcut>
#include <QBoxLayout>
#include <QSignalMapper>
#include <QVector>
#include <QScrollArea>


#include <vector>
#include <fstream>

const int MAX_WIDGET_SIZE = 128;

namespace Ui {
class MainWin;
}

class MainWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWin(QWidget *parent = 0);
    ~MainWin();

protected:

    void UpdatePlaylist();

    void CreateDropArea();

    void CreateAlbumContentArea(Album* const target_album, QWidget* drop_area_container);

    void CreateWidget(void* const media, Type type);

    void CreateNewRow(QBoxLayout *drop_layout, QBoxLayout **drop_row);

    void ConnectSignals();

private slots:

    void on_VolumeSlider_valueChanged(int value);

    void on_ProgressSlider_sliderMoved(int position);

    void on_PositionChange( qint64 position );

    void on_DurationChange( qint64 position );

    void on_ButtonPlay_clicked();

    void on_ButtonStop_clicked();

    void on_actionAddNewAlbum_triggered();

    void on_actionEditPlaylist_triggered();

    void on_EndOfSong();

    void on_EditPlaylistOver(bool b);

    void on_actionAddNewSongs_triggered();

    void on_ButtonForward_clicked();

    void on_ButtonBackward_clicked();

    void on_actionEditLibrary_triggered();

    void on_Icon_deselect();

    void on_Icon_click(QWidget* target);

    void on_Icon_doubleClick(QWidget* target);

    void on_ButtonDeselect_clicked();

    void on_ButtonRemove_clicked();

    void on_ButtonHome_clicked();

private:
    Ui::MainWin *ui;

    QMediaPlayer* _media_player;

    Playlist* _playlist;

    Library _library;

    QVector<iWidget*> _icon_widgets;

    QVector<iWidget*> _selected_icons;

    QSignalMapper* _icon_signal_mapper;

    QWidget* _cache_dropAreaContent = nullptr;

    QVector<iWidget*> _temporary_icon_widgets;

    QSignalMapper* _temporary_signal_mapper;

    bool _temporary_window_entered = false;
};

#endif // MAINWIN_H
