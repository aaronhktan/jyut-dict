#-------------------------------------------------
#
# Project created by QtCreator 2018-10-19T02:04:51
#
#-------------------------------------------------

QT       += core gui sql

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
    windows/mainwindow.cpp \
    components/definitionheaderwidget.cpp \
    components/definitionscrollarea.cpp \
    components/entryheaderwidget.cpp \
    components/mainsplitter.cpp \
    components/maintoolbar.cpp \
    components/searchlineedit.cpp \
    components/searchlistview.cpp \
    logic/entry/entry.cpp \
    logic/entry/sentence.cpp \
    components/definitionsectionwidget.cpp \
    components/definitioncontentwidget.cpp \
    logic/entry/definitionsset.cpp \
    components/definitionwidget.cpp \
    logic/search/sqlsearch.cpp \
    logic/search/sqldatabasemanager.cpp \
    components/entrylistmodel.cpp \
    components/entrydelegate.cpp \
    components/searchoptionsradiogroupbox.cpp \
    logic/search/searchoptionsmediator.cpp

HEADERS += \
    windows/mainwindow.h \
    components/definitionheaderwidget.h \
    components/definitionscrollarea.h \
    components/entryheaderwidget.h \
    components/isectionheaderwidget.h \
    components/mainsplitter.h \
    components/maintoolbar.h \
    components/searchlineedit.h \
    components/searchlistview.h \
    logic/entry/entry.h \
    logic/entry/sentence.h \
    components/definitionsectionwidget.h \
    components/definitioncontentwidget.h \
    logic/entry/definitionsset.h \
    components/definitionwidget.h \
    logic/search/sqlsearch.h \
    logic/search/isearchobservable.h \
    logic/search/isearchobserver.h \
    logic/search/sqldatabasemanager.h \
    logic/search/isearch.h \
    components/entrylistmodel.h \
    components/entrydelegate.h \
    logic/entry/entryphoneticoptions.h \
    logic/entry/entrycharactersoptions.h \
    components/searchoptionsradiogroupbox.h \
    logic/search/searchparameters.h \
    logic/search/isearchoptionsmediator.h \
    logic/search/searchoptionsmediator.h \
    components/isearchlineedit.h

RESOURCES += \
    resources/resource.qrc

macx: {
    ICON = resources/icon/icon.icns
    QMAKE_INFO_PLIST = platform/mac/Info.plist

    # Adds files to the Resources folder in macOS bundle
    APP_DB_FILES.files = resources/db/eng.db
    APP_DB_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_DB_FILES
}

win32: {
    RC_ICONS = resources/icon/icon.ico
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
