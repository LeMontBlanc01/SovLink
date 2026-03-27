#-------------------------------------------------
#
# Project created by QtCreator 2015-09-21T14:03:15
#
#-------------------------------------------------

QT       += core
QT       += network
QT       -= gui

TARGET = TP_SOCKET_CLIENT
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    client.cpp

HEADERS += \
    client.h

INCLUDEPATH += C:/Documents/GitHub/SovLink/libsodium/include
LIBS += -LC:/Documents/GitHub/SovLink/libsodium/x64/Release/v142/dynamic -lsodium
