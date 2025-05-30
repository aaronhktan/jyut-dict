#include <QtTest>

#include "logic/utils/mandarinutils.h"

class TestMandarinUtils : public QObject
{
    Q_OBJECT

public:
    TestMandarinUtils();
    ~TestMandarinUtils();

private slots:
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
    void segmentPinyinInvalidTone();
    void segmentPinyinGarbage();

    void soundChangeZ();
    void soundChangeC();
    void soundChangeS();
    void soundChangeN();
    void soundChangeR();
    void soundChangeLN();
    void soundChangeLNR();
    void soundChangeAng();
    void soundChangeEng();
    void soundChangeIng();
};

TestMandarinUtils::TestMandarinUtils() {}

TestMandarinUtils::~TestMandarinUtils() {}

void TestMandarinUtils::prettyPinyinSimple()
{
    std::string result = MandarinUtils::createPrettyPinyin("shuai4 ge1");
    QCOMPARE(result, "shuài gē");
}

void TestMandarinUtils::prettyPinyinRejectNoTone()
{
    std::string result = MandarinUtils::createPrettyPinyin("ba");
    QCOMPARE(result, "ba");
}

void TestMandarinUtils::prettyPinyinRejectSingleLetter()
{
    std::string result = MandarinUtils::createPrettyPinyin("a");
    QCOMPARE(result, "a");
}

void TestMandarinUtils::prettyPinyinRejectSpecialCharacter()
{
    std::string result = MandarinUtils::createPrettyPinyin("-");
    QCOMPARE(result, "-");
}

void TestMandarinUtils::prettyPinyinSecondaryVowel()
{
    std::string result = MandarinUtils::createPrettyPinyin("hui4 tu2");
    QCOMPARE(result, "huì tú");
}

void TestMandarinUtils::prettyPinyinUmlaut()
{
    std::string result = MandarinUtils::createPrettyPinyin("nu:3 hai2");
    QCOMPARE(result, "nǚ hái");
}

void TestMandarinUtils::prettyPinyinTones()
{
    std::string result = MandarinUtils::createPrettyPinyin(
        "ma1 ma2 ma3 ma4 ma5");
    QCOMPARE(result, "mā má mǎ mà ma");
}
void TestMandarinUtils::prettyPinyinNoTone()
{
    std::string result = MandarinUtils::createPrettyPinyin("nu");
    QCOMPARE(result, "nu");
}

void TestMandarinUtils::numberedPinyinSimple()
{
    std::string result = MandarinUtils::createNumberedPinyin("nu:3 hai2");
    QCOMPARE(result, "nü3 hai2");
}

void TestMandarinUtils::pinyinWithVSimple()
{
    std::string result = MandarinUtils::createPinyinWithV("nu:3 hai2");
    QCOMPARE(result, "nv3 hai2");
}

void TestMandarinUtils::pinuyinToZhuyinSimple()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("ba1 da2 tong1");
    QCOMPARE(result, "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ");
}

void TestMandarinUtils::pinuyinToZhuyinRejectNoTone()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("ba");
    QCOMPARE(result, "ba");
}

void TestMandarinUtils::pinuyinToZhuyinRejectSingleLetter()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("a");
    QCOMPARE(result, "a");
}
void TestMandarinUtils::pinuyinToZhuyinRejectSpecialCharacter()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("-");
    QCOMPARE(result, "-");
}
void TestMandarinUtils::pinuyinToZhuyinNoSpaces()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("ba1da2tong1");
    QCOMPARE(result, "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ");
}
void TestMandarinUtils::pinuyinToZhuyinSpacesToSegment()
{
    std::string result
        = MandarinUtils::convertPinyinToZhuyin("ba1 da2 tong1",
                                               /* useSpacesToSegment = */ true);
    QCOMPARE(result, "ㄅㄚ ㄉㄚˊ ㄊㄨㄥ");
}
void TestMandarinUtils::pinuyinToZhuyinSpecialInitial()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("qu4");
    QCOMPARE(result, "ㄑㄩˋ");

    result = MandarinUtils::convertPinyinToZhuyin("chi1");
    QCOMPARE(result, "ㄔ");

    result = MandarinUtils::convertPinyinToZhuyin("ri4");
    QCOMPARE(result, "ㄖˋ");
}
void TestMandarinUtils::pinuyinToZhuyinSpecialFinals()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("hm5");
    QCOMPARE(result, "˙ㄏㄇ");

    result = MandarinUtils::convertPinyinToZhuyin("hng5");
    QCOMPARE(result, "˙ㄏㄫ");

    result = MandarinUtils::convertPinyinToZhuyin("er2");
    QCOMPARE(result, "ㄦˊ");
}
void TestMandarinUtils::pinuyinToZhuyinErhua()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("quanr1");
    QCOMPARE(result, "ㄑㄩㄢㄦ");
}
void TestMandarinUtils::pinuyinToZhuyinMalformed()
{
    std::string result = MandarinUtils::convertPinyinToZhuyin("chzng2 quanr1");
    QCOMPARE(result, "chzng2 ㄑㄩㄢㄦ");
}

