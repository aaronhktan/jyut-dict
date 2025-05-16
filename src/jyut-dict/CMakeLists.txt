cmake_minimum_required(VERSION 3.20)

project(CantoneseDictionary VERSION 3.25.0304 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS
    LinguistTools
    Multimedia
    Network
    Sql
    Svg
    TextToSpeech
    Widgets)
find_package(Qt6 REQUIRED COMPONENTS
    LinguistTools
    Multimedia
    Network
    Sql
    Svg
    TextToSpeech
    Widgets)
find_package(KF6Archive)

set(TS_FILES
    resources/translations/jyutdictionary-en.ts
    resources/translations/jyutdictionary-fr.ts
    resources/translations/jyutdictionary-fr_CA.ts
    resources/translations/jyutdictionary-yue_Hans.ts
    resources/translations/jyutdictionary-yue_Hant.ts
    resources/translations/jyutdictionary-zh_Hans.ts
    resources/translations/jyutdictionary-zh_Hant.ts
    resources/translations/jyutdictionary-zh_Hans_HK.ts
    resources/translations/jyutdictionary-zh_Hant_HK.ts)

set(PROJECT_HEADERS
        components/definitioncard/definitioncardsection.h
        components/definitioncard/definitioncardwidget.h
        components/definitioncard/definitioncontentwidget.h
        components/definitioncard/definitionheaderwidget.h
        components/definitioncard/icardheaderwidget.h
        components/dictionarylist/dictionarylistdelegate.h
        components/dictionarylist/dictionarylistmodel.h
        components/dictionarylist/dictionarylistview.h
        components/entrysearchresult/resultlistdelegate.h
        components/entrysearchresult/resultlistmodel.h
        components/entrysearchresult/resultlistview.h
        components/entryview/entryactionwidget.h
        components/entryview/entrycontentwidget.h
        components/entryview/entryheaderwidget.h
        components/entryview/entryscrollarea.h
        components/entryview/entryscrollareawidget.h
        components/entryview/entryviewsentencecardsection.h
        components/favouritewindow/favouritesplitter.h
        components/handwriting/handwritingpanel.h
        components/historyview/searchhistorylistdelegate.h
        components/historyview/searchhistorylistmodel.h
        components/historyview/searchhistorylistview.h
        components/historyview/searchhistorytab.h
        components/historyview/viewhistorylistdelegate.h
        components/historyview/viewhistorylistmodel.h
        components/historyview/viewhistorylistview.h
        components/historyview/viewhistorytab.h
        components/layouts/flowlayout.h
        components/magnifywindow/magnifyscrollarea.h
        components/magnifywindow/magnifyscrollareawidget.h
        components/mainwindow/isearchlineedit.h
        components/mainwindow/isearchoptionsselector.h
        components/mainwindow/mainsplitter.h
        components/mainwindow/maintoolbar.h
        components/mainwindow/searchlineedit.h
        components/mainwindow/searchoptionsradiogroupbox.h
        components/related/relatedbutton.h
        components/related/relatedbuttoncontentwidget.h
        components/related/relatedbuttonheaderwidget.h
        components/related/relatedsection.h
        components/sentencecard/loadingwidget.h
        components/sentencecard/sentencecardwidget.h
        components/sentencecard/sentencecontentwidget.h
        components/sentencecard/sentenceheaderwidget.h
        components/sentencesearchresult/sentenceresultlistdelegate.h
        components/sentencesearchresult/sentenceresultlistmodel.h
        components/sentencesearchresult/sentenceresultlistview.h
        components/sentenceview/sentencescrollarea.h
        components/sentenceview/sentencescrollareawidget.h
        components/sentenceview/sentenceviewcontentwidget.h
        components/sentenceview/sentenceviewheaderwidget.h
        components/sentenceview/sentenceviewsentencecardsection.h
        components/sentencewindow/sentencesplitter.h
        components/settings/advancedtab.h
        components/settings/contacttab.h
        components/settings/dictionarytab.h
        components/settings/searchtab.h
        components/settings/settingstab.h
        components/settings/texttab.h
        dialogs/defaultdialog.h
        dialogs/dictionarytabfailuredialog.h
        dialogs/downloadaudiodialog.h
        dialogs/downloadresultdialog.h
        dialogs/entryspeakerrordialog.h
        dialogs/exportdatabasedialog.h
        dialogs/noupdatedialog.h
        dialogs/overwriteconflictingdictionarydialog.h
        dialogs/resetsettingsdialog.h
        dialogs/restoredatabasedialog.h
        dialogs/transcriptionerrordialog.h
        logic/database/queryparseutils.h
        logic/database/sqldatabasemanager.h
        logic/database/sqldatabaseutils.h
        logic/database/sqluserdatautils.h
        logic/database/sqluserhistoryutils.h
        logic/dictation/iinputvolumepublisher.h
        logic/dictation/iinputvolumesubscriber.h
        logic/dictation/itranscriptionresultpublisher.h
        logic/dictation/itranscriptionresultsubscriber.h
        logic/dictation/transcriberwrapper.h
        logic/dictionary/dictionarymetadata.h
        logic/dictionary/dictionarysource.h
        logic/download/downloader.h
        logic/entry/definitionsset.h
        logic/entry/entry.h
        logic/entry/entryspeaker.h
        logic/handwriting/handwritingwrapper.h
        logic/search/isearch.h
        logic/search/isearchobservable.h
        logic/search/isearchobserver.h
        logic/search/isearchoptionsmediator.h
        logic/search/searchoptionsmediator.h
        logic/search/sqlsearch.h
        logic/sentence/sentenceset.h
        logic/sentence/sourcesentence.h
        logic/settings/settings.h
        logic/settings/settingsutils.h
        logic/update/jyutdictionaryreleasechecker.h
        logic/utils/cantoneseutils.h
        logic/utils/chineseutils.h
        logic/utils/mandarinutils.h
        logic/utils/scriptdetector.h
        logic/utils/utils.h
        logic/utils/utils_qt.h
        vendor/zinnia/svm.h
        vendor/zinnia/common.h
        vendor/zinnia/config.h
        vendor/zinnia/feature.h
        vendor/zinnia/freelist.h
        vendor/zinnia/mmap.h
        vendor/zinnia/param.h
        vendor/zinnia/scoped_ptr.h
        vendor/zinnia/sexp.h
        vendor/zinnia/stream_wrapper.h
        vendor/zinnia/zinnia.h
        windows/aboutwindow.h
        windows/handwritingwindow.h
        windows/historywindow.h
        windows/mainwindow.h
        windows/settingswindow.h
        windows/transcriptionwindow.h
        windows/updatewindow.h
        windows/welcomewindow.h
)

