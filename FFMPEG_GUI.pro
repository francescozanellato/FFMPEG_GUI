#-------------------------------------------------
#
# Project created by QtCreator 2013-07-26T15:06:09
#
#-------------------------------------------------
#*
#* Author: Francesco Paolo Luca Zanellato
#* Copyright (C) 2026 Francesco Paolo Luca Zanellato


QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FFMPEG_GUI
TEMPLATE = app

VERSION = 1.0.4
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
DEFINES += APP_NAME=\\\"FFMPEG_GUI\\\"
DEFINES += APP_ORGANIZATION=\\\"Francesco Paolo Luca Zanellato\\\"
DEFINES += APP_ORGANIZATION_DOMAIN=\\\"https://francescozanellato.blogspot.com/\\\"
DEFINES += MY_COPYRIGHT=\\\"Copyright_2026_Francesco_Paolo_Luca_Zanellato.\\\"

QMAKE_TARGET_COPYRIGHT = "\\251 2026 Francesco Paolo Luca Zanellato. License: GPL 3.0"


# COMPILE LITE VERSION OF THE PROGRAM:
# DEFINES += COMPILE_LITE_VERSION

# Print Debug Messages:
# DEFINES += DEBUG_MESSAGES


SOURCES += main.cpp \
    ffmpeg_gui.cpp

HEADERS  += \
    ffmpeg_gui.h

# FORMS    += ffmpeg_gui.ui

# ICON in executable:
RC_ICONS = icon.ico

# 2026-07-12 added macx {} if statement:
macx {
QMAKE_APPLE_DEVICE_ARCHS = x86_64 x86_64h arm64
}


CONFIG(debug, debug|release) {
    DESTDIR = $${PWD}/build/debug
} else {
    DESTDIR = $${PWD}/release
}

COPIES += vbsStartupFile
vbsStartupFile.files = $$files($${PWD}/*.vbs)
vbsStartupFile.files += $$files($${PWD}/*.ini)
vbsStartupFile.path = $${PWD}/release

# 2026-07-12 this is made optional:
# QMAKE_LFLAGS += -Wl,--rpath=\$$ORIGIN/lib


