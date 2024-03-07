#include <QtTest>

#include "logic/settings/settings.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/utils.h"

class TestChineseUtils : public QObject
{
    Q_OBJECT

public:
    TestChineseUtils();
    ~TestChineseUtils();

private slots:
    void applyColoursJyutping();
    void applyColoursPinyin();

    void compareStringsSimple();
    void compareStringsSingleMultibyteGrapheme();
    void compareStringsMultipleMultibyteGraphemes();
    void compareStringsMultibyteGraphemesWithAlpha();
    void compareStringsCompatibilityVariantNormalization();

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

    void prettyPinyinSimple();
    void prettyPinyinRejectNoTone();
    void prettyPinyinRejectSingleLetter();
    void prettyPinyinRejectSpecialCharacter();
    void prettyPinyinSecondaryVowel();
    void prettyPinyinUmlaut();
    void prettyPinyinTones();
    void prettyPinyinNoTone();

    void numberedPinyinSimple();

    void pinyinWithVSimple();

    void pinuyinToZhuyinSimple();
    void pinuyinToZhuyinRejectNoTone();
    void pinuyinToZhuyinRejectSingleLetter();
    void pinuyinToZhuyinRejectSpecialCharacter();
    void pinuyinToZhuyinNoSpaces();
    void pinuyinToZhuyinSpacesToSegment();
    void pinuyinToZhuyinSpecialInitial();
    void pinuyinToZhuyinSpecialFinals();
    void pinuyinToZhuyinErhua();
    void pinuyinToZhuyinMalformed();

    void pinyinToIPASimple();
    void pinyinToIPARejectNoTone();
    void pinyinToIPARejectSingleLetter();
    void pinyinToIPARejectSpecialCharacter();
    void pinyinToIPANoSpaces();
    void pinyinToIPASpacesToSegment();
    void pinyinToIPASpecialCaseNg();
    void pinyinToIPASpecialCaseRi();
    void pinyinToIPASyllableWithV();
    void pinyinToIPAVoicelessInitial();
    void pinyinToIPAToneThree();
    void pinyinToIPAToneFour();
    void pinyinToIPAOtherTone();
    void pinyinToIPAErhua();

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

    void segmentPinyinSimple();
    void segmentPinyinNoDigits();
    void segmentPinyinNoSpaces();
    void segmentPinyinNoDigitsNoSpaces();
    void segmentPinyinNoDigitsApostrophe();
    void segmentPinyinDigitsApostrophe();
    void segmentPinyinRemoveSpecialCharacters();
    void segmentPinyinKeepGlobCharacters();
    void segmentPinyinKeepGlobCharactersNoWhitespace();
    void segmentPinyinKeepMultipleGlobCharacters();
    void segmentPinyinKeepMultipleGlobCharactersWhitespace();
    void segmentPinyinKeepMultipleGlobCharactersWhitespaceSurround();
    void segmentPinyinGlobCharactersTrimWhitespace();
    void segmentPinyinKeepSpecialCharacters();
    void segmentPinyinRemoveWhitespace();
    void segmentPinyinLower();
    void segmentPinyinLowerWithDigits();
    void segmentPinyinMultipleFinalsVowelsOnly();
    void segmentPinyinMultipleFinals();
    void segmentPinyinGarbage();
};

TestChineseUtils::TestChineseUtils() {}

TestChineseUtils::~TestChineseUtils() {}

void TestChineseUtils::applyColoursJyutping()
{
    std::string text = "唔係";
    std::vector<int> tones = {4, 6};
    std::string result
        = ChineseUtils::applyColours(text,
                                     tones,
                                     Settings::defaultJyutpingToneColours,
                                     {},
                                     EntryColourPhoneticType::CANTONESE);
    std::string expected = "<font color=\""
                           + Settings::defaultJyutpingToneColours[4]
                           + "\">唔</font>" + "<font color=\""
                           + Settings::defaultJyutpingToneColours[6]
                           + "\">係</font>";
    QCOMPARE(result, expected);
}
void TestChineseUtils::applyColoursPinyin()
{
    std::string text = "不是";
    std::vector<int> tones = {2, 4};
    std::string result
        = ChineseUtils::applyColours(text,
                                     tones,
                                     {},
                                     Settings::defaultPinyinToneColours,
                                     EntryColourPhoneticType::MANDARIN);
    std::string expected = "<font color=\""
                           + Settings::defaultPinyinToneColours[2]
                           + "\">不</font>" + "<font color=\""
                           + Settings::defaultPinyinToneColours[4]
                           + "\">是</font>";
    QCOMPARE(result, expected);
}

