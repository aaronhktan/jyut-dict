#include "windows/mainwindow.h"

#include "logic/settings/settings.h"

#include <QApplication>
#include <QSysInfo>

#if defined(Q_OS_WIN)
#include <Windows.h>

#include <cstdio>
#include <cstring>
#endif

int main(int argc, char *argv[])
{
    QApplication a{argc, argv};

    QCoreApplication::setOrganizationName("Aaron Tan");
    QCoreApplication::setOrganizationDomain("aaronhktan.com");
#ifdef APPIMAGE
    QCoreApplication::setApplicationName("Jyut Dictionary");
#else
    QCoreApplication::setApplicationName("CantoneseDictionary");
#endif

    // Prevents some UI lag
    QThreadPool::globalInstance()->setMaxThreadCount(16);

    // Required to marshal types to QVariant, so these values can be stored
    // by QSettings or used in a QListModel
    qRegisterMetaType<EntryCharactersOptions>("EntryCharactersOptions");
    qRegisterMetaType<EntryPhoneticOptions>("EntryPhoneticOptions");
    qRegisterMetaType<CantoneseOptions>("CantoneseOptions");
    qRegisterMetaType<MandarinOptions>("MandarinOptions");
    qRegisterMetaType<EntryColourPhoneticType>("EntryColourPhoneticType");
    qRegisterMetaType<SearchParameters>("SearchParameters");
    qRegisterMetaType<Handwriting::Script>("HandwritingScript");
#ifndef Q_OS_LINUX
    qRegisterMetaType<TranscriptionLanguage>("TranscriptionLanguage");
#endif
    qRegisterMetaType<searchTermHistoryItem>("searchTermHistoryItem");
    qRegisterMetaType<conflictingDictionaryMetadata>(
        "conflictingDictionaryNamesMetadata");
    qRegisterMetaType<Settings::InterfaceSize>();
    qRegisterMetaType<TextToSpeech::SpeakerBackend>();
    qRegisterMetaType<TextToSpeech::SpeakerVoice>();

#if defined(Q_OS_WIN)
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr)) {
        std::cerr << "Failed to initialize COM library." << std::endl;
        return -1;
    }
    qputenv("QT_QPA_PLATFORM", "windows:darkmode=1");
#endif

    MainWindow w;
    w.show();

    int res = a.exec();

#ifdef Q_OS_WIN
    CoUninitialize();
#endif

    return res;
}
