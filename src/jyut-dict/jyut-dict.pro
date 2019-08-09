#-------------------------------------------------
#
# Project created by QtCreator 2018-10-19T02:04:51
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = "Jyut Dictionary"
TEMPLATE = app

VERSION = 0.19.06.14
QMAKE_TARGET_COMPANY = "Aaron Tan"
QMAKE_TARGET_PRODUCT = "Jyut Dictionary"
QMAKE_TARGET_DESCRIPTION = "Jyut Dictionary"
QMAKE_TARGET_COPYRIGHT = "Aaron Tan, 2019"

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
    components/definitioncontentwidget.cpp \
    components/definitionheaderwidget.cpp \
    components/definitionscrollarea.cpp \
    components/definitionsectionwidget.cpp \
    components/definitionwidget.cpp \
    components/dictionarylistdelegate.cpp \
    components/dictionarylistmodel.cpp \
    components/dictionarylistview.cpp \
    components/dictionarytab.cpp \
    components/entryheaderwidget.cpp \
    components/mainsplitter.cpp \
    components/maintoolbar.cpp \
    components/resultlistdelegate.cpp \
    components/resultlistmodel.cpp \
    components/resultlistview.cpp \
    components/searchlineedit.cpp \
    components/searchoptionsradiogroupbox.cpp \
    logic/database/sqldatabasemanager.cpp \
    logic/database/sqldatabaseutils.cpp \
    logic/dictionary/dictionarymetadata.cpp \
    logic/dictionary/dictionarysource.cpp \
    logic/entry/definitionsset.cpp \
    logic/entry/entry.cpp \
    logic/entry/sentence.cpp \
    logic/search/searchoptionsmediator.cpp \
    logic/search/sqlsearch.cpp \
    logic/update/githubreleasechecker.cpp \
    logic/utils/utils.cpp \
    windows/mainwindow.cpp \
    windows/settingswindow.cpp \
    windows/updatewindow.cpp

HEADERS += \
    components/definitioncontentwidget.h \
    components/definitionheaderwidget.h \
    components/definitionscrollarea.h \
    components/definitionsectionwidget.h \
    components/definitionwidget.h \
    components/dictionarylistdelegate.h \
    components/dictionarylistmodel.h \
    components/dictionarylistview.h \
    components/dictionarytab.h \
    components/entryheaderwidget.h \
    components/isearchlineedit.h \
    components/isectionheaderwidget.h \
    components/mainsplitter.h \
    components/maintoolbar.h \
    components/resultlistdelegate.h \
    components/resultlistmodel.h \
    components/resultlistview.h \
    components/searchlineedit.h \
    components/searchoptionsradiogroupbox.h \
    logic/database/sqldatabasemanager.h \
    logic/database/sqldatabaseutils.h \
    logic/dictionary/dictionarymetadata.h \
    logic/dictionary/dictionarysource.h \
    logic/entry/definitionsset.h \
    logic/entry/entry.h \
    logic/entry/entrycharactersoptions.h \
    logic/entry/entryphoneticoptions.h \
    logic/entry/sentence.h \
    logic/search/isearch.h \
    logic/search/isearchobservable.h \
    logic/search/isearchobserver.h \
    logic/search/isearchoptionsmediator.h \
    logic/search/searchoptionsmediator.h \
    logic/search/searchparameters.h \
    logic/search/sqlsearch.h \
    logic/update/githubreleasechecker.h \
    logic/update/iupdatechecker.h \
    logic/utils/utils.h \
    logic/utils/utils_mac.h \
    windows/mainwindow.h \
    windows/settingswindow.h \
    windows/updatewindow.h

RESOURCES += \
    resources/resource.qrc

macx: {
    LIBS += -framework AppKit
    OBJECTIVE_SOURCES += \
        logic/utils/utils_mac.mm \

    ICON = resources/icon/icon.icns
    QMAKE_INFO_PLIST = platform/mac/Info.plist

    # Adds files to the Resources folder in macOS bundle
    APP_DB_FILES.files = resources/db/dict.db
    APP_DB_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_DB_FILES
}

win32: {
    # Prevent creation of release and debug folders in build directory
    CONFIG -= debug_and_release debug_and_release_target
    RC_ICONS = resources/icon/icon.ico
}

unix:!macx {
    # Move files to appropriate locations on desktop to install the program
    binfile.extra = cp \"$$system_path($$OUT_PWD)/Jyut Dictionary\" $$system_path($$OUT_PWD)/jyut-dict
    binfile.files += $$system_path($$OUT_PWD)/jyut-dict
    binfile.path = /usr/bin/
    binfile.CONFIG += no_check_exist
    dictfile.files += resources/db/dict.db
    dictfile.path = /usr/share/jyut-dict/dictionaries/
    shortcutfiles.files += platform/linux/jyut-dict.desktop
    shortcutfiles.path = /usr/share/applications/
    icon.files += resources/icon/jyut-dict.svg
    icon.path = /usr/share/icons/hicolor/scalable/apps/
    INSTALLS += binfile
    INSTALLS += dictfile
    INSTALLS += shortcutfiles
    INSTALLS += icon
}

unix|win32:!macx {
    # Copy dictionary database to the build directory
    copydata.commands = $(COPY_DIR) \"$$system_path($$PWD/resources/db/dict.db)\" \"$$system_path($$OUT_PWD)\"
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata
}

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
