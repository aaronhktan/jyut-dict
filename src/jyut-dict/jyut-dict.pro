#-------------------------------------------------
#
# Project created by QtCreator 2018-10-19T02:04:51
#
#-------------------------------------------------

QT       += core gui network qml sql svg texttospeech widgets
QMAKE_MACOSX_DEPLOYMENT_TARGET = 11.0

TARGET = "Jyut Dictionary"
TEMPLATE = app

VERSION = 1.22.02.07
QMAKE_TARGET_COMPANY = "Aaron Tan"
QMAKE_TARGET_PRODUCT = "Jyut Dictionary"
QMAKE_TARGET_DESCRIPTION = "Jyut Dictionary"
QMAKE_TARGET_COPYRIGHT = "Aaron Tan, 2022"

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

QMAKE_CXXFLAGS += "-Wno-implicit-fallthrough"

SOURCES += \
    components/definitioncard/definitioncardsection.cpp \
    components/definitioncard/definitioncardwidget.cpp \
    components/definitioncard/definitioncontentwidget.cpp \
    components/definitioncard/definitionheaderwidget.cpp \
    components/dictionarylist/dictionarylistdelegate.cpp \
    components/dictionarylist/dictionarylistmodel.cpp \
    components/dictionarylist/dictionarylistview.cpp \
    components/entrysearchresult/resultlistdelegate.cpp \
    components/entrysearchresult/resultlistmodel.cpp \
    components/entrysearchresult/resultlistview.cpp \
    components/entryview/entryactionwidget.cpp \
    components/entryview/entrycontentwidget.cpp \
    components/entryview/entryheaderwidget.cpp \
    components/entryview/entryscrollarea.cpp \
    components/entryview/entryscrollareawidget.cpp \
    components/entryview/entryviewsentencecardsection.cpp \
    components/favouritewindow/favouritesplitter.cpp \
    components/historyview/searchhistorylistdelegate.cpp \
    components/historyview/searchhistorylistmodel.cpp \
    components/historyview/searchhistorylistview.cpp \
    components/historyview/searchhistorytab.cpp \
    components/historyview/viewhistorylistdelegate.cpp \
    components/historyview/viewhistorylistmodel.cpp \
    components/historyview/viewhistorylistview.cpp \
    components/historyview/viewhistorytab.cpp \
    components/layouts/flowlayout.cpp \
    components/magnifywindow/magnifyscrollarea.cpp \
    components/magnifywindow/magnifyscrollareawidget.cpp \
    components/mainwindow/mainsplitter.cpp \
    components/mainwindow/maintoolbar.cpp \
    components/mainwindow/searchlineedit.cpp \
    components/mainwindow/searchoptionsradiogroupbox.cpp \
    components/related/relatedbutton.cpp \
    components/related/relatedbuttoncontentwidget.cpp \
    components/related/relatedbuttonheaderwidget.cpp \
    components/related/relatedsection.cpp \
    components/sentencecard/loadingwidget.cpp \
    components/sentencecard/sentencecardwidget.cpp \
    components/sentencecard/sentencecontentwidget.cpp \
    components/sentencecard/sentenceheaderwidget.cpp \
    components/sentencesearchresult/sentenceresultlistdelegate.cpp \
    components/sentencesearchresult/sentenceresultlistmodel.cpp \
    components/sentencesearchresult/sentenceresultlistview.cpp \
    components/sentenceview/sentencescrollarea.cpp \
    components/sentenceview/sentencescrollareawidget.cpp \
    components/sentenceview/sentenceviewcontentwidget.cpp \
    components/sentenceview/sentenceviewheaderwidget.cpp \
    components/sentenceview/sentenceviewsentencecardsection.cpp \
    components/sentencewindow/sentencesplitter.cpp \
    components/settings/advancedtab.cpp \
    components/settings/contacttab.cpp \
    components/settings/dictionarytab.cpp \
    components/settings/settingstab.cpp \
    components/settings/texttab.cpp \
    dialogs/defaultdialog.cpp \
    dialogs/dictionarytabfailuredialog.cpp \
    dialogs/entryspeakerrordialog.cpp \
    dialogs/exportdatabasedialog.cpp \
    dialogs/noupdatedialog.cpp \
    dialogs/overwriteconflictingdictionarydialog.cpp \
    dialogs/resetsettingsdialog.cpp \
    dialogs/restoredatabasedialog.cpp \
    logic/database/queryparseutils.cpp \
    logic/database/sqluserdatautils.cpp \
    logic/database/sqluserhistoryutils.cpp \
    logic/search/sqlsearch.cpp \
    logic/sentence/sentenceset.cpp \
    logic/sentence/sourcesentence.cpp \
    main.cpp \
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
    logic/utils/chineseutils.cpp \
    logic/utils/utils.cpp \
    logic/utils/utils_qt.cpp \
    windows/aboutwindow.cpp \
    windows/historywindow.cpp \
    windows/mainwindow.cpp \
    windows/settingswindow.cpp \
    windows/updatewindow.cpp

