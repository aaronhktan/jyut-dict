#include <QtTest>

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/settings/settings.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/utils.h"

class TestEntry : public QObject
{
    Q_OBJECT

public:
    TestEntry();
    ~TestEntry();

private slots:
    void getCharactersSimpleCantonese();
    void getCharactersSimpleMandarin();
    void getCharactersColourCantonese();
    void getCharactersColourMandarin();
    void getCharactersSimpleNone();

    void traditionalGetSet();
    void simplifiedGetSet();
    void jyutpingGetSet();
    void pinyinGetSet();

    void getPhoneticCantonese();
    void getPhoneticMandarin();

    void getJyutpingNumbersSimple();
    void getJyutpingNumbersInvalidTones();
    void getJyutpingNumbersResetJyutping();

    void getPinyinNumbersSimple();
    void getPinyinNumbersInvalidTones();
    void getPinyinNumbersResetPinyin();

    void getDefinitionsSetsSimple();
    void modifyDefinitionsSetsSimple();

    void getDefinitionSnippetSimple();
    void getDefinitionSnippetEmptyDefinitionsSet();
    void getDefintionSnippetEmptyDefinition();

    void specialCases();
};

TestEntry::TestEntry() {}

TestEntry::~TestEntry() {}

void TestEntry::getCharactersSimpleCantonese()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(
                 traditional + " ["
                 + ChineseUtils::compareStrings(traditional, simplified) + "]"));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(
                 simplified + " ["
                 + ChineseUtils::compareStrings(simplified, traditional) + "]"));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_TRADITIONAL,
                 /* useColours = */ false,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_TRADITIONAL,
                 /* useColours = */ false,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_SIMPLIFIED,
                 /* useColours = */ false,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_SIMPLIFIED,
                 /* useColours = */ false,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
}

void TestEntry::getCharactersSimpleMandarin()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(
                 traditional + " ["
                 + ChineseUtils::compareStrings(traditional, simplified) + "]"));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED,
                                     /* useColours = */ false,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(
                 simplified + " ["
                 + ChineseUtils::compareStrings(simplified, traditional) + "]"));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_TRADITIONAL,
                 /* useColours = */ false,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_TRADITIONAL,
                 /* useColours = */ false,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_SIMPLIFIED,
                 /* useColours = */ false,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_SIMPLIFIED,
                 /* useColours = */ false,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
}

void TestEntry::getCharactersColourCantonese()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    std::string expectedTraditional = "<font color=\""
                                      + Settings::defaultJyutpingToneColours[1]
                                      + "\">清</font><font color=\""
                                      + Settings::defaultJyutpingToneColours[5]
                                      + "\">遠</font>";
    std::string expectedSimplified = "<font color=\""
                                     + Settings::defaultJyutpingToneColours[1]
                                     + "\">清</font><font color=\""
                                     + Settings::defaultJyutpingToneColours[5]
                                     + "\">远</font>";
    std::string expectedPreferTraditional
        = "<font color=\"" + Settings::defaultJyutpingToneColours[1]
          + "\">清</font><font color=\""
          + Settings::defaultJyutpingToneColours[5] + "\">遠</font> ["
          + Utils::SAME_CHARACTER_STRING + "<font color=\""
          + Settings::defaultJyutpingToneColours[5] + +"\">远</font>]";
    std::string expectedPreferSimplified
        = "<font color=\"" + Settings::defaultJyutpingToneColours[1]
          + "\">清</font><font color=\""
          + Settings::defaultJyutpingToneColours[5] + "\">远</font> ["
          + Utils::SAME_CHARACTER_STRING + "<font color=\""
          + Settings::defaultJyutpingToneColours[5] + +"\">遠</font>]";

    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedTraditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedSimplified));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedPreferTraditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedPreferSimplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_TRADITIONAL,
                 /* useColours = */ true,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedTraditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_TRADITIONAL,
                 /* useColours = */ true,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedTraditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_SIMPLIFIED,
                 /* useColours = */ true,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedSimplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_SIMPLIFIED,
                 /* useColours = */ true,
                 EntryColourPhoneticType::CANTONESE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedSimplified));
}

