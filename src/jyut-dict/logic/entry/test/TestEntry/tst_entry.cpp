#include <QtTest>

#include "logic/entry/entry.h"
#include "logic/entry/entrycharactersoptions.h"
#include "logic/utils/chineseutils.h"

class TestEntry : public QObject
{
    Q_OBJECT

public:
    TestEntry();
    ~TestEntry();

private slots:
    void getCharacters();
    void simplifiedAndTraditional();

    void cantonesePhonetic();
    void cantoneseToneNumbers();
    void mandarinPhonetic();
    void mandarinToneNumbers();
    void getPhonetic();

    void definitions();

    void refreshColours();

    void specialCases();
};

TestEntry::TestEntry() {}

TestEntry::~TestEntry() {}

void TestEntry::getCharacters()
{
    std::string simplified{"清远"};
    std::string traditional{"清遠"};
    std::string jyutping{"cing1 jyun5"};
    std::string pinyin{"qing1 yuan3"};
    Entry entry{simplified, traditional, jyutping, pinyin, {}};

    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED,
                                     /* useColours = */ false)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL,
                                     /* useColours = */ false)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED,
                                     /* useColours = */ false)),
             QString::fromStdString(
                 simplified + " ["
                 + ChineseUtils::compareStrings(simplified, traditional) + "]"));
    QCOMPARE(QString::fromStdString(
                 entry.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL,
                                     /* useColours = */ false)),
             QString::fromStdString(
                 traditional + " ["
                 + ChineseUtils::compareStrings(traditional, simplified) + "]"));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_SIMPLIFIED,
                 /* useColours = */ false)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::PREFER_SIMPLIFIED,
                 /* useColours = */ false)),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_TRADITIONAL,
                 /* useColours = */ false)),
             QString::fromStdString(traditional));
    QCOMPARE(QString::fromStdString(entry.getCharactersNoSecondary(
                 EntryCharactersOptions::ONLY_TRADITIONAL,
                 /* useColours = */ false)),
             QString::fromStdString(traditional));
}

void TestEntry::simplifiedAndTraditional()
{
    std::string simplified{"清远"};
    std::string traditional{"清遠"};
    Entry entry{simplified, traditional, "", "", {}};

    QCOMPARE(QString::fromStdString(entry.getSimplified()),
             QString::fromStdString(simplified));
    QCOMPARE(QString::fromStdString(entry.getTraditional()),
             QString::fromStdString(traditional));

    std::string newSimplified{"广州"};
    entry.setSimplified(newSimplified);
    QCOMPARE(QString::fromStdString(entry.getSimplified()),
             QString::fromStdString(newSimplified));
    QCOMPARE(QString::fromStdString(entry.getTraditional()),
             QString::fromStdString(traditional));

    std::string newTraditional{"廣州"};
    entry.setTraditional(newTraditional);
    QCOMPARE(QString::fromStdString(entry.getSimplified()),
             QString::fromStdString(newSimplified));
    QCOMPARE(QString::fromStdString(entry.getTraditional()),
             QString::fromStdString(newTraditional));
}

void TestEntry::cantonesePhonetic()
{
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";
    Entry entry{simplified, traditional, jyutping, pinyin, {}};
    entry.generatePhonetic(CantoneseOptions::RAW_JYUTPING
                               | CantoneseOptions::PRETTY_YALE
                               | CantoneseOptions::CANTONESE_IPA,
                           MandarinOptions::NONE);

    QCOMPARE(QString::fromStdString(
                 entry.getCantonesePhonetic(CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(entry.getJyutping()),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 entry.getCantonesePhonetic(CantoneseOptions::PRETTY_YALE)),
             "tòih sāan wá");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 entry.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯ ˨ ˩  säːn ˥  wäː ˧ ˥"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 entry.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯˨˩  säːn˥  wäː˧˥"});
#endif
}

void TestEntry::cantoneseToneNumbers()
{
    std::string jyutping = "toi4 saan1 waa2";
    Entry entry{"", "", jyutping, "", {}};

    auto result = entry.getJyutpingNumbers();
    std::vector<int> expected{4, 1, 2};
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

void TestEntry::mandarinPhonetic()
{
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";
    Entry entry{simplified, traditional, jyutping, pinyin, {}};
    entry.generatePhonetic(CantoneseOptions::NONE,
                           MandarinOptions::PRETTY_PINYIN
                               | MandarinOptions::ZHUYIN
                               | MandarinOptions::MANDARIN_IPA);

    QCOMPARE(QString::fromStdString(
                 entry.getMandarinPhonetic(MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(entry.getPinyin()),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(
                 entry.getMandarinPhonetic(MandarinOptions::PRETTY_PINYIN)),
             "tái shān huà");
    QCOMPARE(QString::fromStdString(
                 entry.getMandarinPhonetic(MandarinOptions::ZHUYIN)),
             "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 entry.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             "tʰaɪ̯ ˧ ˥  ʂän ˥ ˥  xwä ˥ ˩");
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 entry.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             "tʰaɪ̯˧˥  ʂän˥˥  xwä˥˩");
#endif
}

void TestEntry::mandarinToneNumbers()
{
    std::string pinyin = "tai2 shan1 hua4";
    Entry entry{"", "", "", pinyin, {}};

    auto result = entry.getPinyinNumbers();
    std::vector<int> expected{2, 1, 4};
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

void TestEntry::getPhonetic()
{
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";
    Entry entry{simplified, traditional, jyutping, pinyin, {}};
    entry.generatePhonetic(CantoneseOptions::RAW_JYUTPING
                               | CantoneseOptions::PRETTY_YALE
                               | CantoneseOptions::CANTONESE_IPA,
                           MandarinOptions::NUMBERED_PINYIN
                               | MandarinOptions::PRETTY_PINYIN
                               | MandarinOptions::ZHUYIN
                               | MandarinOptions::MANDARIN_IPA);

    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE)),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN)),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE)),
             QString::fromStdString(jyutping + " (" + pinyin + ")"));
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN)),
             QString::fromStdString(pinyin + " (" + jyutping + ")"));

    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                   MandarinOptions::PRETTY_PINYIN)),
             "tái shān huà");
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                   MandarinOptions::ZHUYIN)),
             QString::fromStdString("ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ (" + jyutping + ")"));

    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                   CantoneseOptions::PRETTY_YALE)),
             "tòih sāan wá");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA)),
             QString::fromStdString("tʰɔːi̯ ˨ ˩  säːn ˥  wäː ˧ ˥ (" + pinyin
                                    + ")"));
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::CANTONESE_IPA)),
             QString::fromStdString("tʰɔːi̯ ˨ ˩  säːn ˥  wäː ˧ ˥ (" + pinyin
                                    + ")"));
#endif

    QCOMPARE(QString::fromStdString(
                 entry.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                   CantoneseOptions::PRETTY_YALE,
                                   MandarinOptions::PRETTY_PINYIN)),
             "tòih sāan wá (tái shān huà)");
}

void TestEntry::definitions()
{
    QSKIP("Not implemented yet");
}

void TestEntry::refreshColours()
{
    QSKIP("Not implemented yet");
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
