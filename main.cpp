/**
  * User friendly media player program
  * main.cpp
  *
  * @author Marek Cermak
  * @version 1.0b 31.3.2017
*/

#include "mainwin.h"
#include <QApplication>
#include <QScreen>
#include <QLabel>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWin w;
    QScreen* screen = a.primaryScreen();
    QRect screen_rect = screen->availableGeometry();

    w.move((screen_rect.width() - w.width()) / 2, (screen_rect.height() - w.height()) / 2);
    w.setWindowTitle("Musician");
    w.show();

    return a.exec();
}
