# common.pri — sources partagées entre server et client
# Inclure avec : include($$PWD/../common/common.pri)

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/Crypto.h   \
    $$PWD/Protocol.h

SOURCES += \
    $$PWD/Crypto.cpp   \
    $$PWD/Protocol.cpp
