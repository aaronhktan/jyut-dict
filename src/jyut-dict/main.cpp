#include "windows/mainwindow.h"

#include <QApplication>

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
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    return a.exec();
}