void TestChineseUtils::compareStringsSimple()
{
    std::string result = ChineseUtils::compareStrings("語言藝術", "语言艺术");
    QCOMPARE(result, "语" + std::string{Utils::SAME_CHARACTER_STRING} + "艺术");
}

void TestChineseUtils::compareStringsSingleMultibyteGrapheme()
{
    std::string result = ChineseUtils::compareStrings("賵", "赗");
    QCOMPARE(result, "赗");
}

void TestChineseUtils::compareStringsMultipleMultibyteGraphemes()
{
    std::string result = ChineseUtils::compareStrings("齮齕", "𬺈龁");
    QCOMPARE(result, "𬺈龁");
}

void TestChineseUtils::compareStringsMultibyteGraphemesWithAlpha()
{
    std::string result = ChineseUtils::compareStrings("齮aaaa齕", "𬺈aaaa龁");
    QCOMPARE(result,
             "𬺈" + std::string{Utils::SAME_CHARACTER_STRING}
                 + Utils::SAME_CHARACTER_STRING + Utils::SAME_CHARACTER_STRING
                 + Utils::SAME_CHARACTER_STRING + "龁");
}

void TestChineseUtils::compareStringsCompatibilityVariantNormalization()
{
    std::string result = ChineseUtils::compareStrings("響", "響");
    QCOMPARE(result, Utils::SAME_CHARACTER_STRING);
}

void TestChineseUtils::jyutpingToYaleSimple()
{
    std::string result = ChineseUtils::convertJyutpingToYale("si1 zi2 saan1");
    QCOMPARE(result, "sī jí sāan");
}

void TestChineseUtils::jyutpingToYaleRejectNoTone()
{
    std::string result = ChineseUtils::convertJyutpingToYale("joeng");
    QCOMPARE(result, "joeng");
}

void TestChineseUtils::jyutpingToYaleRejectSingleLetter()
{
    std::string result = ChineseUtils::convertJyutpingToYale("a");
    QCOMPARE(result, "a");
}

void TestChineseUtils::jyutpingToYaleRejectSpecialCharacter()
{
    std::string result = ChineseUtils::convertJyutpingToYale("-");
    QCOMPARE(result, "x");
}

void TestChineseUtils::jyutpingToYaleNoSpaces()
{
    std::string result = ChineseUtils::convertJyutpingToYale("si1zi2saan1");
    QCOMPARE(result, "sī jí sāan");
}

void TestChineseUtils::jyutpingToYaleSpacesToSegment()
{
    std::string result
        = ChineseUtils::convertJyutpingToYale("si1 zi2 saan1",
                                              /* usSpacesToSegment = */ true);
    QCOMPARE(result, "sī jí sāan");
}

void TestChineseUtils::jyutpingToYaleSpecialFinal()
{
    std::string result = ChineseUtils::convertJyutpingToYale("goek3jyun5");
    QCOMPARE(result, "geuk yúhn");
}

void TestChineseUtils::jyutpingToYaleLightTone()
{
    std::string result = ChineseUtils::convertJyutpingToYale("lok6 jyu5");
    QCOMPARE(result, "lohk yúh");
}

void TestChineseUtils::jyutpingToYaleSpecialSyllable()
{
    std::string result = ChineseUtils::convertJyutpingToYale("m4 hai6");
    QCOMPARE(result, "m̀h haih");
}

void TestChineseUtils::jyutpingToYaleTones()
{
    std::string result = ChineseUtils::convertJyutpingToYale(
        "saam1 gau2 sei3 ling4 ng5 ji6 cat1 baat3 luk6");
    QCOMPARE(result, "sāam gáu sei lìhng ńgh yih chāt baat luhk");
}

void TestChineseUtils::jyutpingToYaleNoTone()
{
    std::string result = ChineseUtils::convertJyutpingToYale("mit");
    QCOMPARE(result, "mit");
}

void TestChineseUtils::jyutpingToIPASimple()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("joeng4 sing4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "jœ̽ːŋ ˨ ˩  sɪŋ ˨ ˩");
#else
    QCOMPARE(result, "jœ̽ːŋ˨˩  sɪŋ˨˩");
#endif
}

void TestChineseUtils::jyutpingToIPARejectNoTone()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("joeng");
    QCOMPARE(result, "joeng");
}

