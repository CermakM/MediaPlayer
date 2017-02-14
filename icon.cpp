#include "icon.h"

const QSize MAXSIZE = QSize(100, 100);
const QSize MINSIZE = QSize(35, 35);
const QSize DEFSIZE = QSize(50, 50);

Icon::Icon(QWidget *parent) : QWidget(parent) {

    _pixmap = new QPixmap(":/icons/icon_empty_folder");
    _icon = new QIcon();
    _action = new QAction();

    qDebug() << "Invalid or unspecified media passed to Icon constructor";
    qDebug() << "Empty folder will be created";

    _icon_title = "Untitled";
    _path_to_media = "";

    _icon->addPixmap(*_pixmap);

    CreateIconLabel();
    CreateTitleLabel();
    CreateLayout();
    CreateWidget();

}

Icon::Icon(Album *media, QWidget *parent) : QWidget(parent)
{
    _icon = new QIcon();
    _action = new QAction();

    _media_ptr = media;

    Album* album_ptr = media;

    if(album_ptr->CurrentIcon() != nullptr) {
        QString file_path = album_ptr->getIcons()->front().absoluteFilePath();
        _pixmap = new QPixmap(file_path);
        _icon->addPixmap(*_pixmap);
    }
    else {
        _pixmap = new QPixmap(":/icons/icon_album");
        _icon->addPixmap(*_pixmap);
    }

    _icon_title = media->getTitle();
    _path_to_media = media->getPath();

    CreateIconLabel();
    CreateTitleLabel();
    CreateLayout();
    CreateWidget();

}

Icon::Icon(Song *media, QWidget *parent) : QWidget(parent){

    _pixmap = new QPixmap(":/icons/icon_song");
    _icon = new QIcon();
    _action = new QAction();

    _media_ptr = media;

    Song* song_ptr = media;

    _icon->addPixmap(*_pixmap);

    _icon_title = song_ptr->getTitle();
    _path_to_media = song_ptr->getPath();

    CreateIconLabel();
    CreateTitleLabel();
    CreateLayout();
    CreateWidget();

}


Icon::~Icon() {

    delete _icon;
    delete _pixmap;
    delete _action;
    delete _spacer;
    delete _hlayout;
    delete _main_widget;
    delete _main_layout;
}


void Icon::CreateIconLabel()
{
    _icon_label = new QLabel();
    _icon_label->setPixmap(*_pixmap);
    _icon_label->setScaledContents(true);
}

void Icon::CreateTitleLabel()
{
    _title_label = new QLabel();
    _title_label->setWordWrap(true);
    _title_label->setText("&" + _icon_title.toUpper());
    _title_label->setAlignment(Qt::AlignHCenter);
    _title_label->setMaximumWidth(MAXSIZE.width());
    _title_label->setMaximumHeight(25);
    _title_label->setBuddy(_icon_label);
}

void Icon::CreateLayout(QWidget* parent)
{
    _main_layout = new QVBoxLayout(parent);

    int space_width = (MAXSIZE.width() - DEFSIZE.width()) / 2;
    _spacer = new QSpacerItem (space_width, 0);

    _hlayout = new QHBoxLayout();
    _hlayout->insertSpacerItem(0, _spacer);
    _hlayout->addWidget(_icon_label);
    _hlayout->insertSpacerItem(-1, _spacer);

    _main_layout->addLayout(_hlayout);
    _main_layout->addWidget(_title_label);
    _main_layout->setMargin(0);
}

void Icon::CreateWidget()
{
    _main_widget = new QWidget(this);

    _main_widget->setLayout(_main_layout);
    _main_widget->setMaximumSize(MAXSIZE);
}

const QSize Icon::Size()
{
    if (_main_layout == nullptr) {
        qDebug()<< "Layout has not been created yet... returning size of icon";
        return _icon_label->size();
    }
    return _main_layout->sizeHint();

}

void Icon::setIcon(QPixmap &pixmap)
{
    _icon->addPixmap(pixmap);
    _action->setIcon(*_icon);
}

