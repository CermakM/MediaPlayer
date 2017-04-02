#-------------------------------------------------
#
# Project created by QtCreator 2017-03-15T23:16:01
#
#-------------------------------------------------

QT       += core gui \
            multimedia \
            sql \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Musician
TEMPLATE = app

CONFIG += c++17

LIBS += -ltag

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    album.cpp \
    dialogaddalbum.cpp \
    dialogaddsongs.cpp \
    dialogeditlibrary.cpp \
    dialogeditplaylist.cpp \
    dragarea.cpp \
    icon.cpp \
    iwidget.cpp \
    library.cpp \
    main.cpp \
    mainwin.cpp \
    playlist.cpp \
    song.cpp \
    flowlayout.cpp \
    iwidgetmenu.cpp \
    scrolltext.cpp \
    dialogabout.cpp \
    customactionrecent.cpp \
    dialogshortcutcs.cpp

HEADERS  += \
    album.h \
    dialogaddalbum.h \
    dialogaddsongs.h \
    dialogeditlibrary.h \
    dialogeditplaylist.h \
    dragarea.h \
    icon.h \
    iwidget.h \
    library.h \
    mainwin.h \
    playlist.h \
    probe.h \
    song.h \
    flowlayout.h \
    iwidgetmenu.h \
    scrolltext.h \
    dialogabout.h \
    customactionrecent.h \
    dialogshortcutcs.h

RESOURCES += \
    resource.qrc

FORMS += \
    dialogaddalbum.ui \
    dialogaddsongs.ui \
    dialogeditlibrary.ui \
    dialogeditplaylist.ui \
    mainwin.ui \
    dialogabout.ui \
    dialogshortcutcs.ui

DISTFILES += \
    Musician.pro.user \
    developernotes.txt \
    README.md \
    Media/media.sqlite
