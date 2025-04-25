#include "settingsutils.h"

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"

#include <QCoreApplication>

namespace Settings
{

QTranslator systemTranslator;
QTranslator applicationTranslator;

QLocale currentLocale;

std::unique_ptr<QSettings> getSettings(QObject *parent)
{
#ifdef Q_OS_MAC
    QString settingsPath = QCoreApplication::applicationDirPath() + "/../Resources/settings.ini";
#elif defined(Q_OS_WIN)
    QString settingsPath = QCoreApplication::applicationDirPath() + "./settings.ini";
#elif defined(Q_OS_LINUX)
    QString settingsPath = QCoreApplication::applicationDirPath() + "/settings.ini";
#endif

    std::unique_ptr<QSettings> settings;

#ifdef PORTABLE
    settings = std::make_unique<QSettings>(settingsPath,
                                           QSettings::IniFormat,
                                           parent);
#else
    settings = std::make_unique<QSettings>(parent);
#endif

    updateSettings(*settings);

    return settings;
}

bool updateSettings(QSettings &settings)
{
    int fileSettingsVersion = settings.value("Metadata/version", QVariant{0})
                                  .toInt();
    if (fileSettingsVersion != SETTINGS_VERSION) {
        if (fileSettingsVersion == 0) {
            // Due to an oversight, a metadata version was never written to file
            // for version 0 or version 1.
            // However, they are substantially the same, so it is safe to
            // migrate to version 2 with the same codepath.
            migrateSettingsFromOneToTwo(settings);
        }
        settings.setValue("Metadata/version", QVariant{SETTINGS_VERSION});
        settings.sync();
    }
    return true;
}

bool clearSettings(QSettings &settings)
{
    settings.clear();
    settings.setValue("Metadata/version", QVariant{SETTINGS_VERSION});
    settings.sync();
    return true;
}

bool migrateSettingsFromOneToTwo(QSettings &settings)
{
    // Due to an oversight, previous versions of settings stored the character
    // set option as an EntryPhoneticOption instead of an EntryCharactersOptions
    if (settings.contains("characterOptions")) {
        EntryPhoneticOptions oldCharacterOptions
            = settings.value("characterOptions").value<EntryPhoneticOptions>();

        EntryCharactersOptions newCharacterOptions
            = static_cast<EntryCharactersOptions>(oldCharacterOptions);
        QVariant newCharactersOptionsVariant
            = QVariant::fromValue<EntryCharactersOptions>(newCharacterOptions);

        settings.remove("characterOptions");
        settings.sync();
        settings.setValue("characterOptions", newCharactersOptionsVariant);
        settings.sync();
    }

    if (settings.contains("phoneticOptions")) {
        EntryPhoneticOptions oldPhoneticOptions
            = settings.value("phoneticOptions").value<EntryPhoneticOptions>();
        QVariant oldPhoneticOptionVariant = QVariant::fromValue(
            oldPhoneticOptions);

        // Since the MandarinOptions::NONE option was added, everything needs to
        // be shifted down.
        MandarinOptions oldMandarinOptions
            = settings
                  .value("mandarinOptions",
                         QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
                  .value<MandarinOptions>();
        MandarinOptions newMandarinOptions = oldMandarinOptions;
        if (static_cast<int>(oldMandarinOptions) == 0) {
            newMandarinOptions = MandarinOptions::NUMBERED_PINYIN;
        } else if (static_cast<int>(oldMandarinOptions) == 1) {
            newMandarinOptions = MandarinOptions::PRETTY_PINYIN;
        }
        QVariant newMandarinOptionVariant = QVariant::fromValue(
            newMandarinOptions);

        settings.setValue("Preview/phoneticOptions", oldPhoneticOptionVariant);
        settings.setValue("Preview/cantonesePronunciationOptions",
                          QVariant::fromValue(CantoneseOptions::RAW_JYUTPING));
        settings.setValue("Preview/mandarinPronunciationOptions",
                          newMandarinOptionVariant);

        // The only option in the old settings that would hide Cantonese
        // pronunciation is EntryPhoneticOptions::ONLY_MANDARIN.
        if (oldPhoneticOptions == EntryPhoneticOptions::ONLY_MANDARIN) {
            settings.setValue("Entry/cantonesePronunciationOptions",
                              QVariant::fromValue(CantoneseOptions::NONE));
        } else {
            settings.setValue("Entry/cantonesePronunciationOptions",
                              QVariant::fromValue(
                                  CantoneseOptions::RAW_JYUTPING));
        }

        // The only option in the old settings that would hide Mandarin
        // pronunciation is EntryPhoneticOptions::ONLY_CANTONESE
        if (oldPhoneticOptions == EntryPhoneticOptions::ONLY_CANTONESE) {
            settings.setValue("Entry/mandarinPronunciationOptions",
                              QVariant::fromValue(MandarinOptions::NONE));
        } else {
            settings.setValue("Entry/mandarinPronunciationOptions",
                              newMandarinOptionVariant);
        }
    }

    settings.setValue("Metadata/version", QVariant{SETTINGS_VERSION});
    settings.sync();

    return true;
}

bool migrateSettingsFromTwoToThree(QSettings &settings)
{
    if (settings.contains("Interface/searchAutoDetect")) {
        settings.setValue("Search/Search/autoDetectLanguage",
                          settings.value("Interface/searchAutoDetect"));
        settings.remove("Interface/searchAutoDetect");
    }

    return true;
}

QLocale getCurrentLocale()
{
    return currentLocale;
}

bool setCurrentLocale(const QLocale &locale)
{
    currentLocale = locale;
    return true;
}

bool isCurrentLocaleHan()
{
    return currentLocale.script() == QLocale::HanScript
           || currentLocale.script() == QLocale::SimplifiedHanScript
           || currentLocale.script() == QLocale::TraditionalHanScript
           || currentLocale.language() == QLocale::Chinese
           || currentLocale.language() == QLocale::Cantonese;
}

bool isCurrentLocaleTraditionalHan() {
    return currentLocale.script() == QLocale::TraditionalHanScript;
}

bool isCurrentLocaleSimplifiedHan() {
    return currentLocale.script() == QLocale::SimplifiedHanScript;
}

std::string getCurrentLocaleLanguageAndScriptIfChinese() {
    if (isCurrentLocaleHan()) {
        return currentLocale.bcp47Name().toStdString();
    } else {
        return currentLocale.bcp47Name().toStdString().substr(0,
            currentLocale.bcp47Name().toStdString().find_first_of("-"));
    }
}

}
