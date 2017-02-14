#ifndef ICON_H
#define ICON_H

#include "library.h"

#include <QObject>
#include <QAction>
#include <QIcon>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class Icon : public QWidget
{
public:
    int row = 0, column = 0;

    Icon(QWidget *parent = 0);
    ~Icon();

    explicit Icon(Album* media, QWidget* parent = 0);

    explicit Icon(Song* media, QWidget *parent = 0);

    void setCheckable() { _action->isCheckable(); }

    void setIcon(QPixmap& pixmap);

    void setTitle(const QString& title) { _icon_title = title; }

    void setPath(const QString& path) {_path_to_media = path; }

    void isInPlaylist(const bool b) { _inPlaylist = b; }

    template<class T>
    void setMedia(T* media) { _media_ptr = media; }

    template<class T>
    T* getMedia() { return _media_ptr; }

    QIcon& getIcon() { return *_icon; }

    QPixmap& getPixmap() {return *_pixmap; }

    QString getTitle() const { return _icon_title; }

    QVBoxLayout* getLayout() { return _main_layout; }

    QLabel* getButton() { return _icon_label; }

    QWidget* getWidget() { return _main_widget; }

    QIcon& operator()() { return *_icon; }

    const QSize Size();

signals:

public slots:

private:

    void* _media_ptr = nullptr;

    bool _inPlaylist = false;

    QString _icon_title, _path_to_media;

    QWidget* _main_widget = nullptr;

    QPixmap* _pixmap = nullptr;

    QIcon* _icon = nullptr;

    QLabel* _icon_label = nullptr;

    QLabel* _title_label = nullptr;

    QVBoxLayout* _main_layout = nullptr;

    QHBoxLayout* _hlayout = nullptr;

    QSpacerItem* _spacer = nullptr;

    QAction* _action = nullptr;

protected:

    void CreateIconLabel();

    void CreateTitleLabel();

    void CreateLayout(QWidget* parent = 0);

    void CreateWidget();
};

#endif // ICON_H