void TestEntry::getCharactersColourMandarin()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    std::string expectedTraditional = "<font color=\""
                                      + Settings::defaultPinyinToneColours[1]
                                      + "\">清</font><font color=\""
                                      + Settings::defaultPinyinToneColours[3]
                                      + "\">遠</font>";
    std::string expectedSimplified = "<font color=\""
                                     + Settings::defaultPinyinToneColours[1]
                                     + "\">清</font><font color=\""
                                     + Settings::defaultPinyinToneColours[3]
                                     + "\">远</font>";
    std::string expectedPreferTraditional
        = "<font color=\"" + Settings::defaultPinyinToneColours[1]
          + "\">清</font><font color=\"" + Settings::defaultPinyinToneColours[3]
          + "\">遠</font> [" + Utils::SAME_CHARACTER_STRING + "<font color=\""
          + Settings::defaultPinyinToneColours[3] + +"\">远</font>]";
    std::string expectedPreferSimplified
        = "<font color=\"" + Settings::defaultPinyinToneColours[1]
          + "\">清</font><font color=\"" + Settings::defaultPinyinToneColours[3]
          + "\">远</font> [" + Utils::SAME_CHARACTER_STRING + "<font color=\""
          + Settings::defaultPinyinToneColours[3] + +"\">遠</font>]";

    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedTraditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedSimplified));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedPreferTraditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::MANDARIN,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedPreferSimplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_TRADITIONAL,
                 /* useColours = */ true,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedTraditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_TRADITIONAL,
                 /* useColours = */ true,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedTraditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_SIMPLIFIED,
                 /* useColours = */ true,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedSimplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_SIMPLIFIED,
                 /* useColours = */ true,
                 EntryColourPhoneticType::MANDARIN,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(expectedSimplified));
}

void TestEntry::getCharactersSimpleNone()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::NONE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::NONE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::NONE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(
                 traditional + " ["
                 + ChineseUtils::compareStrings(traditional, simplified) + "]"));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::NONE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(
                 simplified + " ["
                 + ChineseUtils::compareStrings(simplified, traditional) + "]"));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_TRADITIONAL,
                 /* useColours = */ true,
                 EntryColourPhoneticType::NONE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_TRADITIONAL,
                 /* useColours = */ true,
                 EntryColourPhoneticType::NONE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_SIMPLIFIED,
                 /* useColours = */ true,
                 EntryColourPhoneticType::NONE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_SIMPLIFIED,
                 /* useColours = */ true,
                 EntryColourPhoneticType::NONE,
                 Settings::jyutpingToneColours,
                 Settings::pinyinToneColours)),
             QString::fromStdString(simplified));
}

void TestEntry::traditionalGetSet()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    std::string expectedTraditional = "<font color=\""
                                      + Settings::defaultJyutpingToneColours[1]
                                      + "\">清</font><font color=\""
                                      + Settings::defaultJyutpingToneColours[5]
                                      + "\">遠</font>";
    QCOMPARE(QString::fromStdString(entry.getTraditional()),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedTraditional));

    std::string newTraditional{"廣州"};
    std::string expectedNewTraditional
        = "<font color=\"" + Settings::defaultJyutpingToneColours[2]
          + "\">廣</font><font color=\""
          + Settings::defaultJyutpingToneColours[1] + "\">州</font>";
    entry.setTraditional(newTraditional);
    entry.setJyutping("gwong2 zau1");
    QCOMPARE(QString::fromStdString(entry.getTraditional()),
             QString::fromStdString(newTraditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedNewTraditional));
}

void TestEntry::simplifiedGetSet()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    std::string expectedSimplified = "<font color=\""
                                     + Settings::defaultJyutpingToneColours[1]
                                     + "\">清</font><font color=\""
                                     + Settings::defaultJyutpingToneColours[5]
                                     + "\">远</font>";
    QCOMPARE(QString::fromStdString(entry.getTraditional()),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedSimplified));

    std::string newSimplified{"广州"};
    std::string expectedNewSimplified
        = "<font color=\"" + Settings::defaultJyutpingToneColours[2]
          + "\">广</font><font color=\""
          + Settings::defaultJyutpingToneColours[1] + "\">州</font>";
    entry.setSimplified(newSimplified);
    entry.setJyutping("gwong2 zau1");
    QCOMPARE(QString::fromStdString(entry.getSimplified()),
             QString::fromStdString(newSimplified));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ true,
                                     EntryColourPhoneticType::CANTONESE,
                                     Settings::jyutpingToneColours,
                                     Settings::pinyinToneColours)),
             QString::fromStdString(expectedNewSimplified));
}

void TestEntry::jyutpingGetSet()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(entry.getJyutping()),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(jyutping));
    std::string yale{"chīng yúhn"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::PRETTY_YALE)),
             QString::fromStdString(yale));
