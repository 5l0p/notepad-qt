QT       += core gui widgets

TARGET = notepad
TEMPLATE = app

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    syntaxhighlighter.cpp

HEADERS += \
    mainwindow.h \
    codeeditor.h \
    syntaxhighlighter.h
