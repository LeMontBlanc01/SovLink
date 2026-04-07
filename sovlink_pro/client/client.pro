TARGET   = sovlink_client
TEMPLATE = app

# ── C++ et Qt ────────────────────────────────────────────────
CONFIG  += c++17
CONFIG  -= app_bundle
QT      += core widgets network

# ── Sources communes (Crypto + Protocol) ─────────────────────
include($$PWD/../common/common.pri)

# ── Sources client ───────────────────────────────────────────
HEADERS += \
    Client.h                 \
    ../ui/MainWindow.h       \
    ../ui/LoginWindow.h      \
    ../ui/ChatWindow.h

SOURCES += \
    main_client.cpp          \
    Client.cpp               \
    ../ui/MainWindow.cpp     \
    ../ui/LoginWindow.cpp    \
    ../ui/ChatWindow.cpp

# ── Ressources (icône, logo) — décommenter si tu crées un .qrc ──
# RESOURCES += ../resources/resources.qrc

# ── libsodium ────────────────────────────────────────────────
unix {
    CONFIG      += link_pkgconfig
    PKGCONFIG   += libsodium
}
win32 {
    INCLUDEPATH += C:/vcpkg/installed/x64-windows/include
    LIBS        += -LC:/vcpkg/installed/x64-windows/lib -lsodium
}

# ── Boost (lien boost_system requis par Asio si utilisé côté client) ─
INCLUDEPATH += /usr/include
unix: LIBS  += -lboost_system -lpthread
win32: LIBS += -lboost_system-mt

# ── Métadonnées (reprises depuis main_client.cpp) ────────────
VERSION          = 1.0
QMAKE_TARGET_COMPANY    = BTSCIEL1
QMAKE_TARGET_PRODUCT    = SovLink
QMAKE_TARGET_DESCRIPTION = Messagerie chiffrée éphémère