#ifdef Q_OS_MAC
    std::string cantoneseIPA{"t͡sʰɪŋ ˥  jyːn ˩ ˧"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA)),
             QString::fromStdString(cantoneseIPA));
#elif defined(Q_OS_LINUX)
    std::string cantoneseIPA{"t͡sʰɪŋ˥  jyːn˩˧"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA)),
             QString::fromStdString(cantoneseIPA));
#endif

    std::string newSimplified{"广州"};
    entry.setSimplified(newSimplified);
    std::string newJyutping{"gWoNg2 zau1"};
    std::string normalizedJyutping{"gwong2 zau1"};
    entry.setJyutping(newJyutping);
    QCOMPARE(QString::fromStdString(entry.getJyutping()),
             QString::fromStdString(normalizedJyutping));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(normalizedJyutping));
    std::string newYale{"gwóng jāu"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::PRETTY_YALE)),
             QString::fromStdString(newYale));
#ifdef Q_OS_MAC
    std::string newCantoneseIPA{"kʷɔːŋ ˧ ˥  t͡sɐu̯ ˥"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA)),
             QString::fromStdString(newCantoneseIPA));
#elif defined(Q_OS_LINUX)
    std::string newCantoneseIPA{"kʷɔːŋ˧˥  t͡sɐu̯˥"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA)),
             QString::fromStdString(newCantoneseIPA));
#endif
}

void TestEntry::pinyinGetSet()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(entry.getPinyin()),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(pinyin));
    std::string prettyPinyin{"qīng yuǎn"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::PRETTY_PINYIN)),
             QString::fromStdString(prettyPinyin));
    std::string numberedPinyin{"qing1 yuan3"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::NUMBERED_PINYIN)),
             QString::fromStdString(numberedPinyin));
    std::string zhuyin{"ㄑㄧㄥ ㄩㄢˇ"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::ZHUYIN)),
             QString::fromStdString(zhuyin));
#ifdef Q_OS_MAC
    std::string mandarinIPA{"t͡ɕʰiŋ ˥ ˥  ɥɛn ˨ ˩ ˦ ꜕ ꜖ ( ꜓ )"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::MANDARIN_IPA)),
             QString::fromStdString(mandarinIPA));
#elif defined(Q_OS_LINUX)
    std::string mandarinIPA{"t͡ɕʰiŋ˥˥  ɥɛn˨˩˦꜕꜖(꜓)"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::MANDARIN_IPA)),
             QString::fromStdString(mandarinIPA));
#endif

    std::string newSimplified{"广州"};
    entry.setSimplified(newSimplified);
    std::string newPinyin{"gUaNg3 ZhOu1"};
    std::string normalizedPinyin{"guang3 zhou1"};
    entry.setPinyin(newPinyin);
    QCOMPARE(QString::fromStdString(entry.getPinyin()),
             QString::fromStdString(normalizedPinyin));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(normalizedPinyin));
    std::string newPrettyPinyin{"guǎng zhōu"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::PRETTY_PINYIN)),
             QString::fromStdString(newPrettyPinyin));
    std::string newNumberedPinyin{"guang3 zhou1"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::NUMBERED_PINYIN)),
             QString::fromStdString(newNumberedPinyin));
    std::string newZhuyin{"ㄍㄨㄤˇ ㄓㄡ"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::ZHUYIN)),
             QString::fromStdString(newZhuyin));
#ifdef Q_OS_MAC
    std::string newMandarinIPA{"kwɑŋ ˨ ˩ ˦ ꜕ ꜖ ꜖  ʈ͡ʂoʊ̯ ˥ ˥"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::MANDARIN_IPA)),
             QString::fromStdString(newMandarinIPA));
#elif defined(Q_OS_LINUX)
    std::string newMandarinIPA{"kwɑŋ˨˩˦꜕꜖꜖  ʈ͡ʂoʊ̯˥˥"};
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::MANDARIN_IPA)),
             QString::fromStdString(newMandarinIPA));
#endif
}

void TestEntry::getPhoneticCantonese()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(entry.getJyutping()),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::PRETTY_YALE,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString("chīng yúhn (qing1 yuan3)"));
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString("t͡sʰɪŋ ˥  jyːn ˩ ˧ (qing1 yuan3)"));
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString("t͡sʰɪŋ˥  jyːn˩˧ (qing1 yuan3)"));
#endif

    entry.setPinyin("");
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::PRETTY_YALE,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString("chīng yúhn"));
    entry.setJyutping("");
    entry.setPinyin("qing1 yuan3");
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString("(qing1 yuan3)"));
}

