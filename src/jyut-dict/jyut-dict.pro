#-------------------------------------------------
#
# Project created by QtCreator 2018-10-19T02:04:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = jyut-dict
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++14

SOURCES += \
    main.cpp \
    components/searchlineedit.cpp \
    windows/mainwindow.cpp \
    components/maintoolbar.cpp \
    components/definitionscrollarea.cpp \
    components/searchlistview.cpp \
    components/mainsplitter.cpp

HEADERS += \
    mainwindow.h \
    components/searchlineedit.h \
    windows/mainwindow.h \
    components/maintoolbar.h \
    components/definitionscrollarea.h \
    components/searchlistview.h \
    components/mainsplitter.h

RESOURCES += \
    resources/resource.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