set(PROJECT_SOURCES
        components/definitioncard/definitioncardsection.cpp
        components/definitioncard/definitioncardwidget.cpp
        components/definitioncard/definitioncontentwidget.cpp
        components/definitioncard/definitionheaderwidget.cpp
        components/dictionarylist/dictionarylistdelegate.cpp
        components/dictionarylist/dictionarylistmodel.cpp
        components/dictionarylist/dictionarylistview.cpp
        components/entrysearchresult/resultlistdelegate.cpp
        components/entrysearchresult/resultlistmodel.cpp
        components/entrysearchresult/resultlistview.cpp
        components/entryview/entryactionwidget.cpp
        components/entryview/entrycontentwidget.cpp
        components/entryview/entryheaderwidget.cpp
        components/entryview/entryscrollarea.cpp
        components/entryview/entryscrollareawidget.cpp
        components/entryview/entryviewsentencecardsection.cpp
        components/favouritewindow/favouritesplitter.cpp
        components/handwriting/handwritingpanel.cpp
        components/historyview/searchhistorylistdelegate.cpp
        components/historyview/searchhistorylistmodel.cpp
        components/historyview/searchhistorylistview.cpp
        components/historyview/searchhistorytab.cpp
        components/historyview/viewhistorylistdelegate.cpp
        components/historyview/viewhistorylistmodel.cpp
        components/historyview/viewhistorylistview.cpp
        components/historyview/viewhistorytab.cpp
        components/layouts/flowlayout.cpp
        components/magnifywindow/magnifyscrollarea.cpp
        components/magnifywindow/magnifyscrollareawidget.cpp
        components/mainwindow/mainsplitter.cpp
        components/mainwindow/maintoolbar.cpp
        components/mainwindow/searchlineedit.cpp
        components/mainwindow/searchoptionsradiogroupbox.cpp
        components/related/relatedbutton.cpp
        components/related/relatedbuttoncontentwidget.cpp
        components/related/relatedbuttonheaderwidget.cpp
        components/related/relatedsection.cpp
        components/sentencecard/loadingwidget.cpp
        components/sentencecard/sentencecardwidget.cpp
        components/sentencecard/sentencecontentwidget.cpp
        components/sentencecard/sentenceheaderwidget.cpp
        components/sentencesearchresult/sentenceresultlistdelegate.cpp
        components/sentencesearchresult/sentenceresultlistmodel.cpp
        components/sentencesearchresult/sentenceresultlistview.cpp
        components/sentenceview/sentencescrollarea.cpp
        components/sentenceview/sentencescrollareawidget.cpp
        components/sentenceview/sentenceviewcontentwidget.cpp
        components/sentenceview/sentenceviewheaderwidget.cpp
        components/sentenceview/sentenceviewsentencecardsection.cpp
        components/sentencewindow/sentencesplitter.cpp
        components/settings/advancedtab.cpp
        components/settings/contacttab.cpp
        components/settings/dictionarytab.cpp
        components/settings/searchtab.cpp
        components/settings/settingstab.cpp
        components/settings/texttab.cpp
        dialogs/defaultdialog.cpp
        dialogs/dictionarytabfailuredialog.cpp
        dialogs/downloadaudiodialog.cpp
        dialogs/downloadresultdialog.cpp
        dialogs/entryspeakerrordialog.cpp
        dialogs/exportdatabasedialog.cpp
        dialogs/noupdatedialog.cpp
        dialogs/overwriteconflictingdictionarydialog.cpp
        dialogs/resetsettingsdialog.cpp
        dialogs/restoredatabasedialog.cpp
        dialogs/transcriptionerrordialog.cpp
        logic/database/queryparseutils.cpp
        logic/database/sqldatabasemanager.cpp
        logic/database/sqldatabaseutils.cpp
        logic/database/sqluserdatautils.cpp
        logic/database/sqluserhistoryutils.cpp
        logic/dictionary/dictionarymetadata.cpp
        logic/dictionary/dictionarysource.cpp
        logic/download/downloader.cpp
        logic/entry/definitionsset.cpp
        logic/entry/entry.cpp
        logic/entry/entryspeaker.cpp
        logic/handwriting/handwritingwrapper.cpp
        logic/search/searchoptionsmediator.cpp
        logic/search/sqlsearch.cpp
        logic/sentence/sentenceset.cpp
        logic/sentence/sourcesentence.cpp
        logic/settings/settings.cpp
        logic/settings/settingsutils.cpp
        logic/update/jyutdictionaryreleasechecker.cpp
        logic/utils/cantoneseutils.cpp
        logic/utils/chineseutils.cpp
        logic/utils/mandarinutils.cpp
        logic/utils/scriptdetector.cpp
        logic/utils/utils.cpp
        logic/utils/utils_qt.cpp
        main.cpp
        vendor/zinnia/character.cpp
        vendor/zinnia/feature.cpp
        vendor/zinnia/libzinnia.cpp
        vendor/zinnia/param.cpp
        vendor/zinnia/recognizer.cpp
        vendor/zinnia/sexp.cpp
        vendor/zinnia/svm.cpp
        vendor/zinnia/trainer.cpp
        windows/aboutwindow.cpp
        windows/handwritingwindow.cpp
        windows/historywindow.cpp
        windows/mainwindow.cpp
        windows/settingswindow.cpp
        windows/transcriptionwindow.cpp
        windows/updatewindow.cpp
        windows/welcomewindow.cpp
        ${TS_FILES}
)

