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

find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network WebSockets Sql)

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
)

add_executable(RipcordAlt WIN32 ${SOURCES} ${HEADERS})

target_link_libraries(RipcordAlt PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    Qt6::WebSockets
    Qt6::Sql
)

# Windows Credential Manager for secure token storage (Phase 1: login)
target_link_libraries(RipcordAlt PRIVATE advapi32 crypt32)
