#include "mainwin.h"
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    w.setWindowTitle("CemiSec");
    w.show();

    qDebug() << "The home path is set to: " << QDir::homePath() << endl;
    qDebug() << "The current path is set to: " << QDir::currentPath() << endl;
    qInfo() << "Setting the current path" << endl;

    return a.exec();
}
