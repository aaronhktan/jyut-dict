#include <QtTest>

#include "logic/entry/entrycharactersoptions.h"
#include "logic/entry/entryphoneticoptions.h"
#include "logic/settings/settingsutils.h"

#include <memory>

class TestSettingsUtils : public QObject
{
    Q_OBJECT

public:
    TestSettingsUtils();
    ~TestSettingsUtils();

private slots:
    void noSettingsUpdate();
    void clearSettings();

    void migrateCantoneseOnlyOneToTwo();
    void migrateMandarinOnlyOneToTwo();
    void migratePreferCantoneseOneToTwo();
    void migratePreferMandarinOneToTwo();
};

TestSettingsUtils::TestSettingsUtils()
{
    qRegisterMetaType<EntryCharactersOptions>("EntryCharactersOptions");
    qRegisterMetaTypeStreamOperators<EntryCharactersOptions>(
        "EntryCharactersOptions");
    qRegisterMetaType<EntryPhoneticOptions>("EntryPhoneticOptions");
    qRegisterMetaTypeStreamOperators<EntryPhoneticOptions>(
        "EntryPhoneticOptions");
    qRegisterMetaTypeStreamOperators<CantoneseOptions>("CantoneseOptions");
    qRegisterMetaType<MandarinOptions>("MandarinOptions");
    qRegisterMetaTypeStreamOperators<MandarinOptions>("MandarinOptions");
    qRegisterMetaType<EntryColourPhoneticType>("EntryColourPhoneticType");
}

TestSettingsUtils::~TestSettingsUtils() {}

void TestSettingsUtils::noSettingsUpdate()
{
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    settings->clear();
    settings->setValue("Metadata/version", QVariant{Settings::SETTINGS_VERSION});
    settings->sync();

    Settings::updateSettings(*settings);
    QCOMPARE(settings->value("Metadata/version"),
             QVariant{Settings::SETTINGS_VERSION});
    settings->clear();
    settings->sync();
}

void TestSettingsUtils::clearSettings()
{
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    Settings::clearSettings(*settings);

    QCOMPARE(settings->value("Metadata/version"),
             QVariant{Settings::SETTINGS_VERSION});

    settings->clear();
    settings->sync();
}

void TestSettingsUtils::migrateCantoneseOnlyOneToTwo()
{
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    settings->clear();
    settings->sync();

    settings->setValue("characterOptions",
                       QVariant::fromValue(
                           EntryPhoneticOptions::ONLY_CANTONESE));
    settings->setValue("phoneticOptions",
                       QVariant::fromValue(
                           EntryPhoneticOptions::ONLY_CANTONESE));
    settings->sync();

    Settings::migrateSettingsFromOneToTwo(*settings);

    QCOMPARE(settings->value("Metadata/version"), QVariant{2});
    QCOMPARE(settings->value("characterOptions"),
             QVariant::fromValue(static_cast<EntryCharactersOptions>(
                 EntryPhoneticOptions::ONLY_CANTONESE)));
    QCOMPARE(settings->value("Preview/phoneticOptions"),
             QVariant::fromValue(EntryPhoneticOptions::ONLY_CANTONESE));
    QCOMPARE(settings->value("Entry/cantonesePronunciationOptions"),
             QVariant::fromValue(CantoneseOptions::RAW_JYUTPING));
    QCOMPARE(settings->value("Entry/mandarinPronunciationOptions"),
             QVariant::fromValue(MandarinOptions::NONE));

    settings->clear();
    settings->sync();
}

void TestSettingsUtils::migrateMandarinOnlyOneToTwo()
{
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    settings->clear();
    settings->sync();

    settings->setValue("characterOptions",
                       QVariant::fromValue(EntryPhoneticOptions::ONLY_MANDARIN));
    settings->setValue("phoneticOptions",
                       QVariant::fromValue(EntryPhoneticOptions::ONLY_MANDARIN));
    settings->sync();

    Settings::migrateSettingsFromOneToTwo(*settings);

    QCOMPARE(settings->value("Metadata/version"), QVariant{2});
    QCOMPARE(settings->value("characterOptions"),
             QVariant::fromValue(static_cast<EntryCharactersOptions>(
                 EntryPhoneticOptions::ONLY_MANDARIN)));
    QCOMPARE(settings->value("Preview/phoneticOptions"),
             QVariant::fromValue(EntryPhoneticOptions::ONLY_MANDARIN));
    QCOMPARE(settings->value("Entry/cantonesePronunciationOptions"),
             QVariant::fromValue(CantoneseOptions::NONE));
    QCOMPARE(settings->value("Entry/mandarinPronunciationOptions"),
             QVariant::fromValue(MandarinOptions::PRETTY_PINYIN));

    settings->clear();
    settings->sync();
}

void TestSettingsUtils::migratePreferCantoneseOneToTwo()
{
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    settings->clear();
    settings->sync();

    settings->setValue("characterOptions",
                       QVariant::fromValue(
                           EntryPhoneticOptions::PREFER_CANTONESE));
    settings->setValue("phoneticOptions",
                       QVariant::fromValue(
                           EntryPhoneticOptions::PREFER_CANTONESE));
    settings->sync();

    Settings::migrateSettingsFromOneToTwo(*settings);

    QCOMPARE(settings->value("Metadata/version"), QVariant{2});
    QCOMPARE(settings->value("characterOptions"),
             QVariant::fromValue(static_cast<EntryCharactersOptions>(
                 EntryPhoneticOptions::PREFER_CANTONESE)));
    QCOMPARE(settings->value("Preview/phoneticOptions"),
             QVariant::fromValue(EntryPhoneticOptions::PREFER_CANTONESE));
    QCOMPARE(settings->value("Entry/cantonesePronunciationOptions"),
             QVariant::fromValue(CantoneseOptions::RAW_JYUTPING));
    QCOMPARE(settings->value("Entry/mandarinPronunciationOptions"),
             QVariant::fromValue(MandarinOptions::PRETTY_PINYIN));

    settings->clear();
    settings->sync();
}

void TestSettingsUtils::migratePreferMandarinOneToTwo()
{
    std::unique_ptr<QSettings> settings = Settings::getSettings();
    settings->clear();
    settings->sync();

    settings->setValue("characterOptions",
                       QVariant::fromValue(
                           EntryPhoneticOptions::PREFER_MANDARIN));
    settings->setValue("phoneticOptions",
                       QVariant::fromValue(
                           EntryPhoneticOptions::PREFER_MANDARIN));
    settings->sync();

    Settings::migrateSettingsFromOneToTwo(*settings);

    QCOMPARE(settings->value("Metadata/version"), QVariant{2});
    QCOMPARE(settings->value("characterOptions"),
             QVariant::fromValue(static_cast<EntryCharactersOptions>(
                 EntryPhoneticOptions::PREFER_MANDARIN)));
    QCOMPARE(settings->value("Preview/phoneticOptions"),
             QVariant::fromValue(EntryPhoneticOptions::PREFER_MANDARIN));
    QCOMPARE(settings->value("Entry/cantonesePronunciationOptions"),
             QVariant::fromValue(CantoneseOptions::RAW_JYUTPING));
    QCOMPARE(settings->value("Entry/mandarinPronunciationOptions"),
             QVariant::fromValue(MandarinOptions::PRETTY_PINYIN));

    settings->clear();
    settings->sync();
}

QTEST_MAIN(TestSettingsUtils)

#include "tst_settingsutils.moc"
