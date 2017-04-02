#include "dialogshortcutcs.h"
#include "ui_dialogshortcutcs.h"

DialogShortcutcs::DialogShortcutcs(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogShortcutcs)
{
    ui->setupUi(this);
}

DialogShortcutcs::~DialogShortcutcs()
{
    delete ui;
}

void DialogShortcutcs::on_ButtonClose_clicked()
{
    this->close();
}
