#ifndef ICONPREVIEW_H
#define ICONPREVIEW_H

#include "icon.h"
#include "library.h"

#include <QWidget>
#include <QBoxLayout>
#include <QObject>
#include <QLabel>
#include <QLineEdit>
#include <QTimer>

class MainWin;

class iWidget : public QWidget
{
    Q_OBJECT

public:
    explicit iWidget(QWidget* parent = 0);
    explicit iWidget(Icon* icon, Library* const library, QWidget* parent = 0);
    ~iWidget();

    void DefaultAdjustement();

    void setLibrary(Library* const library) { _library = library; }

    void pushChild(iWidget* const child);

    void removeChild(iWidget* const child);

    iWidget* getChild(iWidget* const child);

    QVector<iWidget*> * getChildWidgets();

    void setAlbumWidget(iWidget* album_widget) { _album_widget = album_widget; }

    inline void setTitle(QString const& new_title);

    inline iWidget* getAlbumWidget() const { return _album_widget; }

    inline QLineEdit* getTitleEditor() const { return _icon_title_editor; }

    inline QString getTitle() const { return _icon_title_editor->text(); }

    inline QString getAlbumTitle() const { return _icon->getAlbumTitle(); }

    inline QString getInterpret() const { return _icon->getInterpret(); }

    inline Icon* getIcon() { return _icon; }

    inline Type  getType() const { return _icon->getType(); }

    inline QString getPath() const { return _icon->getPath(); }

    QSize sizeHint();

    bool operator == (const iWidget& other);

    bool operator == (Album* const album);

    void mousePressEvent(QMouseEvent* ev);

    bool hasHeightForWidth() const;

    bool isPlaying() const { return *_is_playing; }

    bool isPlaying(bool state) { emit state_changed(state); return *_is_playing = state;}

    bool isSelected() const { return _is_selected; }

    bool isSelected(bool state) { return  _is_selected = state; }

signals:

    void clicked();

    void right_clicked(iWidget*);

    void double_clicked(QWidget*);

    void state_changed(bool state);

private slots:

    void on_state_change(bool state) { _label_play->setVisible(state); }

private:
    QVector<iWidget*> _children;
    iWidget* _album_widget;
    Library* _library;
    QTimer*  _event_timer;
    Icon*    _icon;
    QLabel*  _label_play;
    QPixmap* _label_play_pixmap;
    QLineEdit* _icon_title_editor;

    bool* _is_playing = nullptr;
    bool* _is_in_playlist = nullptr;
    bool  _is_selected = false;
};

#endif // ICONPREVIEW_H
