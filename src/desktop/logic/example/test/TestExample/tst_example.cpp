#include <QtTest>

#include "logic/example/example.h"

class TestExample : public QObject
{
    Q_OBJECT

public:
    TestExample();
    ~TestExample();

private slots:
    void constructor();

    void sourceLanguageGetSet();

    void getCharactersTraditional();
    void getCharactersSimplified();
    void traditionalGetSet();
    void simplifiedGetSet();

    void getPhoneticCantonese();
    void getPhoneticMandarin();
    void jyutpingGetSet();
    void pinyinGetSet();

    void getTranslationSetsSimple();
    void getTranslationSnippetSimple();
    void getTranslationSnippetEmptyTranslations();
    void getTranslationSnippetEmptyTranslationSet();
    void getTranslationSnippetEmptyTranslationContent();

    void specialCases();
};

TestExample::TestExample() {}

TestExample::~TestExample() {}

void TestExample::constructor()
{
    std::string language{"yue"};
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    std::string jyutping{"toi4 saan1 waa2"};
    std::string pinyin{"tai2 shan1 hua4"};
    Example example{language, traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(example.getSourceLanguage(), language);
    QCOMPARE(example.getSimplified(), simplified);
    QCOMPARE(example.getTraditional(), traditional);
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(pinyin));
}

void TestExample::sourceLanguageGetSet()
{
    std::string language = "yue";
    Example example{"", "", "", "", "", {}};
    QCOMPARE(example.getSourceLanguage(), "");

    example.setSourceLanguage(language);
    QCOMPARE(example.getSourceLanguage(), language);
}

void TestExample::getCharactersTraditional()
{
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    Example example{"yue", traditional, simplified, "", "", {}};

    QCOMPARE(example.getTraditional(), traditional);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL),
             traditional);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL),
             traditional);
}

void TestExample::getCharactersSimplified()
{
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    Example example{"yue", traditional, simplified, "", "", {}};

    QCOMPARE(example.getSimplified(), simplified);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED),
             simplified);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED),
             simplified);
}

void TestExample::traditionalGetSet()
{
    std::string traditional{"臺山話"};

    Example example{"", "", "", "", "", {}};
    QCOMPARE(example.getTraditional(), "");

    example.setTraditional(traditional);

    QCOMPARE(example.getTraditional(), traditional);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::ONLY_TRADITIONAL),
             traditional);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::PREFER_TRADITIONAL),
             traditional);
}

void TestExample::simplifiedGetSet()
{
    std::string simplified{"台山话"};

    Example example{"", "", "", "", "", {}};
    QCOMPARE(example.getSimplified(), "");

    example.setSimplified(simplified);

    QCOMPARE(example.getSimplified(), simplified);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::ONLY_SIMPLIFIED),
             simplified);
    QCOMPARE(example.getCharacters(EntryCharactersOptions::PREFER_SIMPLIFIED),
             simplified);
}

void TestExample::getPhoneticCantonese()
{
    std::string language{"yue"};
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    std::string jyutping{"toi4 saan1 waa2"};
    std::string pinyin{"tai2 shan1 hua4"};
    Example example{language, traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(example.getJyutping(), jyutping);
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                 CantoneseOptions::PRETTY_YALE,
                                 MandarinOptions::RAW_PINYIN),
             "tòih sāan wá");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯ ˨ ˩  säːn ˥  wäː ˧ ˥"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯˨˩  säːn˥  wäː˧˥"});
#endif

    example.setPinyin("");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                 CantoneseOptions::PRETTY_YALE,
                                 MandarinOptions::RAW_PINYIN),
             "tòih sāan wá");
    example.setJyutping("");
    example.setPinyin("tai2 shan1 hua4");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::PREFER_CANTONESE,
                                 CantoneseOptions::CANTONESE_IPA,
                                 MandarinOptions::RAW_PINYIN),
             "");
}

void TestExample::getPhoneticMandarin()
{
    std::string language{"yue"};
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    std::string jyutping{"toi4 saan1 waa2"};
    std::string pinyin{"tai2 shan1 hua4"};
    Example example{language, traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(example.getPinyin(), pinyin);
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::PRETTY_PINYIN),
             "tái shān huà");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::NUMBERED_PINYIN),
             "tai2 shan1 hua4");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::ZHUYIN),
             "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             QString{"tʰaɪ̯ ˧ ˥  ʂän ˥ ˥  xu̯ä ˥ ˩"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             QString{"tʰaɪ̯˧˥ ʂän˥˥ xu̯ä˥˩"});
#endif

    example.setJyutping("");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::PRETTY_PINYIN),
             "tái shān huà");

    example.setPinyin("");
    example.setJyutping("toi4 saan1 waa2");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::PREFER_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::MANDARIN_IPA),
             "");
}

void TestExample::jyutpingGetSet()
{
    std::string language{"yue"};
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    std::string jyutping{"toi4 saan1 waa2"};
    std::string pinyin{"tai2 shan1 hua4"};
    Example example{language, traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(example.getJyutping(), jyutping);
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                 CantoneseOptions::PRETTY_YALE,
                                 MandarinOptions::RAW_PINYIN),
             "tòih sāan wá");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯ ˨ ˩  säːn ˥  wäː ˧ ˥"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯˨˩  säːn˥  wäː˧˥"});
