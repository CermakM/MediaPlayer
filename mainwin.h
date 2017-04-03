#ifndef MAINWIN_H
#define MAINWIN_H

#include "dialogaddalbum.h"
#include "dialogeditplaylist.h"
#include "dialogaddsongs.h"
#include "dialogeditlibrary.h"
#include "dialogabout.h"
#include "dialogshortcutcs.h"
#include "dialogsearch.h"

#include "playlist.h"
#include "library.h"
#include "iwidget.h"
#include "dragarea.h"
#include "flowlayout.h"
#include "iwidgetmenu.h"
#include "customactionrecent.h"

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

    void CreateGraphics();
    void CreateDropArea();
    iWidget *CreateWidget(void* const media, Type type, int index = -1);
    void CreateWidgetMenu(iWidget * const target);
    void CreateAlbumContentArea(iWidget * const target_widget, DragArea* drop_area_container);

    void ShowAlbums(bool on);
    void ShowSongs(bool on);

    void ConnectSignals();
    void UpdatePlaylist();

    void AddRecentSong(iWidget * const target);

    void resizeEvent(QResizeEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:

    void on_VolumeSlider_valueChanged(int value);
    void on_VolumeSlider_mute();
    void on_ProgressSlider_sliderMoved(int position);
    void on_ProgressSlider_FastForward();
    void on_ProgressSlider_FastBackward();
    void on_PositionChange( qint64 position );
    void on_DurationChange( qint64 position );

    void on_ButtonPlay_clicked();
    void on_ButtonStop_clicked();
    void on_ButtonForward_clicked();
    void on_ButtonBackward_clicked();
    void on_ButtonDeselect_clicked();
    void on_ButtonRemove_clicked();
    void on_ButtonHome_clicked();
    void on_pushButton_clicked();
    void on_ButtonRefresh_clicked();

    void on_actionAddNewAlbum_triggered();
    void on_actionAddNewSongs_triggered();
    void on_actionEditPlaylist_triggered();
    void on_actionEditLibrary_triggered();

    void on_EditPlaylistOver(Album*, Library::ChangeState);
    void on_EditPlaylistOver(bool);

    void on_Media_drop(const QMimeData*);
    void on_Media_change(QMediaPlayer::MediaStatus state);
    void on_Library_change(Album* album, Library::ChangeState state);

    void on_Icon_deselect();
    void on_Icon_removeSelected();
    void on_Icon_AddToPlaylist();
    void on_Icon_RemoveFromPlaylist();
    void on_Icon_click(QWidget* target);
    void on_Icon_rightClick(iWidget *target);
    void on_Icon_doubleClick(QWidget* target);
    /*
     * Reimplemented for signal receiving
     */
    void on_Icon_doubleClick();    
    void on_Icon_Properties();
    void on_Icon_Play();
    void on_Icon_rectangularSelection(QRect&);
    void on_Icon_search(iWidget*);

    /*
     * This function only process right-button clicks
     * Left-button clicks are processed by widgets itselves
     */
    void on_DragArea_pressEvent(QMouseEvent *event);

    void on_actionAbout_triggered();
    void on_actionShortcuts_triggered();
    void on_actionRecent_triggered(iWidget*);
    void on_actionShowAlbums_triggered(bool checked);
    void on_actionShowSongs_triggered(bool checked);
    void on_actionShowAll_triggered(bool checked);

    /*
     *  This function controls the status tip - if all widgets are hidden
     */
    void on_actionShowAll_change();
    void on_actionSearch_triggered();
    void on_actionRandomSong_triggered();

private:
    Ui::MainWin *ui;

    // Media
    QMediaPlayer* _media_player;
    Playlist* _playlist;
    Library   _library;

    // Library Icons
    uint _cache_volume;

    QVector<iWidget*> _icon_widgets;
    QVector<iWidget*> _selected_icons;
    QSignalMapper* _icon_signal_mapper;

    Song*    _current_song = nullptr;
    iWidget* _current_song_widget   = nullptr;
    iWidget* _current_album_widget  = nullptr;
    QWidget* _cache_dropAreaContent = nullptr;
    QSignalMapper* _temporary_signal_mapper = nullptr;

    bool _temporary_window_entered  = false;

    // Actions
    // There will be 3 songs stored in recents
    const ushort DEFAULT_RECENT_COUNT = 3;
    CustomActionRecent* _default_action;
    QList<CustomActionRecent*>  _recent_actions;

    // Rectangular painter features
    QRect   _rect_selection;

    // Graphics
    QPixmap _background;
    QPixmap _buffer;
    QBrush _brush;
    QPen   _pen;
};

#endif // MAINWIN_H