HEADERS += \
    components/definitioncard/definitioncardsection.h \
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
    components/entryview/entryactionwidget.h \
    components/entryview/entrycontentwidget.h \
    components/entryview/entryheaderwidget.h \
    components/entryview/entryscrollarea.h \
    components/entryview/entryscrollareawidget.h \
    components/entryview/entryviewsentencecardsection.h \
    components/favouritewindow/favouritesplitter.h \
    components/historyview/searchhistorylistdelegate.h \
    components/historyview/searchhistorylistmodel.h \
    components/historyview/searchhistorylistview.h \
    components/historyview/searchhistorytab.h \
    components/historyview/viewhistorylistdelegate.h \
    components/historyview/viewhistorylistmodel.h \
    components/historyview/viewhistorylistview.h \
    components/historyview/viewhistorytab.h \
    components/layouts/flowlayout.h \
    components/magnifywindow/magnifyscrollarea.h \
    components/magnifywindow/magnifyscrollareawidget.h \
    components/mainwindow/isearchlineedit.h \
    components/mainwindow/mainsplitter.h \
    components/mainwindow/maintoolbar.h \
    components/mainwindow/searchlineedit.h \
    components/mainwindow/searchoptionsradiogroupbox.h \
    components/related/relatedbutton.h \
    components/related/relatedbuttoncontentwidget.h \
    components/related/relatedbuttonheaderwidget.h \
    components/related/relatedsection.h \
    components/sentencecard/loadingwidget.h \
    components/sentencecard/sentencecardwidget.h \
    components/sentencecard/sentencecontentwidget.h \
    components/sentencecard/sentenceheaderwidget.h \
    components/sentencesearchresult/sentenceresultlistdelegate.h \
    components/sentencesearchresult/sentenceresultlistmodel.h \
    components/sentencesearchresult/sentenceresultlistview.h \
    components/sentenceview/sentencescrollarea.h \
    components/sentenceview/sentencescrollareawidget.h \
    components/sentenceview/sentenceviewcontentwidget.h \
    components/sentenceview/sentenceviewheaderwidget.h \
    components/sentenceview/sentenceviewsentencecardsection.h \
    components/sentencewindow/sentencesplitter.h \
    components/settings/advancedtab.h \
    components/settings/contacttab.h \
    components/settings/dictionarytab.h \
    components/settings/settingstab.h \
    components/settings/texttab.h \
    dialogs/defaultdialog.h \
    dialogs/dictionarytabfailuredialog.h \
    dialogs/entryspeakerrordialog.h \
    dialogs/exportdatabasedialog.h \
    dialogs/noupdatedialog.h \
    dialogs/overwriteconflictingdictionarydialog.h \
    dialogs/resetsettingsdialog.h \
    dialogs/restoredatabasedialog.h \
    logic/database/queryparseutils.h \
    logic/database/sqldatabasemanager.h \
    logic/database/sqldatabaseutils.h \
    logic/database/sqluserdatautils.h \
    logic/database/sqluserhistoryutils.h \
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
    logic/search/searchqueries.h \
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
    logic/utils/chineseutils.h \
    logic/utils/qvariantutils.h \
    logic/utils/utils.h \
    logic/utils/utils_qt.h \
    windows/aboutwindow.h \
    windows/historywindow.h \
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
    OBJECTIVE_SOURCES += logic/utils/utils_mac.mm
    HEADERS += logic/utils/utils_mac.h

    ICON = resources/icon/icon.icns
    QMAKE_INFO_PLIST = platform/mac/Info.plist

    # Adds files to the Resources folder in macOS bundle
    APP_DB_FILES.files = resources/db/dict.db
    APP_DB_FILES.files += resources/db/user.db
    APP_DB_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_DB_FILES

    # Add settings to Resources folder in macOS bundle
    APP_SETTINGS_FILES.files = resources/settings/settings.ini
    APP_SETTINGS_FILES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += APP_SETTINGS_FILES

    # Add licenses to Resources folder in macOS bundle
    FLOW_LAYOUT_LICENSE_FILES.files = resources/licenses/FLOW_LAYOUT_LICENSE.txt
    FLOW_LAYOUT_LICENSE_FILES.path = Contents/Resources/licenses
    QMAKE_BUNDLE_DATA += FLOW_LAYOUT_LICENSE_FILES

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
    SOURCES += logic/utils/utils_windows.cpp
    HEADERS += logic/utils/utils_windows.h

    CONFIG -= debug_and_release debug_and_release_target
    RC_ICONS = resources/icon/icon.ico
}