include_directories(
    ${CMAKE_SOURCE_DIR}
)

if(UNIX AND NOT APPLE)
    list(REMOVE_ITEM PROJECT_HEADERS windows/transcriptionwindow.h)
    list(REMOVE_ITEM PROJECT_SOURCES windows/transcriptionwindow.cpp)
endif()

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(CantoneseDictionary
        MANUAL_FINALIZATION
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCES}
        resources/resource.qrc
    )

else()
    add_executable(CantoneseDictionary
        ${PROJECT_HEADERS}
        ${PROJECT_SOURCES}
        resources/resource.qrc
    )

endif()

target_link_libraries(CantoneseDictionary
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Multimedia
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
    PRIVATE Qt${QT_VERSION_MAJOR}::Svg
    PRIVATE Qt${QT_VERSION_MAJOR}::TextToSpeech
    PRIVATE KF6::Archive
)

add_subdirectory(logic/database/test/TestSqlDatabaseManager)
add_subdirectory(logic/database/test/TestSqlDatabaseUtils)
add_subdirectory(logic/database/test/TestSqlUserDataUtils)
add_subdirectory(logic/database/test/TestSqlUserHistoryUtils)
add_subdirectory(logic/entry/test/TestDefinitionsSet)
add_subdirectory(logic/entry/test/TestEntry)
add_subdirectory(logic/search/test/TestSqlSearch)
add_subdirectory(logic/sentence/test/TestSentenceSet)
add_subdirectory(logic/sentence/test/TestSourceSentence)
add_subdirectory(logic/settings/test/TestSettingsUtils)
add_subdirectory(logic/utils/test/TestCantoneseUtils)
add_subdirectory(logic/utils/test/TestChineseUtils)
add_subdirectory(logic/utils/test/TestMandarinUtils)
add_subdirectory(logic/utils/test/TestScriptDetector)

