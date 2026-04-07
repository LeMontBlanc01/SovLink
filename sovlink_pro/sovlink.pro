TEMPLATE  = subdirs
SUBDIRS   = server client

# Le client n'a pas besoin du serveur pour compiler,
# mais si tu veux lancer les deux en séquence : client.depends = server


INCLUDEPATH += C:\Users\elric.evrard\Documents\GitHub\SovLink\libsodium-win64\include
LIBS        += -LC:\Users\elric.evrard\Documents\GitHub\SovLink\libsodium-win64\lib -lsodium
LIBS += -lws2_32
win32 {
    SODIUM_DLL = C:\Users\elric.evrard\Documents\GitHub\SovLink\libsodium-win64\bin\libsodium.dll
    QMAKE_POST_LINK += copy /Y \"$$SODIUM_DLL\" \"$$OUT_PWD\"
}
