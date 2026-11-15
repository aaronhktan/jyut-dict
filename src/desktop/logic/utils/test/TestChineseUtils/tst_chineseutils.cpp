#include <QtTest>

#include "logic/settings/settings.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/mandarinutils.h"
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

    void constructRomanisationQuerySingleSyllable();
    void constructRomanisationQueryMultiSyllable();
    void constructRomanisationQueryGlobCharacters();
    void constructRomanisationQueryOnlyGlobCharacters();
};

TestChineseUtils::TestChineseUtils() {}

TestChineseUtils::~TestChineseUtils() {}

void TestChineseUtils::applyColoursJyutping()
{
    std::string text = "唔係";
    std::vector<uint8_t> tones = {4, 6};
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
    std::vector<uint8_t> tones = {2, 4};
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

void TestChineseUtils::constructRomanisationQuerySingleSyllable()
{
    std::string result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se"}, "?");
    QCOMPARE(result, "se?");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se2"}, "?");
    QCOMPARE(result, "se2");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se*"}, "?");
    QCOMPARE(result, "se*?");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se?"}, "?");
    QCOMPARE(result, "se??");
}

void TestChineseUtils::constructRomanisationQueryMultiSyllable()
{
    std::string result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se", "dak"}, "?");
    QCOMPARE(result, "se? dak?");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se2", "dak1"}, "?");
    QCOMPARE(result, "se2 dak1");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se*", "dak*"}, "?");
    QCOMPARE(result, "se*? dak*?");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se?", "dak?"}, "?");
    QCOMPARE(result, "se?? dak??");
}

void TestChineseUtils::constructRomanisationQueryGlobCharacters()
{
    std::string result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se", " *"}, "?");
    QCOMPARE(result, "se? *");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se", " ?", "?", "?"}, "?");
    QCOMPARE(result, "se? ???");

    result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"se", " ?", "?", "? ", "dak"}, "?");
    QCOMPARE(result, "se? ??? dak?");
}

void TestChineseUtils::constructRomanisationQueryOnlyGlobCharacters()
{
    std::string result = ChineseUtils::constructRomanisationQuery(
        std::vector<std::string>{"?",
                                 "?",
                                 "?",
                                 "?",
                                 "? ",
                                 "?",
                                 "?",
                                 "?",
                                 "?"},
        "?");
    QCOMPARE(QString::fromStdString(result), "????? ????");
}

QTEST_APPLESS_MAIN(TestChineseUtils)

#include "tst_chineseutils.moc"
