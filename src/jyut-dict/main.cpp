#include "windows/mainwindow.h"

#include <QApplication>

#if defined(Q_OS_WIN)
#include <vector>
#include <string>
#endif

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Aaron Tan");
    QCoreApplication::setOrganizationDomain("aaronhktan.com");
#ifdef APPIMAGE
    QCoreApplication::setApplicationName("Jyut Dictionary");
#else
    QCoreApplication::setApplicationName("CantoneseDictionary");
#endif

    // Prevents some UI lag
    QThreadPool::globalInstance()->setMaxThreadCount(16);

    qRegisterMetaType<EntryCharactersOptions>("EntryCharactersOptions");
    qRegisterMetaTypeStreamOperators<EntryCharactersOptions>("EntryCharactersOptions");
    qRegisterMetaType<EntryPhoneticOptions>("EntryPhoneticOptions");
    qRegisterMetaTypeStreamOperators<EntryPhoneticOptions>("EntryPhoneticOptions");
    qRegisterMetaType<CantoneseOptions>("CantoneseOptions");
    qRegisterMetaTypeStreamOperators<CantoneseOptions>("CantoneseOptions");
    qRegisterMetaType<MandarinOptions>("MandarinOptions");
    qRegisterMetaTypeStreamOperators<MandarinOptions>("MandarinOptions");
    qRegisterMetaType<EntryColourPhoneticType>("EntryColourPhoneticType");
    qRegisterMetaTypeStreamOperators<EntryColourPhoneticType>("EntryColourPhoneticType");
    qRegisterMetaType<SearchParameters>("SearchParameters");
    qRegisterMetaTypeStreamOperators<SearchParameters>("SearchParameters");
    qRegisterMetaType<searchTermHistoryItem>();
    qRegisterMetaType<conflictingDictionaryMetadata>(
        "conflictingDictionaryNamesMetadata");

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#if defined(Q_OS_WIN)
    // This is kind of a horrible hack to get dark borders on Windows
    // But it works!
    std::vector<char *> args{argv, argv + argc};
    char *platformArg = const_cast<char *>("-platform");
    char *darkModeArg = const_cast<char *>("windows:darkmode=1");
    args.emplace_back(platformArg);
    args.emplace_back(darkModeArg);
    argc += 2;

    QApplication a(argc, args.data());
#else
    QApplication a(argc, argv);
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
