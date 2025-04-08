#include <QtTest>

#include "logic/utils/cantoneseutils.h"

class TestCantoneseUtils : public QObject
{
    Q_OBJECT

public:
    TestCantoneseUtils();
    ~TestCantoneseUtils();

private slots:
    void jyutpingToYaleSimple();
    void jyutpingToYaleRejectNoTone();
    void jyutpingToYaleRejectSingleLetter();
    void jyutpingToYaleRejectSpecialCharacter();
    void jyutpingToYaleNoSpaces();
    void jyutpingToYaleSpacesToSegment();
    void jyutpingToYaleSpecialFinal();
    void jyutpingToYaleLightTone();
    void jyutpingToYaleSpecialSyllable();
    void jyutpingToYaleTones();
    void jyutpingToYaleNoTone();

    void jyutpingToIPASimple();
    void jyutpingToIPARejectNoTone();
    void jyutpingToIPARejectSingleLetter();
    void jyutpingToIPARejectSpecialCharacter();
    void jyutpingToIPANoSpaces();
    void jyutpingToIPASpacesToSegment();
    void jyutpingToIPAPreprocessInitial();
    void jyutpingToIPASpecialSyllable();
    void jyutpingToIPACheckedTone();
    void jyutpingToIPASpecialFinal();
    void jyutpingToIPATones();
    void jyutpingToIPANoTone();

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
    void segmentJyutpingInvalidTone();
    void segmentJyutpingGarbage();
};

TestCantoneseUtils::TestCantoneseUtils() {}

TestCantoneseUtils::~TestCantoneseUtils() {}

void TestCantoneseUtils::jyutpingToYaleSimple()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("si1 zi2 saan1");
    qDebug() << result.c_str();
    QCOMPARE(result, "sī jí sāan");
}

void TestCantoneseUtils::jyutpingToYaleRejectNoTone()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("joeng");
    QCOMPARE(result, "joeng");
}

void TestCantoneseUtils::jyutpingToYaleRejectSingleLetter()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("a");
    QCOMPARE(result, "a");
}

void TestCantoneseUtils::jyutpingToYaleRejectSpecialCharacter()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("-");
    QCOMPARE(result, "x");
}

void TestCantoneseUtils::jyutpingToYaleNoSpaces()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("si1zi2saan1");
    QCOMPARE(result, "sī jí sāan");
}

void TestCantoneseUtils::jyutpingToYaleSpacesToSegment()
{
    std::string result
        = CantoneseUtils::convertJyutpingToYale("si1 zi2 saan1",
                                                /* usSpacesToSegment = */ true);
    QCOMPARE(result, "sī jí sāan");
}

void TestCantoneseUtils::jyutpingToYaleSpecialFinal()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("goek3jyun5");
    QCOMPARE(result, "geuk yúhn");
}

void TestCantoneseUtils::jyutpingToYaleLightTone()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("lok6 jyu5");
    QCOMPARE(result, "lohk yúh");
}

void TestCantoneseUtils::jyutpingToYaleSpecialSyllable()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("m4 hai6");
    QCOMPARE(result, "m̀h haih");
}

void TestCantoneseUtils::jyutpingToYaleTones()
{
    std::string result = CantoneseUtils::convertJyutpingToYale(
        "saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6");
    QCOMPARE(result, "sāam gáu sei lìhng ńgh yih chāt baat luhk");
}

void TestCantoneseUtils::jyutpingToYaleNoTone()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("mit");
    QCOMPARE(result, "mit");
}

void TestCantoneseUtils::jyutpingToIPASimple()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("joeng4 sing4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "jœ̽ːŋ ˨ ˩  sɪŋ ˨ ˩");
#else
    QCOMPARE(result, "jœ̽ːŋ˨˩  sɪŋ˨˩");
#endif
}

void TestCantoneseUtils::jyutpingToIPARejectNoTone()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("joeng");
    QCOMPARE(result, "joeng");
}

void TestCantoneseUtils::jyutpingToIPARejectSingleLetter()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("a");
    QCOMPARE(result, "a");
}

void TestCantoneseUtils::jyutpingToIPARejectSpecialCharacter()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("-");
    QCOMPARE(result, "x");
}

void TestCantoneseUtils::jyutpingToIPANoSpaces()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("faa1sing4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "fäː ˥  sɪŋ ˨ ˩");
#else
    QCOMPARE(result, "fäː˥  sɪŋ˨˩");
#endif
}

void TestCantoneseUtils::jyutpingToIPASpacesToSegment()
{
    std::string result
        = CantoneseUtils::convertJyutpingToIPA("joeng4 sing4",
                                               /* useSpacesToSegment = */ true);
#ifdef Q_OS_MAC
    QCOMPARE(result, "jœ̽ːŋ ˨ ˩  sɪŋ ˨ ˩");
#else
    QCOMPARE(result, "jœ̽ːŋ˨˩  sɪŋ˨˩");
#endif
}

void TestCantoneseUtils::jyutpingToIPAPreprocessInitial()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("zyu2 sung3");
#ifdef Q_OS_MAC
    QCOMPARE(result, "t͡ʃyː ˧ ˥  sʊŋ ˧");
#else
    QCOMPARE(result, "t͡ʃyː˧˥  sʊŋ˧");
#endif
}

