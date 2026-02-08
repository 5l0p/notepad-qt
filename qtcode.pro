QT       += core gui widgets

TARGET = qtcode
TEMPLATE = app

CONFIG += c++17

# Static linking: use with a static Qt build (see build-static.sh)
contains(CONFIG, static) {
    QMAKE_LFLAGS += -static-libgcc -static-libstdc++
    # Pull in xcb platform plugin for static builds
    QTPLUGIN += qxcb
}

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    syntaxhighlighter.cpp \
    finddialog.cpp

HEADERS += \
    mainwindow.h \
    codeeditor.h \
    syntaxhighlighter.h \
    finddialog.h
