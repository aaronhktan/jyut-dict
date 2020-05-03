#-------------------------------------------------
#
# Project created by QtCreator 2018-10-19T02:04:51
#
#-------------------------------------------------

QT       += core gui network sql texttospeech

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
    components/definitioncard/definitioncardwidget.cpp \
    components/definitioncard/definitioncontentwidget.cpp \
    components/definitioncard/definitionheaderwidget.cpp \
    components/dictionarylist/dictionarylistdelegate.cpp \
    components/dictionarylist/dictionarylistmodel.cpp \
    components/dictionarylist/dictionarylistview.cpp \
    components/entrysearchresult/resultlistdelegate.cpp \
    components/entrysearchresult/resultlistmodel.cpp \
    components/entrysearchresult/resultlistview.cpp \
    components/entryview/entrycontentwidget.cpp \
    components/entryview/entryheaderwidget.cpp \
    components/entryview/entryscrollarea.cpp \
    components/entryview/entryscrollareawidget.cpp \
    components/mainwindow/mainsplitter.cpp \
    components/mainwindow/maintoolbar.cpp \
    components/mainwindow/searchlineedit.cpp \
    components/mainwindow/searchoptionsradiogroupbox.cpp \
    components/sentencecard/loadingwidget.cpp \
    components/settings/advancedtab.cpp \
    components/settings/contacttab.cpp \
    components/settings/dictionarytab.cpp \
    components/settings/settingstab.cpp \
    logic/search/sqlsearch.cpp \
    logic/sentence/sentenceset.cpp \
    logic/sentence/sourcesentence.cpp \
    main.cpp \
    dialogs/defaultdialog.cpp \
    dialogs/dictionarytabfailuredialog.cpp \
    dialogs/entryspeakerrordialog.cpp \
    dialogs/noupdatedialog.cpp \
    dialogs/resetsettingsdialog.cpp \
    logic/analytics/analytics.cpp \
    logic/database/sqldatabasemanager.cpp \
    logic/database/sqldatabaseutils.cpp \
    logic/dictionary/dictionarymetadata.cpp \
    logic/dictionary/dictionarysource.cpp \
    logic/entry/definitionsset.cpp \
    logic/entry/entry.cpp \
    logic/entry/entryspeaker.cpp \
    logic/search/searchoptionsmediator.cpp \
    logic/settings/settings.cpp \
    logic/settings/settingsutils.cpp \
    logic/update/githubreleasechecker.cpp \
    logic/utils/utils.cpp \
    logic/utils/utils_qt.cpp \
    windows/aboutwindow.cpp \
    windows/mainwindow.cpp \
    windows/settingswindow.cpp \
    windows/updatewindow.cpp

HEADERS += \
    components/definitioncard/definitioncardwidget.h \
    components/definitioncard/definitioncontentwidget.h \
    components/definitioncard/definitionheaderwidget.h \
    components/definitioncard/icardheaderwidget.h \
    components/dictionarylist/dictionarylistdelegate.h \
    components/dictionarylist/dictionarylistmodel.h \
    components/dictionarylist/dictionarylistview.h \
    components/entrysearchresult/resultlistdelegate.h \
    components/entrysearchresult/resultlistmodel.h \
    components/entrysearchresult/resultlistview.h \
    components/entryview/entrycontentwidget.h \
    components/entryview/entryheaderwidget.h \
    components/entryview/entryscrollarea.h \
    components/entryview/entryscrollareawidget.h \
    components/mainwindow/isearchlineedit.h \
    components/mainwindow/mainsplitter.h \
    components/mainwindow/maintoolbar.h \
    components/mainwindow/searchlineedit.h \
    components/mainwindow/searchoptionsradiogroupbox.h \
    components/sentencecard/loadingwidget.h \
    components/settings/advancedtab.h \
    components/settings/contacttab.h \
    components/settings/dictionarytab.h \
    components/settings/settingstab.h \
    dialogs/defaultdialog.h \
    dialogs/dictionarytabfailuredialog.h \
    dialogs/entryspeakerrordialog.h \
    dialogs/noupdatedialog.h \
    dialogs/resetsettingsdialog.h \
    logic/analytics/analytics.h \
    logic/analytics/analyticsconfig.h \
    logic/database/sqldatabasemanager.h \
    logic/database/sqldatabaseutils.h \
    logic/dictionary/dictionarymetadata.h \
    logic/dictionary/dictionarysource.h \
    logic/entry/definitionsset.h \
    logic/entry/entry.h \
    logic/entry/entrycharactersoptions.h \
    logic/entry/entryphoneticoptions.h \
    logic/entry/entryspeaker.h \
    logic/search/isearch.h \
    logic/search/isearchobservable.h \
    logic/search/isearchobserver.h \
    logic/search/isearchoptionsmediator.h \
    logic/search/searchoptionsmediator.h \
    logic/search/searchparameters.h \
    logic/search/sqlsearch.h \
    logic/sentence/sentenceset.h \
    logic/sentence/sourcesentence.h \
    logic/settings/settings.h \
    logic/settings/settingsutils.h \
    logic/strings/strings.h \
    logic/update/githubreleasechecker.h \
    logic/update/iupdatechecker.h \
    logic/utils/qvariantutils.h \
    logic/utils/utils.h \
    logic/utils/utils_mac.h \
    logic/utils/utils_qt.h \
    windows/aboutwindow.h \
    windows/mainwindow.h \
    windows/settingswindow.h \
    windows/updatewindow.h