void TestMandarinUtils::pinyinToIPASimple()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("ba1 da2 tong1");
#ifdef Q_OS_MAC
    QCOMPARE(result, "pä ˥ ˥  tä ˧ ˥  tʰʊŋ ˥ ˥");
#else
    QCOMPARE(result, "pä˥˥  tä˧˥  tʰʊŋ˥˥");
#endif
}

void TestMandarinUtils::pinyinToIPARejectNoTone()
{
    std::string result = MandarinUtils::convertPinyinToIPA("ba");
    QCOMPARE(result, "ba");
}

void TestMandarinUtils::pinyinToIPARejectSingleLetter()
{
    std::string result = MandarinUtils::convertPinyinToIPA("a");
    QCOMPARE(result, "a");
}

void TestMandarinUtils::pinyinToIPARejectSpecialCharacter()
{
    std::string result = MandarinUtils::convertPinyinToIPA("-");
    QCOMPARE(result, "-");
}

void TestMandarinUtils::pinyinToIPANoSpaces()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("ba1da2tong1");
#ifdef Q_OS_MAC
    QCOMPARE(result, "pä ˥ ˥  tä ˧ ˥  tʰʊŋ ˥ ˥");
#else
    QCOMPARE(result, "pä˥˥  tä˧˥  tʰʊŋ˥˥");
#endif
}

void TestMandarinUtils::pinyinToIPASpacesToSegment()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result
        = MandarinUtils::convertPinyinToIPA("ba1 da2 tong1",
                                            /* useSpacesToSegment = */ true);
#ifdef Q_OS_MAC
    QCOMPARE(result, "pä ˥ ˥  tä ˧ ˥  tʰʊŋ ˥ ˥");
#else
    QCOMPARE(result, "pä˥˥  tä˧˥  tʰʊŋ˥˥");
#endif
}

void TestMandarinUtils::pinyinToIPASpecialCaseNg()
{
    std::string result = MandarinUtils::convertPinyinToIPA("ng5");
#ifdef Q_OS_MAC
    QCOMPARE(result, "ŋ̍ ");
#else
    QCOMPARE(result, "ŋ̍");
#endif
}

void TestMandarinUtils::pinyinToIPASpecialCaseRi()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("ri4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "ʐ̩ ˥ ˩");
#else
    QCOMPARE(result, "ʐ̩˥˩");
#endif
}

void TestMandarinUtils::pinyinToIPASyllableWithV()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("nv3");
#ifdef Q_OS_MAC
    QCOMPARE(result, "ny ˨ ˩ ˦");
#else
    QCOMPARE(result, "ny˨˩˦");
#endif

    result = MandarinUtils::convertPinyinToIPA("qu4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "t͡ɕʰy ˥ ˩");
#else
    QCOMPARE(result, "t͡ɕʰy˥˩");
#endif
}

void TestMandarinUtils::pinyinToIPAVoicelessInitial()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("ge5");
#ifdef Q_OS_MAC
    QCOMPARE(result, "g̊ə ");
#else
    QCOMPARE(result, "g̊ə");
#endif

    result = MandarinUtils::convertPinyinToIPA("yi1 ge5");
#ifdef Q_OS_MAC
    QCOMPARE(result, "i ˥ ˥  g̊ə ˨");
#else
    QCOMPARE(result, "i˥˥  g̊ə˨");
#endif
}

void TestMandarinUtils::pinyinToIPAToneThree()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("ke3");
#ifdef Q_OS_MAC
    QCOMPARE(result, "kʰɤ ˨ ˩ ˦");
