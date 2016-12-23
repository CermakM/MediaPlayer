#include "mainwin.h"
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    w.setWindowTitle("CemiSec");
    w.show();

    return a.exec();
}
