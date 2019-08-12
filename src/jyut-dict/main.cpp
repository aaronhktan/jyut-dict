#include "windows/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Aaron Tan");
    QCoreApplication::setOrganizationDomain("aaronhktan.com");
    QCoreApplication::setApplicationName("CantoneseDictionary");

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

    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    a.installTranslator(&qtTranslator);

    // The reason why we need to loop through UI languages is because
    // if we just use QLocale::system() and the language is zh_Hant_HK,
    // zh_Hant will always be prioritized over zh_HK.

    // By creating a locale with the language being zh_Hant_HK, the correct
    // translation file is loaded.

    // Another example of QLocale::system() failing is having simplified
    // Cantonese as the UI language. QTranslator::load() will try to load
    // zh_Hant_HK instead of yue_Hans.
    QTranslator jyutTranslator;
    for (auto language : QLocale::system().uiLanguages()) {
        QLocale locale{language};
        qDebug() << locale.uiLanguages()[0];
        if (jyutTranslator.load(/* QLocale */    locale,
                                /* filename */   "jyutdictionary",
                                /* prefix */     "-",
                                /* directory */  ":/translations")) {
            a.installTranslator(&jyutTranslator);
            break;
        }
    }

    MainWindow w;
    w.show();

    return a.exec();
}
