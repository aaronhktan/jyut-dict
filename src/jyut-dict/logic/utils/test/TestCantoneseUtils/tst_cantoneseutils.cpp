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

    void autocorrectJyutpingHui();
    void autocorrectJyutpingCu();
    void autocorrectJyutpingX();
    void autocorrectJyutpingCh();
    void autocorrectJyutpingSh();
    void autocorrectJyutpingZh();
    void autocorrectJyutpingEungErngEong();
    void autocorrectJyutpingEui();
    void autocorrectJyutpingEuk();
    void autocorrectJyutpingEun();
    void autocorrectJyutpingEut();
    void autocorrectJyutpingEu();
    void autocorrectJyutpingErn();
    void autocorrectJyutpingOen();
    void autocorrectJyutpingAr();
    void autocorrectJyutpingEe();
    void autocorrectJyutpingAy();
    void autocorrectJyutpingOy();
    void autocorrectJyutpingOo();
    void autocorrectJyutpingOng();
    void autocorrectJyutpingYoung();
    void autocorrectJyutpingYue();
    void autocorrectJyutpingUe();
    void autocorrectJyutpingTsz();
    void autocorrectJyutpingCk();
    void autocorrectJyutpingEy();
    void autocorrectJyutpingOh();
    void autocorrectJyutpingOw();
    void autocorrectJyutpingUm();
    void autocorrectJyutpingYum();
    void autocorrectJyutpingYup();
    void autocorrectJyutpingUnsafe();
    void autocorrectJyutpingY();
    void autocorrectJyutpingYuk();
    void autocorrectJyutpingYung();
    void autocorrectJyutpingYun();
    void autocorrectJyutpingYut();
    void autocorrectJyutpingUi();
    void autocorrectJyutpingUn();
    void autocorrectJyutpingUt();
};

TestCantoneseUtils::TestCantoneseUtils() {}

TestCantoneseUtils::~TestCantoneseUtils() {}

