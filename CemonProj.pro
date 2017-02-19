#-------------------------------------------------
#
# Project created by QtCreator 2016-12-18T17:16:15
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

SOURCES += main.cpp\
    mainwin.cpp \
    album.cpp \
    song.cpp \
    dialogaddalbum.cpp \
    dialogeditplaylist.cpp \
    playlist.cpp \
    library.cpp \
    dialogaddsongs.cpp \
    icon.cpp \
    dialogeditlibrary.cpp \
    iwidget.cpp \
    idelegate.cpp

HEADERS  += mainwin.h \
    album.h \
    song.h \
    dialogaddalbum.h \
    dialogeditplaylist.h \
    playlist.h \
    library.h \
    dialogaddsongs.h \
    icon.h \
    dialogeditlibrary.h \
    iwidget.h \
    idelegate.h \
    probe.h

FORMS    += mainwin.ui \
    dialogaddalbum.ui \
    dialogeditplaylist.ui \
    dialogaddsongs.ui \
    dialogeditlibrary.ui

RESOURCES += \
    resource.qrc

DISTFILES +=