void TestChineseUtils::jyutpingToIPARejectSingleLetter()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("a");
    QCOMPARE(result, "a");
}

void TestChineseUtils::jyutpingToIPARejectSpecialCharacter()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("-");
    QCOMPARE(result, "x");
}

void TestChineseUtils::jyutpingToIPANoSpaces()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("faa1sing4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "fäː ˥  sɪŋ ˨ ˩");
#else
    QCOMPARE(result, "fäː˥  sɪŋ˨˩");
#endif
}

void TestChineseUtils::jyutpingToIPASpacesToSegment()
{
    std::string result
        = ChineseUtils::convertJyutpingToIPA("joeng4 sing4",
                                             /* useSpacesToSegment = */ true);
#ifdef Q_OS_MAC
    QCOMPARE(result, "jœ̽ːŋ ˨ ˩  sɪŋ ˨ ˩");
#else
    QCOMPARE(result, "jœ̽ːŋ˨˩  sɪŋ˨˩");
#endif
}

void TestChineseUtils::jyutpingToIPAPreprocessInitial()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("zyu2 sung3");
#ifdef Q_OS_MAC
    QCOMPARE(result, "t͡ʃyː ˧ ˥  sʊŋ ˧");
#else
    QCOMPARE(result, "t͡ʃyː˧˥  sʊŋ˧");
#endif
}

void TestChineseUtils::jyutpingToIPASpecialSyllable()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("m4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "m̩ ˨ ˩");
#else
    QCOMPARE(result, "m̩˨˩");
#endif
}

void TestChineseUtils::jyutpingToIPACheckedTone()
{
    std::string result = ChineseUtils::convertJyutpingToIPA(
        "sik6 si2 o1 faan6");
#ifdef Q_OS_MAC
    QCOMPARE(result, "sɪk̚ ˨  siː ˧ ˥  ɔː ˥  fäːn ˨");
#else
    QCOMPARE(result, "sɪk̚˨  siː˧˥  ɔː˥  fäːn˨");
#endif
}

void TestChineseUtils::jyutpingToIPASpecialFinal()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("uk1 kei2 jan4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "ʊk̚ ˥  kʰei̯ ˧ ˥  jɐn ˨ ˩");
#else
    QCOMPARE(result, "ʊk̚˥  kʰei̯˧˥  jɐn˨˩");
#endif
}
void TestChineseUtils::jyutpingToIPATones()
{
    std::string result = ChineseUtils::convertJyutpingToIPA(
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
void TestChineseUtils::jyutpingToIPANoTone()
{
    std::string result = ChineseUtils::convertJyutpingToIPA("mok");
    QCOMPARE(result, "mok");
}

void TestChineseUtils::prettyPinyinSimple()
{
    std::string result = ChineseUtils::createPrettyPinyin("shuai4 ge1");
    QCOMPARE(result, "shuài gē");
}

void TestChineseUtils::prettyPinyinRejectNoTone()
{
    std::string result = ChineseUtils::createPrettyPinyin("ba");
    QCOMPARE(result, "ba");
}

void TestChineseUtils::prettyPinyinRejectSingleLetter()
{
    std::string result = ChineseUtils::createPrettyPinyin("a");
    QCOMPARE(result, "a");
}

void TestChineseUtils::prettyPinyinRejectSpecialCharacter()
{
    std::string result = ChineseUtils::createPrettyPinyin("-");
    QCOMPARE(result, "-");
}

void TestChineseUtils::prettyPinyinSecondaryVowel()
{
    std::string result = ChineseUtils::createPrettyPinyin("hui4 tu2");
    QCOMPARE(result, "huì tú");
}

void TestChineseUtils::prettyPinyinUmlaut()
{
    std::string result = ChineseUtils::createPrettyPinyin("nu:3 hai2");
    QCOMPARE(result, "nǚ hái");
}

void TestChineseUtils::prettyPinyinTones()
{
    std::string result = ChineseUtils::createPrettyPinyin(
        "ma1 ma2 ma3 ma4 ma5");
    QCOMPARE(result, "mā má mǎ mà ma");
}
void TestChineseUtils::prettyPinyinNoTone()
{
    std::string result = ChineseUtils::createPrettyPinyin("nu");
    QCOMPARE(result, "nu");
}

void TestChineseUtils::numberedPinyinSimple()
{
    std::string result = ChineseUtils::createNumberedPinyin("nu:3 hai2");
    QCOMPARE(result, "nü3 hai2");
}

void TestChineseUtils::pinyinWithVSimple()
{
    std::string result = ChineseUtils::createPinyinWithV("nu:3 hai2");
    QCOMPARE(result, "nv3 hai2");
}

void TestChineseUtils::pinuyinToZhuyinSimple()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("ba1 da2 tong1");
    QCOMPARE(result, "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ");
}

void TestChineseUtils::pinuyinToZhuyinRejectNoTone()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("ba");
    QCOMPARE(result, "ba");
}

