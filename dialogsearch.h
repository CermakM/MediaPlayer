#ifndef DIALOGSEARCH_H
#define DIALOGSEARCH_H

#include "iwidget.h"

#include <QDialog>
#include <QVector>
#include <QRegularExpression>
#include <QMap>
#include <QTreeWidget>

namespace Ui {
class DialogSearch;
}

class DialogSearch : public QDialog
{
    Q_OBJECT

public:
    explicit DialogSearch(const QVector<iWidget*> & widgets, QWidget *parent = 0);
    ~DialogSearch();

    void setSearchContainer(const QVector<iWidget*> & widgets) { _widgets = widgets; }

protected:
    bool MatchBeginning(QString const& s1, QString const& s2);
    void CreateTreeItem(iWidget *widget);

signals:
    void selected(iWidget*);

private slots:
    void on_TitleInput_textChanged(const QString &arg1);

    void on_checkSongs_toggled(bool checked);

    void on_checkAlbums_toggled(bool checked);

//    void on_selection(QWidget*);

    void on_buttonBox_accepted();

private:
    Ui::DialogSearch *ui;

    QVector<iWidget*> _widgets;
    QMap<QTreeWidgetItem*, iWidget*> _map;

    QString _cache_search;
    bool _albums_enabled = true;
    bool _songs_enabled = true;
};

#endif // DIALOGSEARCH_H