void TestCantoneseUtils::jyutpingToYaleSimple()
{
    std::string result = CantoneseUtils::convertJyutpingToYale("si1 zi2 saan1");
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

void TestCantoneseUtils::autocorrectJyutpingHui()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("hui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "heoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("hui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "heoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("shui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "seoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("shui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "seoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingCu()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("cui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "(k)(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("cui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "(k)(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("cum",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "(k)am";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("cum",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "(k)am";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingX()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("xuet",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "s(yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("xuet",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "s(yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("xui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "s(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("xui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "s(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingCh()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("choeng",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "coeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("choeng",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "coeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("chi",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "ci";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("chi",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "ci";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingSh()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("shoeng",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "soeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("shoeng",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "soeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("shi",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "si";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("shi",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "si";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingZh()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("zhoeng",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zhoeng",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zhi",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "zi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zhi",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEungErngEong()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("zeung",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zeung",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zerng",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zerng",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zeong",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zeong",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEui()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("zeui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "zeoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zeui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zeoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEuk()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("seuk",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "soek";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("seuk",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "soek";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEun()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("ceun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "c(eo|yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("ceun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "c(eo|yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEut()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("seut",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "s(eo|yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("seut",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "s(eo|yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEu()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("zeu",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "z(e|y)u";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zeu",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "z(e|y)u";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingErn()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("zern",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "zeon";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zern",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zeon";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingOen()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("zoen",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "zeon";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zoen",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zeon";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingAr()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("char",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "caa";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("char",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "caa";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEe()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("see",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "si";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("see",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "si";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingAy()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("hay",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "hei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("hay",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "hei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingOy()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("choy",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "coi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("choy",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "coi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingOo()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("soot",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "s(y!u)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("soot",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "s(y!u)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingOng()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("fong",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "f(o|u)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("fong",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "f(o|u)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYoung()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("young",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "j(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("young",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "j(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYue()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yuet",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "j(ja|yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yuet",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "j(ja|yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingUe()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("tsuen",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "ts(yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("tsuen",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "c(yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingTsz()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("tsz",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "zi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("tsz",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingCk()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("back",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "bak";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("back",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "bak";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingEy()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("gey ",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "gei ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gey ",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gei ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gey'",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gei'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gey'",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gei'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gey",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gey",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("geyhey",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "geihei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("geyhey",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "geihei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the -ei combo
    err = CantoneseUtils::jyutpingAutocorrect("peylou",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "peilou";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("peylou",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "peilou";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the -e j- combo
    err = CantoneseUtils::jyutpingAutocorrect("yeye",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "je je";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yeye",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "je je";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + initial after the "ey"
    err = CantoneseUtils::jyutpingAutocorrect("beycaam",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "beicaam";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("beycaam",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "beicaam";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + non-initial after the "ey"
    err = CantoneseUtils::jyutpingAutocorrect("geye",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "ge je";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("geye",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "ge je";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingOh()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("moh ",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "mou ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("moh ",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "mou ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("moh'",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "mou'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("moh'",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "mou'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("moh",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "mou";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("moh",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "mou";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("ohoh",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "ouou";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("ohoh",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "ouou";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the -ou combo
    err = CantoneseUtils::jyutpingAutocorrect("nohdoi",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "noudoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("nohdoi",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "noudoi";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + initial after the "oh"
    err = CantoneseUtils::jyutpingAutocorrect("lohjan",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "loujan";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("lohjan",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "loujan";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + non-initial after the "oh"
    err = CantoneseUtils::jyutpingAutocorrect("lohon",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "lo hon";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("lohon",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "lo hon";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingOw()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("gow ",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "gau ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gow ",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gau ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gow'",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gau'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gow'",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gau'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the -au combo
    err = CantoneseUtils::jyutpingAutocorrect("mow",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "mau";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("mow",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "mau";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + initial after the "ow"
    err = CantoneseUtils::jyutpingAutocorrect("towgai",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "taugai";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("towgai",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "taugai";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + non-initial after the "ow"
    err = CantoneseUtils::jyutpingAutocorrect("howu",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "ho wu";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("ho wu",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "ho wu";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingUm()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("gum ",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "gam ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gum ",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gam ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gum'",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gam'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gum'",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gam'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the -am combo
    err = CantoneseUtils::jyutpingAutocorrect("bum",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "bam";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("bum",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "bam";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the -u m- combo
    err = CantoneseUtils::jyutpingAutocorrect("wumit",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "wumit";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("wumit",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "wumit";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + initial after the "um"
    err = CantoneseUtils::jyutpingAutocorrect("gumzau",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gamzau";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gumzau",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gamzau";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with ambiguous initial + non-initial after the "um"
    err = CantoneseUtils::jyutpingAutocorrect("guman",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gu man";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("guman",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gu man";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYum()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yum ",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "jam ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yum ",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jam ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yum'",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "jam'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yum'",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jam'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is valid for the -yu m- combo
    err = CantoneseUtils::jyutpingAutocorrect("cyumat",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "cyu mat";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("cyumat",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "cyu mat";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the jam combo
    err = CantoneseUtils::jyutpingAutocorrect("syuyum",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "syujam";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("syuyum",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "syujam";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYup()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yup ",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "jap ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yup ",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jap ";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yup'",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "jap'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yup'",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jap'";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is valid for the -yu p- combo
    err = CantoneseUtils::jyutpingAutocorrect("syupei",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "syu pei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("syupei",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "syu pei";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    // Test with an initial that is only valid for the jap combo
    err = CantoneseUtils::jyutpingAutocorrect("zeonyup",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "zeonjap";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("zeonyup",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "zeonjap";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingUnsafe()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("kwun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "kw(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("kwun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "g(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("baakwun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "baakw(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("baakwun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "baag(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("tsuen",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "ts(yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("tsuen",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "c(yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("wantsuen",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "wants(yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("wantsuen",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "wanc(yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingY()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yaang",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "jaang";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yaang",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jaang";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yeng",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "jeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yeng",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yuen",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "j(ja|jyu|yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yuen",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "j(ja|jyu|yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYuk()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yuk",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "juk";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yuk",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "juk";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yuk",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = " juk";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yuk",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = " juk";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("geyyuk",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "geijuk";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("geyyuk",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "geijuk";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYung()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yung",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "j(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yung",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "j(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yung",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = " j(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yung",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = " j(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gumyung",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gamj(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gumyung",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gamj(y!u|a|eo)ng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYun()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "jan";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jan";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = " jan";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = " jan";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gumyun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gam(ja|jyu|yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gumyun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gam(ja|jyu|yu)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingYut()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("yut",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "jat";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yut",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jat";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yut",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = " jat";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" yut",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = " jat";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gamyut",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gam(ja|yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gamyut",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gam(ja|yu)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingUi()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("gui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "g(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "g(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" gui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = " g(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" gui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = " g(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yumsui",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "jams(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("yumsui",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "jams(eo|u)i";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingUn()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("gun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "g(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "g(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" gun",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = " g(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" gun",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = " g(y!u|a|eo)n";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gunzoeng",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "g(y!u|a|eo)nzoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gunzoeng",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "g(y!u|a|eo)nzoeng";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestCantoneseUtils::autocorrectJyutpingUt()
{
    QString result;
    bool err
        = CantoneseUtils::jyutpingAutocorrect("gut",
                                              result,
                                              /* unsafeSubstitutions */ false);
    QString expected = "g(u|a)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gut",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "g(u|a)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" gut",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = " g(u|a)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect(" gut",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = " g(u|a)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gamgut",
                                              result,
                                              /* unsafeSubstitutions */ false);
    expected = "gamg(u|a)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    err = CantoneseUtils::jyutpingAutocorrect("gamgut",
                                              result,
                                              /* unsafeSubstitutions */ true);
    expected = "gamg(u|a)t";
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

QTEST_APPLESS_MAIN(TestCantoneseUtils)

#include "tst_cantoneseutils.moc"