void TestChineseUtils::pinuyinToZhuyinRejectSingleLetter()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("a");
    QCOMPARE(result, "a");
}
void TestChineseUtils::pinuyinToZhuyinRejectSpecialCharacter()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("-");
    QCOMPARE(result, "-");
}
void TestChineseUtils::pinuyinToZhuyinNoSpaces()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("ba1da2tong1");
    QCOMPARE(result, "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ");
}
void TestChineseUtils::pinuyinToZhuyinSpacesToSegment()
{
    std::string result
        = ChineseUtils::convertPinyinToZhuyin("ba1 da2 tong1",
                                              /* useSpacesToSegment = */ true);
    QCOMPARE(result, "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ");
}
void TestChineseUtils::pinuyinToZhuyinSpecialInitial()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("qu4");
    QCOMPARE(result, "ㄑㄩˋ");

    result = ChineseUtils::convertPinyinToZhuyin("chi1");
    QCOMPARE(result, "ㄔ");

    result = ChineseUtils::convertPinyinToZhuyin("ri4");
    QCOMPARE(result, "ㄖˋ");
}
void TestChineseUtils::pinuyinToZhuyinSpecialFinals()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("hm5");
    QCOMPARE(result, "˙ㄏㄇ");

    result = ChineseUtils::convertPinyinToZhuyin("hng5");
    QCOMPARE(result, "˙ㄏㄫ");

    result = ChineseUtils::convertPinyinToZhuyin("er2");
    QCOMPARE(result, "ㄦˊ");
}
void TestChineseUtils::pinuyinToZhuyinErhua()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("quanr1");
    QCOMPARE(result, "ㄑㄩㄢㄦ");
}
void TestChineseUtils::pinuyinToZhuyinMalformed()
{
    std::string result = ChineseUtils::convertPinyinToZhuyin("chzng2 quanr1");
    QCOMPARE(result, "chzng2 ㄑㄩㄢㄦ");
}

void TestChineseUtils::pinyinToIPASimple()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("ba1 da2 tong1");
    qDebug() << QString::fromStdString(result);
    QCOMPARE(result, "pä˥˥  tä˧˥  tʰʊŋ˥˥");
}

void TestChineseUtils::pinyinToIPARejectNoTone()
{
    std::string result = ChineseUtils::convertPinyinToIPA("ba");
    QCOMPARE(result, "ba");
}

void TestChineseUtils::pinyinToIPARejectSingleLetter()
{
    std::string result = ChineseUtils::convertPinyinToIPA("a");
    QCOMPARE(result, "a");
}

void TestChineseUtils::pinyinToIPARejectSpecialCharacter()
{
    std::string result = ChineseUtils::convertPinyinToIPA("-");
    QCOMPARE(result, "-");
}

void TestChineseUtils::pinyinToIPANoSpaces()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("ba1da2tong1");
    qDebug() << result.c_str();
    QCOMPARE(result, "pä˥˥  tä˧˥  tʰʊŋ˥˥");
}

void TestChineseUtils::pinyinToIPASpacesToSegment()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result
        = ChineseUtils::convertPinyinToIPA("ba1 da2 tong1",
                                           /* useSpacesToSegment = */ true);
    qDebug() << result.c_str();
    QCOMPARE(result, "pä˥˥  tä˧˥  tʰʊŋ˥˥");
}

void TestChineseUtils::pinyinToIPASpecialCaseNg()
{
    std::string result = ChineseUtils::convertPinyinToIPA("ng5");
    qDebug() << result.c_str();
    QCOMPARE(result, "ŋ̍");
}

void TestChineseUtils::pinyinToIPASpecialCaseRi()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("ri4");
    qDebug() << result.c_str();
    QCOMPARE(result, "ʐ̩˥˩");
}

void TestChineseUtils::pinyinToIPASyllableWithV()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("nv3");
    qDebug() << result.c_str();
    QCOMPARE(result, "ny˨˩˦");

    result = ChineseUtils::convertPinyinToIPA("qu4");
    qDebug() << result.c_str();
    QCOMPARE(result, "t͡ɕʰy˥˩");
}

