#-------------------------------------------------
#
# Project created by QtCreator 2016-12-18T17:16:15
#
#-------------------------------------------------

QT       += core gui \
            multimedia \
            sql \

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CemiSec
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
    library.cpp

HEADERS  += mainwin.h \
    album.h \
    song.h \
    dialogaddalbum.h \
    dialogeditplaylist.h \
    playlist.h \
    library.h

FORMS    += mainwin.ui \
    dialogaddalbum.ui \
    dialogeditplaylist.ui

RESOURCES += \
    resource.qrc

DISTFILES +=
