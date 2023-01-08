#include "windows/mainwindow.h"

#include <QApplication>

#if defined(Q_OS_WIN)
#include <cstdio>
#include <cstring>

constexpr auto ARG_STR_LEN = 64;
constexpr auto platformArg = "-platform";
constexpr auto darkModeArg = "windows:darkmode=1";
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

    // Required to marshal types to QVariant, so these values can be stored
    // by QSettings or used in a QListModel
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
    qRegisterMetaType<Settings::InterfaceSize>();

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);

#if defined(Q_OS_WIN)
    // This is kind of a horrible hack to get dark borders on Windows
    // But it works!
    int new_argc = argc + 2;
    char **new_argv = static_cast<char **>(realloc(argv, static_cast<size_t>(new_argc)));
    char new_platform_arg[ARG_STR_LEN];
    snprintf(new_platform_arg, ARG_STR_LEN, platformArg);
    new_argv[argc] = new_platform_arg;
    char new_dark_mode_arg[ARG_STR_LEN];
    snprintf(new_dark_mode_arg, ARG_STR_LEN, darkModeArg);
    new_argv[argc+1] = new_dark_mode_arg;

    QApplication a{new_argc, new_argv};
#else
    QApplication a{argc, argv};
#endif

    MainWindow w;
    w.show();

    return a.exec();
}
