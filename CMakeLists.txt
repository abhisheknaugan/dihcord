cmake_minimum_required(VERSION 3.20)
project(RipcordAlt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# --- GPU-off / lightweight rendering policy -------------------------------
# We ONLY use Qt Widgets (raster/software paint engine, CPU-only).
# We deliberately do NOT link Qt6::Quick, Qt6::QuickWidgets, or
# Qt6::WebEngineWidgets anywhere in this project. If you ever add a
# dependency, grep this file first and ask whether it pulls in a GPU
# compositor before adding it.
add_definitions(-DQT_NO_OPENGL)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network WebSockets Sql Multimedia)

set(SOURCES
    src/main.cpp
    src/logindialog.cpp
    src/restclient.cpp
    src/gatewayclient.cpp
    src/mainwindow.cpp
    src/tokenstore.cpp
    src/processwatcher.cpp
    src/profiledialog.cpp
    src/voiceclient.cpp
    src/audioengine.cpp
    src/equalizer.cpp
    src/voicesettingsdialog.cpp
    src/davesession.cpp
)

set(HEADERS
    src/logindialog.h
    src/restclient.h
    src/gatewayclient.h
    src/mainwindow.h
    src/tokenstore.h
    src/processwatcher.h
    src/profiledialog.h
    src/voiceclient.h
    src/audioengine.h
    src/equalizer.h
    src/voicesettingsdialog.h
    src/davesession.h
)

add_executable(RipcordAlt WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(RipcordAlt PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
    Qt6::Sql
    Qt6::Multimedia
)

# Windows Credential Manager for secure token storage (Phase 1: login)
target_link_libraries(RipcordAlt PRIVATE advapi32 crypt32)

# --- Voice audio: Opus (codec) + libsodium (encryption) -------------------
# These come from MSYS2's mingw-w64 package repo (pacman), NOT from Qt's
# installer. Set MSYS2_ROOT to wherever you installed MSYS2 - default
# guess below is the standard install location.
set(MSYS2_ROOT "C:/msys64/mingw64" CACHE PATH "Path to MSYS2's mingw64 folder")

target_include_directories(RipcordAlt PRIVATE "${MSYS2_ROOT}/include" "${MSYS2_ROOT}/include/opus")
target_link_directories(RipcordAlt PRIVATE "${MSYS2_ROOT}/lib")
target_link_libraries(RipcordAlt PRIVATE opus sodium)

# --- DAVE (real E2EE voice) - libdave + mlspp, built via vcpkg -----------
set(LIBDAVE_ROOT "C:/Users/abhis/Downloads/libdave/cpp" CACHE PATH "Path to libdave/cpp")
set(LIBDAVE_VCPKG_INSTALLED "${LIBDAVE_ROOT}/vcpkg/installed/x64-mingw-dynamic" CACHE PATH "vcpkg installed dir")

target_include_directories(RipcordAlt PRIVATE
    "${LIBDAVE_ROOT}/includes"
    "${LIBDAVE_VCPKG_INSTALLED}/include"
)
target_link_directories(RipcordAlt PRIVATE
    "${LIBDAVE_ROOT}/build"
    "${LIBDAVE_VCPKG_INSTALLED}/lib"
)
target_link_libraries(RipcordAlt PRIVATE
    dave
    mlspp
    hpke
    mls_ds
    bytes
    tls_syntax
    ssl
    crypto
)