set_target_properties(CantoneseDictionary PROPERTIES
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/platform/mac/Info.plist
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(APPLE)
    find_library(SpeechFramework Speech)
    find_library(AVFoundationFramework AVFoundation)
    target_link_libraries(CantoneseDictionary PRIVATE "-framework AppKit")
    target_link_libraries(CantoneseDictionary
        PRIVATE
        ${SpeechFramework}
        ${AVFoundationFramework}
    )

    target_compile_options(CantoneseDictionary PRIVATE
        -fobjc-arc  # Enable ARC for Objective-C and Objective-C++ files
    )


    TARGET_SOURCES(CantoneseDictionary
        PRIVATE logic/dictation/transcriber_mac.h
        PRIVATE logic/dictation/transcriber_mac.mm
        PRIVATE logic/dictation/transcriberwrapper_mac.mm
        PRIVATE logic/utils/utils_mac.mm
        PRIVATE resources/db/dict.db
        PRIVATE resources/db/user.db
        PRIVATE resources/licenses/FLOW_LAYOUT_LICENSE.txt
        PRIVATE resources/settings/settings.ini
        PRIVATE resources/icon/icon.icns
        PRIVATE platform/mac/en.lproj/InfoPlist.strings
        PRIVATE platform/mac/fr.lproj/InfoPlist.strings
        PRIVATE platform/mac/yue_Hans.lproj/InfoPlist.strings
        PRIVATE platform/mac/yue_Hant.lproj/InfoPlist.strings
        PRIVATE platform/mac/zh_Hans.lproj/InfoPlist.strings
        PRIVATE platform/mac/zh_Hant.lproj/InfoPlist.strings
        PRIVATE vendor/zinnia/models/handwriting-zh_TW.model
    )

    set_property(SOURCE resources/db/dict.db PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
    set_property(SOURCE resources/db/user.db PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
    set_property(SOURCE resources/licenses/FLOW_LAYOUT_LICENSE.txt PROPERTY MACOSX_PACKAGE_LOCATION "Resources/Licenses")
    set_property(SOURCE resources/settings/settings.ini PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
    set_property(SOURCE resources/icon/icon.icns PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
    set_property(SOURCE platform/mac/en.lproj/InfoPlist.strings PROPERTY MACOSX_PACKAGE_LOCATION "Resources/en.lproj")
    set_property(SOURCE platform/mac/fr.lproj/InfoPlist.strings PROPERTY MACOSX_PACKAGE_LOCATION "Resources/fr.lproj")
    set_property(SOURCE platform/mac/yue_Hans.lproj/InfoPlist.strings PROPERTY MACOSX_PACKAGE_LOCATION "Resources/yue-Hans.lproj")
    set_property(SOURCE platform/mac/yue_Hant.lproj/InfoPlist.strings PROPERTY MACOSX_PACKAGE_LOCATION "Resources/yue-Hant.lproj")
    set_property(SOURCE platform/mac/zh_Hans.lproj/InfoPlist.strings PROPERTY MACOSX_PACKAGE_LOCATION "Resources/zh-hans.lproj")
    set_property(SOURCE platform/mac/zh_Hant.lproj/InfoPlist.strings PROPERTY MACOSX_PACKAGE_LOCATION "Resources/zh-hant.lproj")
    set_property(SOURCE vendor/zinnia/models/handwriting-zh_TW.model PROPERTY MACOSX_PACKAGE_LOCATION "Resources/Handwriting")
elseif(UNIX)
    TARGET_SOURCES(CantoneseDictionary PRIVATE logic/utils/utils_linux.cpp)
    configure_file(resources/db/dict.db dict.db COPYONLY)
    configure_file(resources/db/user.db user.db COPYONLY)
    configure_file(resources/licenses/FLOW_LAYOUT_LICENSE.txt FLOW_LAYOUT_LICENSE.txt COPYONLY)
    configure_file(resources/settings/settings.ini settings.ini COPYONLY)
elseif(WIN32)
    enable_language("RC")

    target_compile_definitions(CantoneseDictionary PRIVATE UNICODE _UNICODE _ATL_NO_AUTOMATIC_NAMESPACE)
    target_link_libraries(CantoneseDictionary
        PRIVATE
        ole32
        oleaut32
        uuid
        sapi
        imm32
    )

    TARGET_SOURCES(CantoneseDictionary
        PRIVATE logic/dictation/transcriber_windows.cpp
        PRIVATE logic/dictation/transcriber_windows.h
        PRIVATE logic/dictation/transcriberwrapper_windows.cpp
        PRIVATE logic/utils/utils_windows.cpp
        PRIVATE platform/windows/app.rc
    )

    configure_file(resources/db/dict.db dict.db COPYONLY)
    configure_file(resources/db/user.db user.db COPYONLY)
    configure_file(resources/licenses/FLOW_LAYOUT_LICENSE.txt FLOW_LAYOUT_LICENSE.txt COPYONLY)
    configure_file(resources/settings/settings.ini settings.ini COPYONLY)
endif()

install(TARGETS CantoneseDictionary
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(CantoneseDictionary)
endif()

