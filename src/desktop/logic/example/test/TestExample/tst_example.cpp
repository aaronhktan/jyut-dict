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
    void settersAndGetters();
    void generatePhonetic();
    void specialCases();
};

TestExample::TestExample() {}

TestExample::~TestExample() {}

void TestExample::constructor()
{
    std::string language = "yue";
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";
    Example example{language,
                            simplified,
                            traditional,
                            jyutping,
                            pinyin,
                            {}};

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

void TestExample::settersAndGetters()
{
    Translation::Translation translation{"Taishanese", "eng", true};
    TranslationSet set = {"Wiktionary", {translation}};

    std::string language = "yue";
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";

    Example example{"", "", "", "", "", {set}};
    example.setSourceLanguage(language);
    example.setSimplified(simplified);
    example.setTraditional(traditional);
    example.setJyutping(jyutping);
    example.setPinyin(pinyin);

    QCOMPARE(example.getSourceLanguage(), language);
    QCOMPARE(example.getSimplified(), simplified);
    QCOMPARE(example.getTraditional(), traditional);
    QCOMPARE(QString::fromStdString(example.getJyutping()),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(example.getPinyin()),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(example.getTranslationSnippet()),
             "Taishanese");
    QCOMPARE(QString::fromStdString(example.getTranslationSnippetLanguage()),
             "eng");
}

void TestExample::generatePhonetic()
{
    std::string language = "yue";
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";
    Example example{language,
                            simplified,
                            traditional,
                            jyutping,
                            pinyin,
                            {}};

    example.generatePhonetic(CantoneseOptions::RAW_JYUTPING
                                  | CantoneseOptions::PRETTY_YALE
                                  | CantoneseOptions::CANTONESE_IPA,
                              MandarinOptions::NUMBERED_PINYIN
                                  | MandarinOptions::PRETTY_PINYIN
                                  | MandarinOptions::ZHUYIN
                                  | MandarinOptions::MANDARIN_IPA);
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(example.getJyutping()),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 example.getCantonesePhonetic(CantoneseOptions::PRETTY_YALE)),
             "tòih sāan wá");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(example.getCantonesePhonetic(
                 CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯ ˨ ˩  säːn ˥  wäː ˧ ˥"});
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(example.getCantonesePhonetic(
                 CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯˨˩  säːn˥  wäː˧˥"});
#endif
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(example.getPinyin()),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(example.getMandarinPhonetic(
                 MandarinOptions::NUMBERED_PINYIN)),
             QString::fromStdString("tai2 shan1 hua4"));
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::PRETTY_PINYIN)),
             "tái shān huà");
    QCOMPARE(QString::fromStdString(example.getPrettyPinyin()), "tái shān huà");
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::ZHUYIN)),
             "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ");
#ifdef Q_OS_MAC
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             "tʰaɪ̯ ˧ ˥  ʂän ˥ ˥  xwä ˥ ˩");
#elif defined(Q_OS_LINUX)
    QCOMPARE(QString::fromStdString(
                 example.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             "tʰaɪ̯˧˥  ʂän˥˥  xwä˥˩");
#endif
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
