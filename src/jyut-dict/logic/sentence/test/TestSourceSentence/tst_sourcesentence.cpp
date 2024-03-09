#include <QtTest>

#include "logic/sentence/sourcesentence.h"

class TestSourceSentence : public QObject
{
    Q_OBJECT

public:
    TestSourceSentence();
    ~TestSourceSentence();

private slots:
    void constructor();
    void settersAndGetters();
    void generatePhonetic();
    void specialCases();
};

TestSourceSentence::TestSourceSentence() {}

TestSourceSentence::~TestSourceSentence() {}

void TestSourceSentence::constructor()
{
    std::string language = "yue";
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";
    SourceSentence sentence{language,
                            simplified,
                            traditional,
                            jyutping,
                            pinyin,
                            {}};

    QCOMPARE(sentence.getSourceLanguage(), language);
    QCOMPARE(sentence.getSimplified(), simplified);
    QCOMPARE(sentence.getTraditional(), traditional);
    QCOMPARE(QString::fromStdString(
                 sentence.getCantonesePhonetic(CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 sentence.getMandarinPhonetic(MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(pinyin));
}

void TestSourceSentence::settersAndGetters()
{
    Sentence::TargetSentence targetSentence{"Taishanese", "eng", true};
    SentenceSet set = {"Wiktionary", {targetSentence}};

    std::string language = "yue";
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";

    SourceSentence sentence{"", "", "", "", "", {set}};
    sentence.setSourceLanguage(language);
    sentence.setSimplified(simplified);
    sentence.setTraditional(traditional);
    sentence.setJyutping(jyutping);
    sentence.setPinyin(pinyin);

    QCOMPARE(sentence.getSourceLanguage(), language);
    QCOMPARE(sentence.getSimplified(), simplified);
    QCOMPARE(sentence.getTraditional(), traditional);
    QCOMPARE(QString::fromStdString(sentence.getJyutping()),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(sentence.getPinyin()),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(sentence.getSentenceSnippet()),
             "Taishanese");
    QCOMPARE(QString::fromStdString(sentence.getSentenceSnippetLanguage()),
             "eng");
}

void TestSourceSentence::generatePhonetic()
{
    std::string language = "yue";
    std::string simplified = "台山话";
    std::string traditional = "臺山話";
    std::string jyutping = "toi4 saan1 waa2";
    std::string pinyin = "tai2 shan1 hua4";
    SourceSentence sentence{language,
                            simplified,
                            traditional,
                            jyutping,
                            pinyin,
                            {}};

    sentence.generatePhonetic(CantoneseOptions::RAW_JYUTPING
                                  | CantoneseOptions::PRETTY_YALE
                                  | CantoneseOptions::CANTONESE_IPA,
                              MandarinOptions::PRETTY_PINYIN
                                  | MandarinOptions::ZHUYIN
                                  | MandarinOptions::MANDARIN_IPA);
    QCOMPARE(QString::fromStdString(
                 sentence.getCantonesePhonetic(CantoneseOptions::RAW_JYUTPING)),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(sentence.getJyutping()),
             QString::fromStdString(jyutping));
    QCOMPARE(QString::fromStdString(
                 sentence.getCantonesePhonetic(CantoneseOptions::PRETTY_YALE)),
             "tòih sāan wá");
    QCOMPARE(QString::fromStdString(sentence.getCantonesePhonetic(
                 CantoneseOptions::CANTONESE_IPA)),
             QString{"tʰɔːi̯ ˨ ˩  säːn ˥  wäː ˧ ˥"});
    QCOMPARE(QString::fromStdString(
                 sentence.getMandarinPhonetic(MandarinOptions::RAW_PINYIN)),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(sentence.getPinyin()),
             QString::fromStdString(pinyin));
    QCOMPARE(QString::fromStdString(
                 sentence.getMandarinPhonetic(MandarinOptions::PRETTY_PINYIN)),
             "tái shān huà");
    QCOMPARE(QString::fromStdString(sentence.getPrettyPinyin()), "tái shān huà");
    QCOMPARE(QString::fromStdString(
                 sentence.getMandarinPhonetic(MandarinOptions::ZHUYIN)),
             "ㄊㄞˊ ㄕㄢ ㄏㄨㄚˋ");
    QCOMPARE(QString::fromStdString(
                 sentence.getMandarinPhonetic(MandarinOptions::MANDARIN_IPA)),
             "tʰaɪ̯ ˧ ˥  ʂän ˥ ˥  xwä ˥ ˩");
}

void TestSourceSentence::specialCases()
{
    SourceSentence sentence;
    sentence.setIsEmpty(true);
    QCOMPARE(sentence.isEmpty(), true);

    sentence.setIsWelcome(true);
    QCOMPARE(sentence.isWelcome(), true);
}

QTEST_APPLESS_MAIN(TestSourceSentence)

#include "tst_sourcesentence.moc"
