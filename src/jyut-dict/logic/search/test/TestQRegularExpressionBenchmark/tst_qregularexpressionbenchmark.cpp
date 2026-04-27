#include "logic/utils/cantoneseutils.h"
#include "logic/utils/chineseutils.h"
#include "logic/utils/mandarinutils.h"

#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QRegularExpression>
#include <QTemporaryDir>
#include <QtTest>

#include <array>
#include <optional>
#include <span>
#include <string>
#include <vector>

using Utf8Corpus = std::vector<QByteArray>;

Q_DECLARE_METATYPE(Utf8Corpus)

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

QString buildJyutpingGlob(const QString &searchTerm)
{
    std::vector<std::string> syllables;
    CantoneseUtils::segmentJyutping(searchTerm,
                                    syllables,
                                    /* removeSpecialCharacters */ true,
                                    /* removeGlobCharacters */ false,
                                    /* removeRegexCharacters */ true);
    const std::string query
        = ChineseUtils::constructRomanisationQuery(syllables, "?");
    return QString::fromStdString(query) + "*";
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

QString buildPinyinGlob(const QString &searchTerm)
{
    QString processedSearchTerm = searchTerm;
    processedSearchTerm.replace("v", "u:");
    processedSearchTerm.replace("ü", "u:");

    std::vector<std::string> syllables;
    MandarinUtils::segmentPinyin(processedSearchTerm,
                                 syllables,
                                 /* removeSpecialCharacters */ true,
                                 /* removeGlobCharacters */ false);
    const std::string query
        = ChineseUtils::constructRomanisationQuery(syllables, "?");
    return QString::fromStdString(query) + "*";
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

bool executeSingleValueQuery(QSqlDatabase &db,
                             const QString &sql,
                             const QString &bindValue,
                             int &valueOut)
{
    QSqlQuery query{db};
    if (!query.prepare(sql)) {
        qWarning() << "prepare failed:" << query.lastError();
        return false;
    }
    query.addBindValue(bindValue);
    if (!query.exec()) {
        qWarning() << "exec failed:" << query.lastError();
        return false;
    }
    if (!query.next()) {
        qWarning() << "query returned no rows:" << sql;
        return false;
    }
    valueOut = query.value(0).toInt();
    return true;
}

QStringList getExplainQueryPlan(QSqlDatabase &db,
                                const QString &sql,
                                const QString &bindValue)
{
    QSqlQuery query{db};
    QStringList lines;
    if (!query.prepare("EXPLAIN QUERY PLAN " + sql)) {
        lines << ("prepare failed: " + query.lastError().text());
        return lines;
    }
    query.addBindValue(bindValue);
    if (!query.exec()) {
        lines << ("exec failed: " + query.lastError().text());
        return lines;
    }
    while (query.next()) {
        lines << query.value(3).toString();
    }
    return lines;
}

} // namespace

class TestQRegularExpressionBenchmark : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void rawQRegularExpressionBenchmark_data();
    void rawQRegularExpressionBenchmark();
    void utf8ToQStringContainsBenchmark_data();
    void utf8ToQStringContainsBenchmark();
    void sqliteBenchmark_data();
    void sqliteBenchmark();
    void explainQueryPlan_data();
    void explainQueryPlan();

private:
    bool createBenchmarkDatabase();

    QStringList _jyutpingCorpus;
    QStringList _pinyinCorpus;
    Utf8Corpus _jyutpingUtf8Corpus;
    Utf8Corpus _pinyinUtf8Corpus;
    std::optional<QTemporaryDir> _databaseDir;
    QString _databasePath;
    QString _connectionName;
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
    qRegisterMetaType<Utf8Corpus>();
    _jyutpingUtf8Corpus.reserve(_jyutpingCorpus.size());
    for (const QString &entry : _jyutpingCorpus) {
        _jyutpingUtf8Corpus.push_back(entry.toUtf8());
    }
    _pinyinUtf8Corpus.reserve(_pinyinCorpus.size());
    for (const QString &entry : _pinyinCorpus) {
        _pinyinUtf8Corpus.push_back(entry.toUtf8());
    }

    QVERIFY(createBenchmarkDatabase());
}

void TestQRegularExpressionBenchmark::cleanupTestCase()
{
    if (!_connectionName.isEmpty()) {
        QSqlDatabase::database(_connectionName, /* open = */ false).close();
        QSqlDatabase::removeDatabase(_connectionName);
    }
}

