cmake_minimum_required(VERSION 3.16)
project(SovLink VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ── Qt6 ──────────────────────────────────────────────────────────────────────
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

# ── Boost ────────────────────────────────────────────────────────────────────
# Sur Windows : définir BOOST_ROOT dans les variables CMake de Qt Creator
# ex: BOOST_ROOT = C:/boost_1_84_0
set(Boost_USE_STATIC_LIBS    ON)
set(Boost_USE_MULTITHREADED  ON)
set(Boost_NO_WARN_NEW_VERSIONS ON)
find_package(Boost REQUIRED COMPONENTS system)

# ── Libsodium ─────────────────────────────────────────────────────────────────
# Sur Windows : définir SODIUM_ROOT dans les variables CMake de Qt Creator
# ex: SODIUM_ROOT = C:/Users/elric.evrard/Documents/GitHub/SovLink/libsodium-win64
if(WIN32)
    # Cherche d'abord via SODIUM_ROOT (variable CMake définie dans Qt Creator)
    if(DEFINED SODIUM_ROOT)
        set(SODIUM_INCLUDE_DIR "${SODIUM_ROOT}/include")
        find_library(SODIUM_LIB
            NAMES sodium libsodium
            PATHS "${SODIUM_ROOT}/lib"
            NO_DEFAULT_PATH
        )
        # DLL pour la copie automatique
        find_file(SODIUM_DLL
            NAMES libsodium.dll sodium.dll
            PATHS "${SODIUM_ROOT}/bin" "${SODIUM_ROOT}/lib"
            NO_DEFAULT_PATH
        )
    else()
        # Fallback : cherche dans les chemins système MinGW
        find_library(SODIUM_LIB NAMES sodium libsodium)
        find_path(SODIUM_INCLUDE_DIR sodium.h)
    endif()
else()
    # Linux / macOS : libsodium installé via apt/brew
    find_library(SODIUM_LIB NAMES sodium REQUIRED)
    find_path(SODIUM_INCLUDE_DIR sodium.h PATH_SUFFIXES sodium)
endif()

if(NOT SODIUM_LIB)
    message(FATAL_ERROR
        "libsodium introuvable.\n"
        "Sous Windows : définissez SODIUM_ROOT dans Qt Creator (Projects > CMake > Add)\n"
        "  SODIUM_ROOT = C:/chemin/vers/libsodium-win64\n"
        "Sous Linux   : sudo apt install libsodium-dev"
    )
endif()

message(STATUS "libsodium : ${SODIUM_LIB}")
message(STATUS "libsodium include : ${SODIUM_INCLUDE_DIR}")

# ── Includes communs ──────────────────────────────────────────────────────────
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${SODIUM_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_SOURCE_DIR}/client
)

# ── pthread (Linux uniquement) ────────────────────────────────────────────────
if(WIN32)
    set(THREAD_LIB "")          # MinGW lie winpthread automatiquement
else()
    set(THREAD_LIB pthread)
endif()

# ── Serveur ───────────────────────────────────────────────────────────────────
add_executable(sovlink_server
    server/main_server.cpp
    server/Server.cpp
    server/ClientSession.cpp
    common/Crypto.cpp
    common/Protocol.cpp
)

target_link_libraries(sovlink_server
    PRIVATE
    Boost::system
    ${SODIUM_LIB}
    Qt6::Core
    ${THREAD_LIB}
)

target_include_directories(sovlink_server PRIVATE
    ${CMAKE_SOURCE_DIR}/server
)

# ── Client ────────────────────────────────────────────────────────────────────
add_executable(sovlink_client
    client/main_client.cpp
    client/Client.cpp
    common/Crypto.cpp
    common/Protocol.cpp
    ui/MainWindow.cpp
    ui/ChatWindow.cpp
    ui/LoginWindow.cpp
)

target_link_libraries(sovlink_client
    PRIVATE
    Boost::system
    ${SODIUM_LIB}
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    ${THREAD_LIB}
)

target_include_directories(sovlink_client PRIVATE
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_SOURCE_DIR}/ui
)

# ── Copie automatique de libsodium.dll sur Windows ────────────────────────────
if(WIN32 AND SODIUM_DLL)
    message(STATUS "libsodium DLL : ${SODIUM_DLL}")
    add_custom_command(TARGET sovlink_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SODIUM_DLL}"
            "$<TARGET_FILE_DIR:sovlink_client>"
        COMMENT "Copie libsodium.dll dans le dossier de sortie..."
    )
    add_custom_command(TARGET sovlink_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SODIUM_DLL}"
            "$<TARGET_FILE_DIR:sovlink_server>"
        COMMENT "Copie libsodium.dll dans le dossier de sortie..."
    )
endif()

# ── Déploiement Qt (windeployqt) sur Windows ─────────────────────────────────
if(WIN32)
    find_program(WINDEPLOYQT windeployqt
        HINTS "${Qt6_DIR}/../../../bin"
    )
    if(WINDEPLOYQT)
        add_custom_command(TARGET sovlink_client POST_BUILD
            COMMAND "${WINDEPLOYQT}" "$<TARGET_FILE:sovlink_client>"
            COMMENT "Déploiement des DLLs Qt..."
        )
    endif()
endif()