unix:!macx {
    SOURCES += logic/utils/utils_linux.cpp
    HEADERS += logic/utils/utils_linux.h

    # Move files to appropriate locations on desktop to install the program
    binfile.extra = cp \"$$system_path($$OUT_PWD)/Jyut Dictionary\" $$system_path($$OUT_PWD)/jyut-dict
    binfile.files += $$system_path($$OUT_PWD)/jyut-dict
    binfile.path = /usr/bin/
    binfile.CONFIG += no_check_exist
    dictfile.files += resources/db/dict.db
    dictfile.path = /usr/share/jyut-dict/dictionaries/
    userfile.files += resources/db/user.db
    userfile.path = /usr/share/jyut-dict/dictionaries/
    flowlayoutlicensefile.files += resources/licenses/FLOW_LAYOUT_LICENSE.txt
    flowlayoutlicensefile.path = /usr/share/jyut-dict/licenses/
    shortcutfiles.files += platform/linux/jyut-dict.desktop
    shortcutfiles.path = /usr/share/applications/
    icon.files += resources/icon/jyut-dict.svg
    icon.path = /usr/share/icons/hicolor/scalable/apps/
    INSTALLS += binfile
    INSTALLS += dictfile
    INSTALLS += userfile
    INSTALLS += flowlayoutlicensefile
    INSTALLS += shortcutfiles
    INSTALLS += icon
}

unix|win32:!macx {
    # Copy dictionary database to the build directory
    DICTFILES = $$files($${PWD}/resources/db/dict.db)
    DICTFILES += $$files($${PWD}/resources/db/user.db)

    copy_files.name = copy dictionary files
    copy_files.input = DICTFILES
    copy_files.output = $${OUT_PWD}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
    copy_files.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copy_files.CONFIG += no_link target_predeps

    QMAKE_EXTRA_COMPILERS += copy_files

    # Copy licenses to build directory
    LICENSEFILES = $$files($${PWD}/resources/licenses/FLOW_LAYOUT_LICENSE.txt)
    copy_licenses.name = copy licenses
    copy_licenses.input = LICENSEFILES
    copy_licenses.output = $${OUT_PWD}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
    copy_licenses.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copy_licenses.CONFIG += no_link target_predeps

    QMAKE_EXTRA_COMPILERS += copy_licenses

    # Copy settings file to build directory
    SETTINGSFILES = $$files($${PWD}/resources/settings/settings.ini)
    copy_settings.name = copy settings
    copy_settings.input = SETTINGSFILES
    copy_settings.output = $${OUT_PWD}/${QMAKE_FILE_BASE}${QMAKE_FILE_EXT}
    copy_settings.commands = ${COPY_FILE} ${QMAKE_FILE_IN} ${QMAKE_FILE_OUT}
    copy_settings.CONFIG += no_link target_predeps

    QMAKE_EXTRA_COMPILERS += copy_settings
}

DISTFILES += \
    resources/images/chevron-down_inverted.png \
    resources/images/chevron-up_inverted.png
