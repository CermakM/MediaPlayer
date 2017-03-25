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

class iWidget : public QWidget
{
    Q_OBJECT

public:
    explicit iWidget(QWidget* parent = 0);
    explicit iWidget(Icon* icon, Library* const library, QWidget* parent = 0);
    ~iWidget();

    void DefaultAdjustement();

    void setLibrary(Library* const library) { _library = library; }

    inline void setTitle(QString const& new_title);

    inline QLineEdit* getTitleEditor() const { return _icon_title_editor; }

    inline QString getTitle() const { return _icon_title_editor->text(); }

    inline QString getAlbumTitle() const { return _icon->getAlbumTitle(); }

    inline Icon* getIcon() { return _icon; }

    inline Type  getType() const { return _icon->getType(); }

    inline QString getPath() const { return _icon->getPath(); }

    QSize sizeHint();

    bool operator == (const iWidget& other);

    bool operator == (Album* const album);

    void mousePressEvent(QMouseEvent* ev);

    void mouseReleaseEvent(QMouseEvent* ev);

//    void mouseDoubleClickEvent(QMouseEvent* ev);

    bool hasHeightForWidth() const;

    bool isPlaying() const { return _is_playing; }

    bool isPlaying(bool state) { emit state_changed(state); return *_is_playing = state;}

    bool isSelected() const { return _is_selected; }

    bool isSelected(bool state) { return  _is_selected = state; }

signals:

    void clicked();

    void double_clicked(QWidget*);

    void released();

    void state_changed(bool state);


private slots:

    void on_state_change(bool state) { _label_play->setVisible(state); }

private:
    Library* _library;
    QTimer* _event_timer;
    Icon* _icon;
    QLineEdit* _icon_title_editor;
    QLabel* _label_play;

    bool* _is_playing = nullptr;
    bool  _is_selected = false;
};

#endif // ICONPREVIEW_H