void TestCantoneseUtils::jyutpingToIPASpecialSyllable()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("m4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "m̩ ˨ ˩");
#else
    QCOMPARE(result, "m̩˨˩");
#endif
}

void TestCantoneseUtils::jyutpingToIPACheckedTone()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA(
        "sik6 si2 o1 faan6");
#ifdef Q_OS_MAC
    QCOMPARE(result, "sɪk̚ ˨  siː ˧ ˥  ɔː ˥  fäːn ˨");
#else
    QCOMPARE(result, "sɪk̚˨  siː˧˥  ɔː˥  fäːn˨");
#endif
}

void TestCantoneseUtils::jyutpingToIPASpecialFinal()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("uk1 kei2 jan4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "ʊk̚ ˥  kʰei̯ ˧ ˥  jɐn ˨ ˩");
#else
    QCOMPARE(result, "ʊk̚˥  kʰei̯˧˥  jɐn˨˩");
#endif
}
void TestCantoneseUtils::jyutpingToIPATones()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA(
        "saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6");
#ifdef Q_OS_MAC
    QCOMPARE(result,
             "säːm ˥  kɐu̯ ˧ ˥  sei̯ ˧  lɪŋ ˨ ˩  ŋ̍ ˩ ˧  jiː ˨  t͡sʰɐt̚ ˥  päːt̚ ˧  "
             "lʊk̚ ˨");
#else
    QCOMPARE(result,
             "säːm˥  kɐu̯˧˥  sei̯˧  lɪŋ˨˩  ŋ̍˩˧  jiː˨  t͡sʰɐt̚˥  päːt̚˧  lʊk̚˨");
#endif
}
void TestCantoneseUtils::jyutpingToIPANoTone()
{
    std::string result = CantoneseUtils::convertJyutpingToIPA("mok");
    QCOMPARE(result, "mok");
}

void TestCantoneseUtils::segmentJyutpingSimple()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m4 goi1", result);
    std::vector<std::string> expected = {"m4", "goi1"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingNoDigits()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m goi", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingNoSpaces()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m4goi1", result);
    std::vector<std::string> expected = {"m4", "goi1"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingNoDigitsNoSpaces()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("mgoi", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingNoDigitsApostrophe()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m'aam", result);
    std::vector<std::string> expected = {"m", "aam"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingDigitsApostrophe()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m4'aam", result);
    std::vector<std::string> expected = {"m4", "aam"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingRemoveSpecialCharacters()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m*goi", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingKeepGlobCharacters()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m* goi",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingKeepGlobCharactersNoWhitespace()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m*goi",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "*", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingKeepMultipleGlobCharacters()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m?* goi",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "?", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingKeepMultipleGlobCharactersWhitespace()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m? * goi",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "? ", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::
    segmentJyutpingKeepMultipleGlobCharactersWhitespaceSurround()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m ? * goi",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", " ? ", "* ", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingGlobCharactersTrimWhitespace()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m  ?            *      goi",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", " ? ", "* ", "goi"};
    QCOMPARE(result, expected);

    CantoneseUtils::segmentJyutping("m?* ????",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    expected = {"m", "?", "* ", "?", "?", "?", "?"};
    QCOMPARE(result, expected);

    CantoneseUtils::segmentJyutping("m * ????*",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    expected = {"m", " * ", "?", "?", "?", "?", "*"};
    QCOMPARE(result, expected);

    CantoneseUtils::segmentJyutping("m? goi*",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    expected = {"m", "? ", "goi", "*"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingKeepSpecialCharacters()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m？ goi",
                                    result,
                                    /* removeSpecialCharacters = */ false);
    std::vector<std::string> expected = {"m", "？", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingRemoveWhitespace()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("  m                           goi      ",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingLower()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("mGoI", result);
    std::vector<std::string> expected = {"m", "goi"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingLowerWithDigits()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("m4GoI1", result);
    std::vector<std::string> expected = {"m4", "goi1"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingMultipleFinalsVowelsOnly()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("aaaa",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"aa", "aa"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingMultipleFinalsConsonantsOnly()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("ngng",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"ng", "ng"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingMultipleFinals()
{
    std::vector<std::string> result;
    CantoneseUtils::segmentJyutping("amam",
                                    result,
                                    /* removeSpecialCharacters = */ true,
                                    /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"am", "am"};
    QCOMPARE(result, expected);
}

void TestCantoneseUtils::segmentJyutpingInvalidTone()
{
    std::vector<std::string> result;
    bool valid
        = CantoneseUtils::segmentJyutping("hang0",
                                          result,
                                          /* removeSpecialCharacters = */ true,
                                          /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"hang0"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);

    valid = CantoneseUtils::segmentJyutping("hang7",
                                            result,
                                            /* removeSpecialCharacters = */ true,
                                            /* removeGlobCharacters = */ false);
    expected = {"hang7"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);
}

void TestCantoneseUtils::segmentJyutpingGarbage()
{
    std::vector<std::string> result;
    bool valid
        = CantoneseUtils::segmentJyutping("kljnxclkjvnl",
                                          result,
                                          /* removeSpecialCharacters = */ true,
                                          /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"kljnxclkjvnl"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);
}

QTEST_APPLESS_MAIN(TestCantoneseUtils)

#include "tst_cantoneseutils.moc"
