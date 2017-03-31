/**
  * User friendly media player program
  * main.cpp
  *
  * @author Marek Cermak
  * @version 1.0b 31.3.2017
*/

#include "mainwin.h"
#include <QApplication>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    w.setWindowTitle("Musician");
    w.show();

    return a.exec();
}