bool TestQRegularExpressionBenchmark::createBenchmarkDatabase()
{
    _databaseDir.emplace();
    if (!_databaseDir->isValid()) {
        qWarning() << "Failed to create temporary directory";
        return false;
    }

    _databasePath = _databaseDir->path() + "/benchmark.db";
    _connectionName = "TestQRegularExpressionBenchmark";

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", _connectionName);
    db.setDatabaseName(_databasePath);
    db.setConnectOptions("QSQLITE_ENABLE_REGEXP");
    if (!db.open()) {
        qWarning() << "Failed to open database:" << db.lastError();
        return false;
    }

    QSqlQuery query{db};
    if (!query.exec("CREATE TABLE entries ("
                    "  entry_id INTEGER PRIMARY KEY,"
                    "  jyutping TEXT,"
                    "  pinyin TEXT"
                    ")")) {
        qWarning() << query.lastError();
        return false;
    }
    if (!query.exec("CREATE INDEX entries_jyutping_idx ON entries(jyutping)")) {
        qWarning() << query.lastError();
        return false;
    }
    if (!query.exec("CREATE INDEX entries_pinyin_idx ON entries(pinyin)")) {
        qWarning() << query.lastError();
        return false;
    }
    if (!query.prepare("INSERT INTO entries (jyutping, pinyin) VALUES (?, ?)")) {
        qWarning() << query.lastError();
        return false;
    }

    for (int i = 0; i < _jyutpingCorpus.size() && i < _pinyinCorpus.size(); ++i) {
        query.addBindValue(_jyutpingCorpus.at(i));
        query.addBindValue(_pinyinCorpus.at(i));
        if (!query.exec()) {
            qWarning() << "insert failed:" << query.lastError();
            return false;
        }
    }

    return true;
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

void TestQRegularExpressionBenchmark::utf8ToQStringContainsBenchmark_data()
{
    QTest::addColumn<QString>("pattern");
    QTest::addColumn<Utf8Corpus>("subjects");
    QTest::addColumn<int>("expectedMinMatches");

    QTest::newRow("jyutping-fuzzy")
        << buildFuzzyJyutpingRegex("gwong dung waa")
        << _jyutpingUtf8Corpus
        << 2000;

    QTest::newRow("pinyin-fuzzy")
        << buildFuzzyPinyinRegex("guang dong hua")
        << _pinyinUtf8Corpus
        << 2000;
}

void TestQRegularExpressionBenchmark::utf8ToQStringContainsBenchmark()
{
    QFETCH(QString, pattern);
    QFETCH(Utf8Corpus, subjects);
    QFETCH(int, expectedMinMatches);

    const QRegularExpression regex{pattern, QRegularExpression::DontCaptureOption};
    QVERIFY2(regex.isValid(), qPrintable(regex.errorString()));

    int matchCount = 0;
    QBENCHMARK {
        int localMatchCount = 0;
        for (const QByteArray &subjectUtf8 : subjects) {
            const QString subject = QString::fromUtf8(subjectUtf8);
            if (subject.contains(regex)) {
                ++localMatchCount;
            }
        }
        matchCount = localMatchCount;
    }

    QVERIFY(matchCount >= expectedMinMatches);
}

void TestQRegularExpressionBenchmark::sqliteBenchmark_data()
{
    QTest::addColumn<QString>("sql");
    QTest::addColumn<QString>("bindValue");
    QTest::addColumn<int>("expectedMinMatches");

    const QString jyutpingRegex = buildFuzzyJyutpingRegex("gwong dung waa");
    const QString pinyinRegex = buildFuzzyPinyinRegex("guang dong hua");
    const QString jyutpingGlob = buildJyutpingGlob("gwong dung waa");
    const QString pinyinGlob = buildPinyinGlob("guang dong hua");

    QTest::newRow("jyutping-regexp")
        << QString{"SELECT count(*) FROM entries WHERE jyutping REGEXP ?"}
        << jyutpingRegex
        << 2000;
    QTest::newRow("jyutping-glob")
        << QString{"SELECT count(*) FROM entries WHERE jyutping GLOB ?"}
        << jyutpingGlob
        << 2000;
    QTest::newRow("pinyin-regexp")
        << QString{"SELECT count(*) FROM entries WHERE pinyin REGEXP ?"}
        << pinyinRegex
        << 2000;
    QTest::newRow("pinyin-glob")
        << QString{"SELECT count(*) FROM entries WHERE pinyin GLOB ?"}
        << pinyinGlob
        << 2000;
}

void TestQRegularExpressionBenchmark::sqliteBenchmark()
{
    QFETCH(QString, sql);
    QFETCH(QString, bindValue);
    QFETCH(int, expectedMinMatches);

    QSqlDatabase db = QSqlDatabase::database(_connectionName);
    QVERIFY(db.isOpen());

    int matchCount = 0;
    QBENCHMARK {
        QVERIFY(executeSingleValueQuery(db, sql, bindValue, matchCount));
    }

    QVERIFY(matchCount >= expectedMinMatches);
}

void TestQRegularExpressionBenchmark::explainQueryPlan_data()
{
    QTest::addColumn<QString>("label");
    QTest::addColumn<QString>("sql");
    QTest::addColumn<QString>("bindValue");

    const QString jyutpingRegex = buildFuzzyJyutpingRegex("gwong dung waa");
    const QString pinyinRegex = buildFuzzyPinyinRegex("guang dong hua");
    const QString jyutpingGlob = buildJyutpingGlob("gwong dung waa");
    const QString pinyinGlob = buildPinyinGlob("guang dong hua");

    QTest::newRow("jyutping-regexp")
        << QString{"jyutping-regexp"}
        << QString{"SELECT count(*) FROM entries WHERE jyutping REGEXP ?"}
        << jyutpingRegex;
    QTest::newRow("jyutping-glob")
        << QString{"jyutping-glob"}
        << QString{"SELECT count(*) FROM entries WHERE jyutping GLOB ?"}
        << jyutpingGlob;
    QTest::newRow("pinyin-regexp")
        << QString{"pinyin-regexp"}
        << QString{"SELECT count(*) FROM entries WHERE pinyin REGEXP ?"}
        << pinyinRegex;
    QTest::newRow("pinyin-glob")
        << QString{"pinyin-glob"}
        << QString{"SELECT count(*) FROM entries WHERE pinyin GLOB ?"}
        << pinyinGlob;
}

void TestQRegularExpressionBenchmark::explainQueryPlan()
{
    QFETCH(QString, label);
    QFETCH(QString, sql);
    QFETCH(QString, bindValue);

    QSqlDatabase db = QSqlDatabase::database(_connectionName);
    QVERIFY(db.isOpen());

    const QStringList planLines = getExplainQueryPlan(db, sql, bindValue);
    QVERIFY(!planLines.isEmpty());

    qInfo().noquote() << "EXPLAIN QUERY PLAN" << label;
    for (const QString &line : planLines) {
        qInfo().noquote() << "  " << line;
    }
}

QTEST_MAIN(TestQRegularExpressionBenchmark)

#include "tst_qregularexpressionbenchmark.moc"
