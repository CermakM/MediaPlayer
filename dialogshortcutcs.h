#ifndef DIALOGSHORTCUTCS_H
#define DIALOGSHORTCUTCS_H

#include <QDialog>

namespace Ui {
class DialogShortcutcs;
}

class DialogShortcutcs : public QDialog
{
    Q_OBJECT

public:
    explicit DialogShortcutcs(QWidget *parent = 0);
    ~DialogShortcutcs();

private slots:
    void on_ButtonClose_clicked();

private:
    Ui::DialogShortcutcs *ui;
};

#endif // DIALOGSHORTCUTCS_H
