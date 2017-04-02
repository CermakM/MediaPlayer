#include "dialogsearch.h"
#include "ui_dialogsearch.h"

DialogSearch::DialogSearch(const QVector<iWidget *> &widgets, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSearch),
    _widgets(widgets)
{
    ui->setupUi(this);

    ui->treeWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(on_buttonBox_accepted()));

    // Initialize all with empty string - matches all
    on_TitleInput_textChanged("");
}

DialogSearch::~DialogSearch()
{
    delete ui;
}

bool DialogSearch::MatchBeginning(const QString &s1, const QString &s2)
{
    QRegularExpression regex("^" + s2 + ".*", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = regex.match(s1);
    if (match.hasMatch())
        return true;

    return false;
}

void DialogSearch::CreateTreeItem(iWidget* widget) {

    QTreeWidgetItem* widget_item;

    widget_item = new QTreeWidgetItem(ui->treeWidget);
    widget_item->setText(0, widget->getTitle());
    if (Type::T_ALBUM ==  widget->getType()) {
        widget_item->setText(1, "");
        widget_item->setText(2, "Album");
    }
    else {
        widget_item->setText(1, widget->getAlbumTitle());
        widget_item->setText(2, "Song");
    }

    if (widget->isPlaying())
        widget_item->setText(3, "Yes");
    else
        widget_item->setText(3, "No");

    // Connect the item and the widget for faster search
    _map[widget_item] = widget;

    // Add the top level item to the tree
    ui->treeWidget->addTopLevelItem(widget_item);
}

void DialogSearch::on_TitleInput_textChanged(const QString &arg1)
{
    ui->treeWidget->clear();
    _map.clear();

    for (iWidget* const parent_widget : _widgets) {
        if ( MatchBeginning(parent_widget->getTitle(), arg1)) {
            if ( ((T_ALBUM == parent_widget->getType()) && _albums_enabled ) ||
                 ((T_SONG  == parent_widget->getType()) && _songs_enabled  )
               )
                CreateTreeItem(parent_widget);
        }
        if (!parent_widget->getChildWidgets()->empty() && _songs_enabled) {
            for (iWidget* const child_widget : *parent_widget->getChildWidgets()) {
                if ( MatchBeginning(child_widget->getTitle(), arg1)) {
                    CreateTreeItem(child_widget);
                }
            }
        }
    }

    _cache_search = arg1;
}

void DialogSearch::on_checkSongs_toggled(bool checked)
{
    _songs_enabled = checked;
    on_TitleInput_textChanged(_cache_search);
}

void DialogSearch::on_checkAlbums_toggled(bool checked)
{
    _albums_enabled = checked;
    on_TitleInput_textChanged(_cache_search);
}

void DialogSearch::on_buttonBox_accepted()
{
    iWidget* widget = _map[ui->treeWidget->selectedItems().first()];

    emit selected(widget);
}