void TestChineseUtils::pinyinToIPAVoicelessInitial()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("ge5");
    qDebug() << result.c_str();
    QCOMPARE(result, "g̊ə");

    result = ChineseUtils::convertPinyinToIPA("yi1 ge5");
    qDebug() << result.c_str();
    QCOMPARE(result, "i˥˥  g̊ə˨");
}

void TestChineseUtils::pinyinToIPAToneThree()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("ke3");
    qDebug() << result.c_str();
    QCOMPARE(result, "kʰɤ˨˩˦");

    result = ChineseUtils::convertPinyinToIPA("ke3 yi3");
    qDebug() << result.c_str();
    QCOMPARE(result, "kʰɤ˨˩˦꜔꜒  i˨˩˦꜕꜖(꜓)");
}

void TestChineseUtils::pinyinToIPAToneFour()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("xia4 qu4");
    qDebug() << result.c_str();
    QCOMPARE(result, "ɕjä˥˩꜒꜔  t͡ɕʰy˥˩");

    result = ChineseUtils::convertPinyinToIPA("xia4");
    qDebug() << result.c_str();
    QCOMPARE(result, "ɕjä˥˩");
}

void TestChineseUtils::pinyinToIPAOtherTone()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("ma1");
    qDebug() << result.c_str();
    QCOMPARE(result, "mä˥˥");

    result = ChineseUtils::convertPinyinToIPA("ma2");
    qDebug() << result.c_str();
    QCOMPARE(result, "mä˧˥");

    result = ChineseUtils::convertPinyinToIPA("ma5");
    qDebug() << result.c_str();
    QCOMPARE(result, "mä");
}
void TestChineseUtils::pinyinToIPAErhua()
{
#if defined(Q_OS_WINDOWS)
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = ChineseUtils::convertPinyinToIPA("huar1");
    qDebug() << result.c_str();
    QCOMPARE(result, "xu̯ɑɻ˥˥");

    result = ChineseUtils::convertPinyinToIPA("quanr1");
    QCOMPARE(result, "t͡ɕʰɥɑɻ˥˥");
}

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

void TestChineseUtils::segmentPinyinSimple()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang3 dong1", result);
    std::vector<std::string> expected = {"guang3", "dong1"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinNoDigits()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang dong", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinNoSpaces()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang3dong1", result);
    std::vector<std::string> expected = {"guang3", "dong1"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinNoDigitsNoSpaces()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guangdong", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinNoDigitsApostrophe()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("xi'an", result);
    std::vector<std::string> expected = {"xi", "an"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinDigitsApostrophe()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("xi1'an", result);
    std::vector<std::string> expected = {"xi1", "an"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinRemoveSpecialCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang*dong!!", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinKeepGlobCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang* dong?",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "* ", "dong", "?"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinKeepGlobCharactersNoWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang*dong?",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "*", "dong", "?"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinKeepMultipleGlobCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang?* dong",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "?", "* ", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinKeepMultipleGlobCharactersWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang? * dong",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "? ", "* ", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinKeepMultipleGlobCharactersWhitespaceSurround()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang ? * dong",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", " ? ", "* ", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinGlobCharactersTrimWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang  ?            *      dong",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", " ? ", "* ", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinKeepSpecialCharacters()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guang？ dong1",
                                result,
                                /* removeSpecialCharacters = */ false);
    std::vector<std::string> expected = {"guang", "？", "dong1"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinRemoveWhitespace()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("  guang                           dong      ",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinLower()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("gUanGdOnG", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinLowerWithDigits()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("guAng3dONg1", result);
    std::vector<std::string> expected = {"guang3", "dong1"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinMultipleFinalsVowelsOnly()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("ee",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"e", "e"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinMultipleFinals()
{
    std::vector<std::string> result;
    ChineseUtils::segmentPinyin("angang",
                                result,
                                /* removeSpecialCharacters = */ true,
                                /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"ang", "ang"};
    QCOMPARE(result, expected);
}

void TestChineseUtils::segmentPinyinGarbage()
{
    std::vector<std::string> result;
    bool valid
        = ChineseUtils::segmentPinyin("kljnxclkjvnl",
                                      result,
                                      /* removeSpecialCharacters = */ true,
                                      /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"kljnxclkjvnl"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);
}

QTEST_APPLESS_MAIN(TestChineseUtils)

#include "tst_chineseutils.moc"
