#-------------------------------------------------
#
# Project created by QtCreator 2016-12-18T17:16:15
#
#-------------------------------------------------

QT       += core gui \
            multimedia \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CemiSec
TEMPLATE = app

CONFIG += c++14

LIBS += -ltag

SOURCES += main.cpp\
    mainwin.cpp \
    album.cpp \
    song.cpp \
    dialogaddalbum.cpp \
    dialogeditplaylist.cpp

HEADERS  += mainwin.h \
    album.h \
    song.h \
    dialogaddalbum.h \
    dialogeditplaylist.h

FORMS    += mainwin.ui \
    dialogaddalbum.ui \
    dialogeditplaylist.ui

RESOURCES += \
    resource.qrc

DISTFILES += \
    songs.txt \
    albums.txt \