RESOURCES += \
    resources/resource.qrc

TRANSLATIONS += \
    resources/translations/jyutdictionary-en.ts \
    resources/translations/jyutdictionary-fr.ts \
    resources/translations/jyutdictionary-fr_CA.ts \
    resources/translations/jyutdictionary-yue_Hans.ts \
    resources/translations/jyutdictionary-yue_Hant.ts \
    resources/translations/jyutdictionary-zh_Hans.ts \
    resources/translations/jyutdictionary-zh_Hant.ts \
    resources/translations/jyutdictionary-zh_Hans_HK.ts \
    resources/translations/jyutdictionary-zh_Hant_HK.ts

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

    # Add settings to Resources folder in macOS bundle
    APP_SETTINGS_FILES.files = resources/settings/settings.ini
    APP_SETTINGS_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_SETTINGS_FILES

    # Add translations of application name
    TRANSLATE_en.files = platform/mac/en.lproj/InfoPlist.strings
    TRANSLATE_en.path = Contents/Resources/en.lproj
    QMAKE_BUNDLE_DATA += TRANSLATE_en

    TRANSLATE_fr.files = platform/mac/fr.lproj/InfoPlist.strings
    TRANSLATE_fr.path = Contents/Resources/fr.lproj
    QMAKE_BUNDLE_DATA += TRANSLATE_fr

    TRANSLATE_yue_Hans.files = platform/mac/yue_Hans.lproj/InfoPlist.strings
    TRANSLATE_yue_Hans.path = Contents/Resources/yue-Hans.lproj
    QMAKE_BUNDLE_DATA += TRANSLATE_yue_Hans

    TRANSLATE_yue_Hant.files = platform/mac/yue_Hant.lproj/InfoPlist.strings
    TRANSLATE_yue_Hant.path = Contents/Resources/yue-Hant.lproj
    QMAKE_BUNDLE_DATA += TRANSLATE_yue_Hant

    TRANSLATE_zh_Hans.files = platform/mac/zh_Hans.lproj/InfoPlist.strings
    TRANSLATE_zh_Hans.path = Contents/Resources/zh-Hans.lproj
    QMAKE_BUNDLE_DATA += TRANSLATE_zh_Hans

    TRANSLATE_zh_Hant.files = platform/mac/zh_Hant.lproj/InfoPlist.strings
    TRANSLATE_zh_Hant.path = Contents/Resources/zh-Hant.lproj
    QMAKE_BUNDLE_DATA += TRANSLATE_zh_Hant
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

    # Copy settings file to build directory
    copysettings.commands = $(COPY_DIR) \"$$system_path($$PWD/resources/settings/settings.ini)\" \"$$system_path($$OUT_PWD)\"
    second.depends = $(second) copysettings
    export(second.depends)
    export(copysettings.commands)
    QMAKE_EXTRA_TARGETS += second copysettings
}

# Default rules for deployment.
#qnx: target.path = /tmp/$${TARGET}/bin
#else: unix:!android: target.path = /opt/$${TARGET}/bin
#!isEmpty(target.path): INSTALLS += target
