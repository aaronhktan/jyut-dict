#include <QtTest>

#include "logic/utils/scriptdetector.h"

class TestScriptDetector : public QObject
{
    Q_OBJECT

public:
    TestScriptDetector();
    ~TestScriptDetector();

private slots:
    void simplifiedChinese();
    void noSimplifiedChinese();

    void traditionalChinese();
    void noTraditionalChinese();

    void chinese();
    void noChinese();

    void validJyutping();
    void noValidJyutping();

    void validPinyin();
    void noValidPinyin();
};

TestScriptDetector::TestScriptDetector() {}

TestScriptDetector::~TestScriptDetector() {}

void TestScriptDetector::simplifiedChinese()
{
    ScriptDetector detector{"语言"};
    QCOMPARE(detector.containsSimplifiedChinese(), true);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);
}

void TestScriptDetector::noSimplifiedChinese()
{
    ScriptDetector detector{"incomplete"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"預"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), true);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"個個person"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), true);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"cingjyun2"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"yongyuan"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);
}

void TestScriptDetector::traditionalChinese()
{
    ScriptDetector detector{"語言"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), true);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);
}

void TestScriptDetector::noTraditionalChinese()
{
    ScriptDetector detector{"incomplete"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"预"};
    QCOMPARE(detector.containsSimplifiedChinese(), true);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"个个person"};
    QCOMPARE(detector.containsSimplifiedChinese(), true);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"cingjyun2"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"yongyuan"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);
}

void TestScriptDetector::chinese()
{
    ScriptDetector detector{"叮叮"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"一二三四五"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), true);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);
}

void TestScriptDetector::noChinese()
{
    ScriptDetector detector{"mto"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"guiren"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);

    detector = {"notgood"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);
}

void TestScriptDetector::validJyutping()
{
    ScriptDetector detector{"oet4 hei3"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"oet4hei3"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"oet hei"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"oethei"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"cat1"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), true);
    QCOMPARE(detector.isValidPinyin(), false);
}

void TestScriptDetector::noValidJyutping()
{
    ScriptDetector detector{"bonk"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"ngngngngngngngngngngngngng"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);
}

void TestScriptDetector::validPinyin()
{
    ScriptDetector detector{"hua2 ren2"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);

    detector = {"hua2ren2"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);

    detector = {"ji xu"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);

    detector = {"jixu"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);

    detector = {"cha2"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), true);
}

void TestScriptDetector::noValidPinyin()
{
    ScriptDetector detector{"bonk"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);

    detector = {"xxxxxxxxxx"};
    QCOMPARE(detector.containsSimplifiedChinese(), false);
    QCOMPARE(detector.containsTraditionalChinese(), false);
    QCOMPARE(detector.containsChinese(), false);
    QCOMPARE(detector.isValidJyutping(), false);
    QCOMPARE(detector.isValidPinyin(), false);
}

QTEST_APPLESS_MAIN(TestScriptDetector)

#include "tst_scriptdetector.moc"
