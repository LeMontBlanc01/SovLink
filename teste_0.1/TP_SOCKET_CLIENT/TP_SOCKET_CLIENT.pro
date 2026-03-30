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

INCLUDEPATH += C:\Users\jean.chevrier\Documents\GitHub\SovLink\libsodium-win64\include
LIBS        += -LC:\Users\jean.chevrier\Documents\GitHub\SovLink\libsodium-win64\lib -lsodium
LIBS += -lws2_32
win32 {
    SODIUM_DLL = C:\Users\jean.chevrier\Documents\GitHub\SovLink\libsodium-win64\bin\libsodium.dll
    QMAKE_POST_LINK += copy /Y \"$$SODIUM_DLL\" \"$$OUT_PWD\"
}