#endif

    std::string newJyutping{"SeI3 jAp1 WaA2"};
    std::string normalizedNewJyutping{"sei3 jap1 waa2"};
    example.setJyutping(newJyutping);
    QCOMPARE(example.getJyutping(), normalizedNewJyutping);
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_CANTONESE,
                                 CantoneseOptions::PRETTY_YALE,
                                 MandarinOptions::RAW_PINYIN),
             "sei yāp wá");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"sei̯ ˧  jɐp̚ ˥  wäː ˧ ˥"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::CANTONESE_IPA)),
             QString{"sei̯˧ jɐp̚˥ wäː˧˥"});
#endif
}

void TestExample::pinyinGetSet()
{
    std::string language{"yue"};
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    std::string jyutping{"toi4 saan1 waa2"};
    std::string pinyin{"tai2 shan1 hua4"};
    Example example{language, traditional, simplified, jyutping, pinyin, {}};

    QCOMPARE(example.getPinyin(), pinyin);
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::PRETTY_PINYIN),
             "tái shān huà");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::NUMBERED_PINYIN),
             "tai2 shan1 hua4");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::ZHUYIN),
             "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             QString{"tʰaɪ̯ ˧ ˥  ʂän ˥ ˥  xu̯ä ˥ ˩"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             QString{"tʰaɪ̯˧˥ ʂän˥˥ xu̯ä˥˩"});
#endif

    std::string newPinyin{"Si4 yi4 HuA4"};
    std::string normalizedNewPinyin{"si4 yi4 hua4"};
    example.setPinyin(newPinyin);
    QCOMPARE(example.getPinyin(), normalizedNewPinyin);
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::PRETTY_PINYIN),
             "sì yì huà");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::NUMBERED_PINYIN),
             "si4 yi4 hua4");
    QCOMPARE(example.getPhonetic(EntryPhoneticOptions::ONLY_MANDARIN,
                                 CantoneseOptions::RAW_JYUTPING,
                                 MandarinOptions::ZHUYIN),
             "ㄙˋ ㄧˋ ㄏㄨㄚˋ");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             QString{"sz̩ ˥ ˩ ꜒ ꜔  i ˥ ˩ ꜒ ꜔  xu̯ä ˥ ˩"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             QString{"sz̩˥˩꜒꜔ i˥˩꜒꜔ xu̯ä˥˩"});
#endif
}

void TestExample::getTranslationSetsSimple()
{
    Translation::Translation translation{"Taishanese", "eng", true};
    TranslationSet set = {"Wiktionary", {translation}};

    std::string language{"yue"};
    std::string traditional{"臺山話"};
    std::string simplified{"台山话"};
    std::string jyutping{"toi4 saan1 waa2"};
    std::string pinyin{"tai2 shan1 hua4"};
    Example example{language, traditional, simplified, jyutping, pinyin, {set}};

    std::span<const TranslationSet> setsSpan = example.getTranslationSets();
    std::vector<TranslationSet> sets;
    sets.assign(setsSpan.begin(), setsSpan.end());

    QCOMPARE(sets, {set});
}

void TestExample::getTranslationSnippetSimple()
{
    Translation::Translation translation{"egg waffle", "eng", true};
    TranslationSet set = {"粵典—words.hk", {translation}};
    std::string language{"yue"};
    std::string traditional{"雞蛋仔"};
    std::string simplified{"鸡蛋仔"};
    std::string jyutping{"gai1 daan2 zai2"};
    std::string pinyin{"ji1 dan4 zai3"};
    Example example{language, traditional, simplified, jyutping, pinyin, {set}};

    QCOMPARE(example.getTranslationSnippet(), "egg waffle");
}

void TestExample::getTranslationSnippetEmptyTranslations()
{
    std::string language{"yue"};
    std::string traditional{"雞蛋仔"};
    std::string simplified{"鸡蛋仔"};
    std::string jyutping{"gai1 daan2 zai2"};
    std::string pinyin{"ji1 dan4 zai3"};
    Example example{language, traditional, simplified, jyutping, pinyin, {}};
    QCOMPARE(example.getTranslationSnippet(), "");
}

void TestExample::getTranslationSnippetEmptyTranslationSet()
{
    TranslationSet set = {"粵典—words.hk", {}};
    std::string language{"yue"};
    std::string traditional{"雞蛋仔"};
    std::string simplified{"鸡蛋仔"};
    std::string jyutping{"gai1 daan2 zai2"};
    std::string pinyin{"ji1 dan4 zai3"};
    Example example{language, traditional, simplified, jyutping, pinyin, {set}};

    QCOMPARE(example.getTranslationSnippet(), "");
}

void TestExample::getTranslationSnippetEmptyTranslationContent()
{
    Translation::Translation translation{"", "eng", true};
    TranslationSet set = {"粵典—words.hk", {translation}};
    std::string language{"yue"};
    std::string traditional{"雞蛋仔"};
    std::string simplified{"鸡蛋仔"};
    std::string jyutping{"gai1 daan2 zai2"};
    std::string pinyin{"ji1 dan4 zai3"};
    Example example{language, traditional, simplified, jyutping, pinyin, {set}};

    QCOMPARE(example.getTranslationSnippet(), "");
}

void TestExample::specialCases()
{
    Example example;
    example.setIsEmpty(true);
    QCOMPARE(example.isEmpty(), true);

    example.setIsWelcome(true);
    QCOMPARE(example.isWelcome(), true);
}

QTEST_APPLESS_MAIN(TestExample)

#include "tst_example.moc"
