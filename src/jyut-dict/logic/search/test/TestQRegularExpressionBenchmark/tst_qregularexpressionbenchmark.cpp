#include "logic/utils/cantoneseutils.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/mandarinutils.h"

#include <QRegularExpression>
#include <QtTest>

#include <array>
#include <span>
#include <string>
#include <vector>

namespace {

QString buildFuzzyJyutpingRegex(const QString &searchTerm)
{
    std::vector<std::string> syllables;
    CantoneseUtils::segmentJyutping(searchTerm,
                                    syllables,
                                    /* removeSpecialCharacters */ true,
                                    /* removeGlobCharacters */ false,
                                    /* removeRegexCharacters */ false);
    CantoneseUtils::jyutpingSoundChanges(syllables);

    const std::string query
        = ChineseUtils::constructRomanisationQuery(syllables, "?");

    QString pattern = QString::fromStdString(query);
    pattern.replace("*", ".*");
    pattern.replace("?", ".");
    pattern.replace("!", "?");
    return "^" + pattern + ".*$";
}

QString buildFuzzyPinyinRegex(const QString &searchTerm)
{
    QString processedSearchTerm = searchTerm;
    processedSearchTerm.replace("v", "u:");
    processedSearchTerm.replace("ü", "u:");

    std::vector<std::string> syllables;
    MandarinUtils::segmentPinyin(processedSearchTerm,
                                 syllables,
                                 /* removeSpecialCharacters */ true,
                                 /* removeGlobCharacters */ false);
    MandarinUtils::pinyinSoundChanges(syllables);

    const std::string query
        = ChineseUtils::constructRomanisationQuery(syllables, "?");

    QString pattern = QString::fromStdString(query);
    pattern.replace("*", ".*");
    pattern.replace("?", ".");
    pattern.replace("!", "?");
    return "^" + pattern + ".*$";
}

QStringList buildCorpus(std::span<const QString> baseEntries, int repeatCount)
{
    QStringList corpus;
    corpus.reserve(static_cast<qsizetype>(baseEntries.size()) * repeatCount);
    for (int i = 0; i < repeatCount; ++i) {
        for (const QString &entry : baseEntries) {
            corpus.push_back(entry);
        }
    }
    return corpus;
}

} // namespace

class TestQRegularExpressionBenchmark : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void rawQRegularExpressionBenchmark_data();
    void rawQRegularExpressionBenchmark();

private:
    QStringList _jyutpingCorpus;
    QStringList _pinyinCorpus;
};

void TestQRegularExpressionBenchmark::initTestCase()
{
    static const std::array<QString, 16> jyutpingEntries = {
        "baak6 wan4 saan1",
        "baak6 waan4 saan1",
        "gwong2 dung1 waa2",
        "gwong2 dung1 waa2",
        "zi1 dung6 ce1",
        "jyu5 jin4 hok6",
        "soeng6 hoi2",
        "zung1 gwok3",
        "hei3 hau6 bing3",
        "nei5 hou2",
        "nei5 hou2 maa3",
        "sik6 faan6",
        "daai6 hok6 saang1",
        "jyu4 gwo2",
        "soeng2 heoi3",
        "jyun4 lei5"
    };
    static const std::array<QString, 16> pinyinEntries = {
        "bai2 yun2 shan1",
        "guang3 dong1 hua4",
        "guang3 zhou1 hua4",
        "zi4 dong4 che1",
        "yu3 yan2 xue2",
        "shang4 hai3",
        "zhong1 guo2",
        "tian1 qi4",
        "ni3 hao3",
        "ni3 hao3 ma5",
        "chi1 fan4",
        "da4 xue2 sheng1",
        "ru2 guo3",
        "xiang3 qu4",
        "yuan2 li3",
        "guang3 bo1"
    };

    _jyutpingCorpus = buildCorpus(jyutpingEntries, 2048);
    _pinyinCorpus = buildCorpus(pinyinEntries, 2048);
}

void TestQRegularExpressionBenchmark::rawQRegularExpressionBenchmark_data()
{
    QTest::addColumn<QString>("pattern");
    QTest::addColumn<QStringList>("subjects");
    QTest::addColumn<int>("expectedMinMatches");

    QTest::newRow("jyutping-fuzzy")
        << buildFuzzyJyutpingRegex("gwong dung waa")
        << _jyutpingCorpus
        << 2000;

    QTest::newRow("pinyin-fuzzy")
        << buildFuzzyPinyinRegex("guang dong hua")
        << _pinyinCorpus
        << 2000;
}

void TestQRegularExpressionBenchmark::rawQRegularExpressionBenchmark()
{
    QFETCH(QString, pattern);
    QFETCH(QStringList, subjects);
    QFETCH(int, expectedMinMatches);

    const QRegularExpression regex{pattern, QRegularExpression::DontCaptureOption};
    QVERIFY2(regex.isValid(), qPrintable(regex.errorString()));

    int matchCount = 0;
    QBENCHMARK {
        int localMatchCount = 0;
        for (const QString &subject : subjects) {
            if (subject.contains(regex)) {
                ++localMatchCount;
            }
        }
        matchCount = localMatchCount;
    }

    QVERIFY(matchCount >= expectedMinMatches);
}

QTEST_MAIN(TestQRegularExpressionBenchmark)

#include "tst_qregularexpressionbenchmark.moc"
