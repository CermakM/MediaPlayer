#include "icon.h"

Icon::Icon(QWidget* parent) :
    QLabel(parent)
{
    _pixmap = new QPixmap(":/icons/icon_empty_folder");
    _type = T_NOTYPE;

    qDebug() << "Invalid or unspecified media passed to Icon constructor";
    qDebug() << "Empty folder will be created";

    _path_to_media = "Unspecified";
    _title = "Unknown";

    CreateLabel(_pixmap);
}

Icon::Icon (Album *media, QWidget *parent) :
    QLabel(parent)
{
    _title = media->getTitle();
    _type = T_ALBUM;
    Album* album_ptr = media;

    if(album_ptr->CurrentIcon()) {
        QString file_path = album_ptr->getIcons()->front().absoluteFilePath();
        _pixmap = new QPixmap(file_path);
    }
    else {
        _pixmap = new QPixmap(":/icons/icon_album");
    }

    _path_to_media = album_ptr->getPath();

    CreateLabel(_pixmap);
}

Icon::Icon(Song *media, QWidget *parent) :
    QLabel(parent)
{
    _title = media->getTitle();
    _type = T_SONG;
    Song* song_ptr = media;

    _pixmap = new QPixmap(":/icons/icon_song");

    _in_playlist = song_ptr->is_in_playlist;

    _path_to_media = song_ptr->getPath();

    CreateLabel(_pixmap);
}


Icon::~Icon() {

    delete _pixmap;
}


bool Icon::CreateLabel(QPixmap* pixmap) {
    if (!pixmap) {
        qDebug() << "Error creating QLabel: QPixmap = nullptr\n";
        return false;
    }

    this->setPixmap(pixmap->scaled(_size, Qt::KeepAspectRatio));
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    this->setMinimumSize(_size);
    this->setBaseSize(64,64);

    return true;
}

void Icon::setTitle(const QString &new_title)
{
    // Temporary solution
    _title = new_title;
}

void Icon::Update() {

    this->setPixmap(_pixmap->scaled(_size, Qt::KeepAspectRatio));
}

void Icon::on_click()
{

}