void TestEntry::getPhoneticMandarin()
{
    std::string traditional{"清遠"};
    std::string simplified{"清远"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(entry.getPinyin()),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                   CantoneseOptions::RAW_JYUTPING,
                                   MandarinOptions::PRETTY_PINYIN)),
             QString::fromStdString("qīng yuǎn (cing1 jyun5)"));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                   CantoneseOptions::RAW_JYUTPING,
                                   MandarinOptions::ZHUYIN)),
             QString::fromStdString("ㄑㄧㄥ ㄩㄢˇ (cing1 jyun5)"));

#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                   CantoneseOptions::RAW_JYUTPING,
                                   MandarinOptions::MANDARIN_IPA)),
             QString::fromStdString(
                 "t͡ɕʰiŋ ˥ ˥  ɥɛn ˨ ˩ ˦ ꜕ ꜖ ( ꜓ ) (cing1 jyun5)"));
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                   CantoneseOptions::RAW_JYUTPING,
                                   MandarinOptions::MANDARIN_IPA)),
             QString::fromStdString("t͡ɕʰiŋ˥˥  ɥɛn˨˩˦꜕꜖(꜓) (cing1 jyun5)"));
#endif

    entry.setJyutping("");
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                   CantoneseOptions::RAW_JYUTPING,
                                   MandarinOptions::NUMBERED_PINYIN)),
             QString::fromStdString("qing1 yuan3"));
    entry.setPinyin("");
    entry.setJyutping("cing1 jyun5");
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                   CantoneseOptions::RAW_JYUTPING,
                                   MandarinOptions::RAW_PINYIN)),
             QString::fromStdString("(cing1 jyun5)"));
}

void TestEntry::getJyutpingNumbersSimple()
{
    std::string jyutping = "toi4 saan1 waa2";
    Entry entry{"", "", jyutping, "", {}};

    auto result = entry.getJyutpingNumbers();
    std::vector<uint8_t> expected{4, 1, 2};
    QCOMPARE(result, expected);

    entry.setJyutping("mei5 gwok3");
    result = entry.getJyutpingNumbers();
    expected = {5, 3};
    QCOMPARE(result, expected);

    entry.setJyutping("gaa1naa4 daai6");
    result = entry.getJyutpingNumbers();
    expected = {1, 4, 6};
    QCOMPARE(result, expected);
}

void TestEntry::getJyutpingNumbersInvalidTones()
{
    std::string jyutping = "toi4 saan1 saan7 waa2";
    Entry entry{"", "", jyutping, "", {}};

    auto result = entry.getJyutpingNumbers();
    std::vector<uint8_t> expected{4, 1, 2};
    QCOMPARE(result, expected);
}

void TestEntry::getJyutpingNumbersResetJyutping()
{
    std::string jyutping = "toi4 saan1 waa2";
    Entry entry{"", "", jyutping, "", {}};

    auto result = entry.getJyutpingNumbers();
    std::vector<uint8_t> expected{4, 1, 2};
    QCOMPARE(result, expected);

    entry.setJyutping("gaa1 naa4 daai6");
    result = entry.getJyutpingNumbers();
    expected = {1, 4, 6};
    QCOMPARE(result, expected);
}

void TestEntry::getPinyinNumbersSimple()
{
    std::string pinyin = "tai2 shan1 hua4";
    Entry entry{"", "", "", pinyin, {}};

    auto result = entry.getPinyinNumbers();
    std::vector<uint8_t> expected{2, 1, 4};
    QCOMPARE(result, expected);

    entry.setPinyin("mei3 guo2");
    result = entry.getPinyinNumbers();
    expected = {3, 2};
    QCOMPARE(result, expected);

    entry.setPinyin("jia1 na2 da4");
    result = entry.getPinyinNumbers();
    expected = {1, 2, 4};
    QCOMPARE(result, expected);
}

void TestEntry::getPinyinNumbersInvalidTones()
{
    std::string pinyin = "tai2 shan1 shan6 hua4";
    Entry entry{"", "", "", pinyin, {}};

    auto result = entry.getPinyinNumbers();
    std::vector<uint8_t> expected{2, 1, 4};
    QCOMPARE(result, expected);
}

