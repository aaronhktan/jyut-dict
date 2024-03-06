#include <QtTest>

#include "logic/utils/chineseutils.h"

class TestChineseUtils : public QObject
{
    Q_OBJECT

public:
    TestChineseUtils();
    ~TestChineseUtils();

private slots:
    void segmentJyutpingSimple();
    void segmentJyutpingNoDigits();
    void segmentJyutpingNoSpaces();
    void segmentJyutpingNoDigitsNoSpaces();
    void segmentJyutpingNoDigitsApostrophe();
    void segmentJyutpingDigitsApostrophe();
    void segmentJyutpingRemoveSpecialCharacters();
    void segmentJyutpingKeepGlobCharacters();
    void segmentJyutpingKeepGlobCharactersNoWhitespace();
    void segmentJyutpingKeepMultipleGlobCharacters();
    void segmentJyutpingKeepMultipleGlobCharactersWhitespace();
    void segmentJyutpingKeepMultipleGlobCharactersWhitespaceSurround();
    void segmentJyutpingGlobCharactersTrimWhitespace();
    void segmentJyutpingKeepSpecialCharacters();
    void segmentJyutpingRemoveWhitespace();
    void segmentJyutpingLower();
    void segmentJyutpingLowerWithDigits();
    void segmentJyutpingMultipleFinalsVowelsOnly();
    void segmentJyutpingMultipleFinalsConsonantsOnly();
    void segmentJyutpingMultipleFinals();
    void segmentJyutpingGarbage();
};

TestChineseUtils::TestChineseUtils() {}

TestChineseUtils::~TestChineseUtils() {}

void TestChineseUtils::segmentJyutpingSimple()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m4 goi1", result);
    std::vector<std::string> expected = {"m4", "goi1"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingNoDigits()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m goi", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingNoSpaces()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m4goi1", result);
    std::vector<std::string> expected = {"m4", "goi1"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingNoDigitsNoSpaces()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("mgoi", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingNoDigitsApostrophe()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m'aam", result);
    std::vector<std::string> expected = {"m", "aam"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingDigitsApostrophe()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m4'aam", result);
    std::vector<std::string> expected = {"m4", "aam"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingRemoveSpecialCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m*goi", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingKeepGlobCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m* goi",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingKeepGlobCharactersNoWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m*goi",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "*", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingKeepMultipleGlobCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m?* goi",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "?", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingKeepMultipleGlobCharactersWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m? * goi",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "? ", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingKeepMultipleGlobCharactersWhitespaceSurround()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m ? * goi",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", " ? ", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingGlobCharactersTrimWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m  ?            *      goi",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", " ? ", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingKeepSpecialCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m？ goi",
                                  result,
                                  /* removeSpecialCharacters = */ false);
    std::vector<std::string> expected = {"m", "？", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingRemoveWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("  m                           goi      ",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingLower()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("mGoI", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingLowerWithDigits()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("m4GoI1", result);
    std::vector<std::string> expected = {"m4", "goi1"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingMultipleFinalsVowelsOnly()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("aaaa",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"aa", "aa"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingMultipleFinalsConsonantsOnly()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("ngng",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"ng", "ng"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingMultipleFinals()
{
    std::vector<std::string> result;
    ChineseUtils::segmentJyutping("amam",
                                  result,
                                  /* removeSpecialCharacters = */ true,
                                  /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"am", "am"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentJyutpingGarbage()
{
    std::vector<std::string> result;
    bool valid
        = ChineseUtils::segmentJyutping("kljnxclkjvnl",
                                        result,
                                        /* removeSpecialCharacters = */ true,
                                        /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"kljnxclkjvnl"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);
}

QTEST_APPLESS_MAIN(TestChineseUtils)

#include "tst_testchineseutils.moc"
