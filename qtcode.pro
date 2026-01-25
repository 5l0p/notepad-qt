QT       += core gui widgets

TARGET = qtcode
TEMPLATE = app

CONFIG += c++17

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