void TestEntry::getPinyinNumbersResetPinyin()
{
    std::string pinyin = "tai2 shan1 hua4";
    Entry entry{"", "", "", pinyin, {}};

    auto result = entry.getPinyinNumbers();
    std::vector<uint8_t> expected{2, 1, 4};
    QCOMPARE(result, expected);

    entry.setPinyin("jia1 na2 da4");
    result = entry.getPinyinNumbers();
    expected = {1, 2, 4};
    QCOMPARE(result, expected);
}

void TestEntry::getDefinitionsSetsSimple()
{
    std::vector<Definition::Definition> definitions{
        {"Chinatown", "", {}},
        {"CL:條|条[tiao2],座[zuo4]", "", {}},
    };

    DefinitionsSet definitionsSet{"CC-CEDICT", definitions};

    std::string traditional = "唐人街";
    std::string simplified = "唐人街";
    std::string jyutping = "tong4 jan4 gaai1";
    std::string pinyin = "tang2 ren2 jie1";
    Entry entry{traditional, simplified, jyutping, pinyin, {definitionsSet}};

    QCOMPARE(std::vector<DefinitionsSet>(entry.getDefinitionsSets().begin(),
                                         entry.getDefinitionsSets().end()),
             std::vector<DefinitionsSet>{definitionsSet});
}

void TestEntry::modifyDefinitionsSetsSimple()
{
    Definition::Definition definition{"Chinatown", "", {}};
    std::vector<Definition::Definition> definitions{
        definition,
        {"CL:條|条[tiao2],座[zuo4]", "", {}},
    };

    DefinitionsSet definitionsSet{"CC-CEDICT", definitions};

    std::string traditional = "唐人街";
    std::string simplified = "唐人街";
    std::string jyutping = "tong4 jan4 gaai1";
    std::string pinyin = "tang2 ren2 jie1";
    Entry entry{traditional, simplified, jyutping, pinyin, {definitionsSet}};

    QCOMPARE(std::vector<DefinitionsSet>(entry.getDefinitionsSets().begin(),
                                         entry.getDefinitionsSets().end()),
             std::vector<DefinitionsSet>{definitionsSet});

    definition.definitionContent
        = "(historical) Qingyuan (a former county of Guangdong, China)";
    // DefinitionsSet should be copied into entry, so modifications outside should not affect it
    QCOMPARE(definitionsSet.getDefinitions()[0].definitionContent, "Chinatown");
    // The sets returned by getDefinitionsSets should be a copy, not a reference to the internal sets
    QCOMPARE(entry.getDefinitionsSets()[0].getDefinitions()[0].definitionContent,
             "Chinatown");
}

void TestEntry::getDefinitionSnippetSimple()
{
    Definition::Definition definition{"Chinatown", "", {}};
    DefinitionsSet definitionsSet{"CC-CEDICT", {definition}};
    std::string traditional = "唐人街";
    std::string simplified = "唐人街";
    std::string jyutping = "tong4 jan4 gaai1";
    std::string pinyin = "tang2 ren2 jie1";
    Entry entry{traditional, simplified, jyutping, pinyin, {definitionsSet}};

    QCOMPARE(entry.getDefinitionSnippet(), "Chinatown");
}

void TestEntry::getDefinitionSnippetEmptyDefinitionsSet()
{
    std::string traditional = "唐人街";
    std::string simplified = "唐人街";
    std::string jyutping = "tong4 jan4 gaai1";
    std::string pinyin = "tang2 ren2 jie1";
    Entry entry{traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(entry.getDefinitionSnippet(), "");
}

void TestEntry::getDefintionSnippetEmptyDefinition()
{
    Definition::Definition definition{"", "", {}};
    DefinitionsSet definitionsSet{"CC-CEDICT", {definition}};
    std::string traditional = "唐人街";
    std::string simplified = "唐人街";
    std::string jyutping = "tong4 jan4 gaai1";
    std::string pinyin = "tang2 ren2 jie1";
    Entry entry{traditional, simplified, jyutping, pinyin, {definitionsSet}};

    QCOMPARE(entry.getDefinitionSnippet(), "");
}

void TestEntry::specialCases()
{
    Entry entry;
    entry.setIsWelcome(true);
    QCOMPARE(entry.isWelcome(), true);

    entry.setIsEmpty(true);
    QCOMPARE(entry.isEmpty(), true);
}

QTEST_APPLESS_MAIN(TestEntry)

#include "tst_entry.moc"