#else
    QCOMPARE(result, "kʰɤ˨˩˦");
#endif

    result = MandarinUtils::convertPinyinToIPA("ke3 yi3");
#ifdef Q_OS_MAC
    QCOMPARE(result, "kʰɤ ˨ ˩ ˦ ꜔ ꜒  i ˨ ˩ ˦ ꜕ ꜖ ( ꜓ )");
#else
    QCOMPARE(result, "kʰɤ˨˩˦꜔꜒  i˨˩˦꜕꜖(꜓)");
#endif
}

void TestMandarinUtils::pinyinToIPAToneFour()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("xia4 qu4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "ɕjä ˥ ˩ ꜒ ꜔  t͡ɕʰy ˥ ˩");
#else
    QCOMPARE(result, "ɕjä˥˩꜒꜔  t͡ɕʰy˥˩");
#endif

    result = MandarinUtils::convertPinyinToIPA("xia4");
#ifdef Q_OS_MAC
    QCOMPARE(result, "ɕjä ˥ ˩");
#else
    QCOMPARE(result, "ɕjä˥˩");
#endif
}

void TestMandarinUtils::pinyinToIPAOtherTone()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("ma1");
#ifdef Q_OS_MAC
    QCOMPARE(result, "mä ˥ ˥");
#else
    QCOMPARE(result, "mä˥˥");
#endif

    result = MandarinUtils::convertPinyinToIPA("ma2");
#ifdef Q_OS_MAC
    QCOMPARE(result, "mä ˧ ˥");
#else
    QCOMPARE(result, "mä˧˥");
#endif

    result = MandarinUtils::convertPinyinToIPA("ma5");
#ifdef Q_OS_MAC
    QCOMPARE(result, "mä ");
#else
    QCOMPARE(result, "mä");
#endif
}
void TestMandarinUtils::pinyinToIPAErhua()
{
#ifdef Q_OS_WINDOWS
    QSKIP("IPA tone comparison doesn't work on Windows, skipping test");
#endif
    std::string result = MandarinUtils::convertPinyinToIPA("huar1");
#ifdef Q_OS_MAC
    QCOMPARE(result, "xu̯ɑɻ ˥ ˥");
#else
    QCOMPARE(result, "xu̯ɑɻ˥˥");
#endif

    result = MandarinUtils::convertPinyinToIPA("quanr1");
#ifdef Q_OS_MAC
    QCOMPARE(result, "t͡ɕʰɥɑɻ ˥ ˥");
#else
    QCOMPARE(result, "t͡ɕʰɥɑɻ˥˥");
#endif
}

