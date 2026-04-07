TARGET   = sovlink_server
TEMPLATE = app

# ── C++ et Qt ────────────────────────────────────────────────
CONFIG  += c++17 console
CONFIG  -= app_bundle
QT      += core
QT      -= gui

# ── Sources communes (Crypto + Protocol) ─────────────────────
include($$PWD/../common/common.pri)

# ── Sources du serveur ───────────────────────────────────────
HEADERS += \
    Server.h         \
    ClientSession.h

SOURCES += \
    main_server.cpp  \
    Server.cpp       \
    ClientSession.cpp

# ── Boost Asio ───────────────────────────────────────────────
# Boost Asio est header-only ; seul boost_system nécessite un link
# Adapter le chemin si Boost n'est pas dans /usr/include
INCLUDEPATH += /usr/include
LIBS        += -lboost_system

# ── libsodium ────────────────────────────────────────────────
unix {
    # pkg-config disponible (Linux / macOS avec Homebrew)
    CONFIG      += link_pkgconfig
    PKGCONFIG   += libsodium
}
win32 {
    # Windows : adapter ces chemins selon votre installation vcpkg / msys2
    INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
    LIBS        += -LC:/vcpkg/installed/x64-windows/lib -lsodium
}

# ── Threads ──────────────────────────────────────────────────
unix: LIBS += -lpthread
