#include "iwidget.h"

iWidget::iWidget(QWidget* parent) :
    QWidget(parent)
{
    _icon = new Icon();
    _library = nullptr;
    _album_widget = nullptr;

    SetProperties();
    DefaultAdjustement();
}

iWidget::iWidget(Icon *icon, Library* const library, QWidget *parent) :
    QWidget(parent)
{
    _icon = icon;
    _library = library;
    _album_widget = nullptr;

    SetProperties();
    DefaultAdjustement();
}

iWidget::~iWidget()
{
    if (*(this->_is_in_playlist)) {
        qDebug() << "I was in playlist: " << this->getTitle();
        _library->playlist_updated = true;
    }

    while(!_children.empty()) {
        delete _children.takeFirst();
    }

    if (Type::T_ALBUM == this->getType()) {
        delete _is_playing;
        delete _is_in_playlist;
    }

    delete _label_play_pixmap;
    delete _label_play;
    delete _icon;
    delete _icon_title_editor;
    delete _event_click_timer;
}

void iWidget::setTitle(const QString &new_title)
{
    _icon->setTitle(new_title);
    _icon_title_editor->setText(_icon->getTitle());
}

void iWidget::SetProperties()
{
    _label_play = new QLabel(_icon);
    _label_play_pixmap = new QPixmap(":/icons/icon_play");
    _label_play->setPixmap(_label_play_pixmap->scaled(32,32, Qt::KeepAspectRatio));
    _label_play->setFixedSize(32,32);
    _label_play->raise();

    _event_click_timer = new QTimer(this);
    _event_click_timer->setSingleShot(true);
    _event_click_timer->setInterval(200);

    if (_icon->getType() == Type::T_SONG) {
        Song* song = _library->getSongByTitle(_icon->getTitle());
        _is_playing = song->isPlaying();
        _is_in_playlist = song->isInPlaylist();
    }

    else {
        _is_playing = new bool(false);
        _is_in_playlist = new bool(false);
    }

    _icon_title_editor = new ScrollText(this);
    _icon_title_editor->setText(_icon->getTitle());
    _icon->setBuddy(_icon_title_editor);

    this->setLayout(new QGridLayout(this));
    connect(this, SIGNAL(state_changed(bool)), this, SLOT(on_state_change(bool)));

    this->layout()->addWidget(_icon);
    this->layout()->addWidget(_icon_title_editor);
    this->setBaseSize(_icon->size() + QSize(_icon->size().width(), _icon->size().height()));
    this->setFixedSize(this->baseSize());
    this->setContentsMargins(4,4,4,4);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    DefaultAdjustement();
}

void iWidget::DefaultAdjustement() {

    _label_play->setVisible(*_is_playing);

    _default_graphic_effect = new QGraphicsDropShadowEffect(_icon);
    _icon->setGraphicsEffect(nullptr);
}

void iWidget::pushChild(iWidget * const child)
{
    _children.push_back(child);
    child->setAlbumWidget(this);
}

void iWidget::removeChild(iWidget * const child)
{
    _children.removeAt(_children.indexOf(child));
}

iWidget *iWidget::getChild(iWidget * const child)
{
    return _children.at(_children.indexOf(child));
}

QVector<iWidget *> * iWidget::getChildWidgets()
{
    return &_children;
}

QSize iWidget::sizeHint()
{
    return _icon->isNull() ? QWidget::sizeHint() : this->baseSize();
}

bool iWidget::operator == (const iWidget &other)
{
    return this->_icon_title_editor->text() == other.getTitle() &&
            this->getType() == other.getType();
}

bool iWidget::operator ==(Album * const album)
{
    return this->_icon_title_editor->text() == album->getTitle();
}

void iWidget::mousePressEvent(QMouseEvent *ev)
{
    (void) ev;
    if (ev->button() == Qt::RightButton) {
        emit right_clicked(this);
        return;
    }
    if (!_event_click_timer->isActive()) {
        // Filter right click - MainWin will take care of that
        emit clicked();
        _event_click_timer->start();
    }
    else emit double_clicked(this);
}

bool iWidget::hasHeightForWidth() const
{
    return true;
}

bool iWidget::isSelected(bool state)
{
    if (state) {
        _graphic_effect = new QGraphicsColorizeEffect(_icon);
        _graphic_effect->setColor(QColor(168, 0, 0, 255));
        _icon->setGraphicsEffect(_graphic_effect);
    }
    else DefaultAdjustement();

    _icon_title_editor->setTextFlow(state);
    return _is_selected = state;
}