void TestMandarinUtils::soundChangeZ()
{
    std::vector<std::string> result{"zuan"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"z(h)!uang!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"zuan3"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"z(h)!uang!3"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"zuan?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"z(h)!uang!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeC()
{
    std::vector<std::string> result{"cong"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"c(h)!ong"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"cong1"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"c(h)!ong1"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"chong?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"c(h)!ong?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeS()
{
    std::vector<std::string> result{"se"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"s(h)!e"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"se2"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"s(h)!e2"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"se?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"s(h)!e?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeN()
{
    std::vector<std::string> result{"ni"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"(n|l)i"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"ni3"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(n|l)i3"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"ni?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(n|l)i?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeR()
{
    std::vector<std::string> result{"re"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"(l|r)e"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"re4"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|r)e4"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"re?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|r)e?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeLN()
{
    std::vector<std::string> result{"li"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"(l|n)i"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"lie4"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)ie4"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"liao?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)iao?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"liu"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)iu"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"lian2"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)iang!2"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"lin?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)ing!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"liang"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)iang!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"ling1"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)ing!1"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"lu:?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)u:?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"lu:e"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n)u:e"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeLNR()
{
    std::vector<std::string> result{"lang"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"(l|n|r)ang!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"lang4"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n|r)ang!4"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"lang?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"(l|n|r)ang!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeAng()
{
    std::vector<std::string> result{"bang"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"bang!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"bang1"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"bang!1"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"bang?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"bang!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"ban"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"bang!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"ban2"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"bang!2"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"ban?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"bang!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeEng()
{
    std::vector<std::string> result{"peng"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"peng!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"peng4"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"peng!4"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"peng?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"peng!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"pen"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"peng!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"pen1"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"peng!1"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"pen?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"peng!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::soundChangeIng()
{
    std::vector<std::string> result{"bing"};
    bool err = MandarinUtils::pinyinSoundChanges(result);
    std::vector<std::string> expected{"bing!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"bing3"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"bing!3"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"bing?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"bing!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"pin"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"ping!"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"pin1"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"ping!1"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);

    result = {"pin?"};
    err = MandarinUtils::pinyinSoundChanges(result);
    expected = {"ping!?"};
    QCOMPARE(result, expected);
    QCOMPARE(err, false);
}

void TestMandarinUtils::segmentPinyinSimple()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang3 dong1", result);
    std::vector<std::string> expected = {"guang3", "dong1"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinNoDigits()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang dong", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinNoSpaces()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang3dong1", result);
    std::vector<std::string> expected = {"guang3", "dong1"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinNoDigitsNoSpaces()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guangdong", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinNoDigitsApostrophe()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("xi'an", result);
    std::vector<std::string> expected = {"xi", "an"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinDigitsApostrophe()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("xi1'an", result);
    std::vector<std::string> expected = {"xi1", "an"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinRemoveSpecialCharacters()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang。dong？", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinKeepGlobCharacters()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang* dong?",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "* ", "dong", "?"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinKeepGlobCharactersNoWhitespace()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang*dong?",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "*", "dong", "?"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinKeepMultipleGlobCharacters()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang?* dong",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "?", "* ", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinKeepMultipleGlobCharactersWhitespace()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang? * dong",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "? ", "* ", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinKeepMultipleGlobCharactersWhitespaceSurround()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang ? * dong",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", " ? ", "* ", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinGlobCharactersTrimWhitespace()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang  ?            *      dong",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", " ? ", "* ", "dong"};
    QCOMPARE(result, expected);

    MandarinUtils::segmentPinyin("guang?* ?????",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    expected = {"guang", "?", "* ", "?", "?", "?", "?", "?"};
    QCOMPARE(result, expected);

    MandarinUtils::segmentPinyin("guang * ????*",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    expected = {"guang", " * ", "?", "?", "?", "?", "*"};
    QCOMPARE(result, expected);

    MandarinUtils::segmentPinyin("guang? dong*",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    expected = {"guang", "? ", "dong", "*"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinKeepSpecialCharacters()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guang？ dong1",
                                 result,
                                 /* removeSpecialCharacters = */ false);
    std::vector<std::string> expected = {"guang", "？", "dong1"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinRemoveWhitespace()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("  guang                           dong      ",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinLower()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("gUanGdOnG", result);
    std::vector<std::string> expected = {"guang", "dong"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinLowerWithDigits()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("guAng3dONg1", result);
    std::vector<std::string> expected = {"guang3", "dong1"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinMultipleFinalsVowelsOnly()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("ee",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"e", "e"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinMultipleFinals()
{
    std::vector<std::string> result;
    MandarinUtils::segmentPinyin("angang",
                                 result,
                                 /* removeSpecialCharacters = */ true,
                                 /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"ang", "ang"};
    QCOMPARE(result, expected);
}

void TestMandarinUtils::segmentPinyinInvalidTone()
{
    std::vector<std::string> result;
    bool valid
        = MandarinUtils::segmentPinyin("heng0",
                                       result,
                                       /* removeSpecialCharacters = */ true,
                                       /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"heng0"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);

    valid = MandarinUtils::segmentPinyin("heng6",
                                         result,
                                         /* removeSpecialCharacters = */ true,
                                         /* removeGlobCharacters = */ false);
    expected = {"heng6"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);
}

void TestMandarinUtils::segmentPinyinGarbage()
{
    std::vector<std::string> result;
    bool valid
        = MandarinUtils::segmentPinyin("kljnxclkjvnl",
                                       result,
                                       /* removeSpecialCharacters = */ true,
                                       /* removeGlobCharacters = */ false);
    std::vector<std::string> expected = {"kljnxclkjvnl"};
    QCOMPARE(result, expected);
    QCOMPARE(valid, false);
}

QTEST_APPLESS_MAIN(TestMandarinUtils)

#include "tst_mandarinutils.moc"
