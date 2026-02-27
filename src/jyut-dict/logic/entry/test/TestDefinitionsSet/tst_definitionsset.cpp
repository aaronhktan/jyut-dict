#include <QtTest>

#include "logic/entry/definitionsset.h"

#include "logic/dictionary/dictionarysource.h"

class TestDefinitionsSet : public QObject
{
    Q_OBJECT

public:
    TestDefinitionsSet();
    ~TestDefinitionsSet();

private slots:
    void isEmpty();
    void addDefinitions();

    void getSources();
    void getDefinitions();
};

TestDefinitionsSet::TestDefinitionsSet() {}

TestDefinitionsSet::~TestDefinitionsSet() {}

void TestDefinitionsSet::isEmpty()
{
    DefinitionsSet set{"CC-CEDICT", {}};
    QCOMPARE(set.isEmpty(), true);
}

void TestDefinitionsSet::addDefinitions()
{
    std::vector<Definition::Definition> definitions{
        {"香港地區名；係香港嘅政治商業中心，大部份政府機構、銀行總行、跨國金融"
         "機構、外國領事館嘅所在地",
         "名詞",
         {}},
    };
    DefinitionsSet set{"粵典—words.hk", definitions};
    QCOMPARE(set.isEmpty(), false);
    QCOMPARE(set.getDefinitions().size(), 1);

    set.pushDefinition({"Central (a place in Hong Kong)", "name", {}});
    QCOMPARE(set.getDefinitions().size(), 2);
}

void TestDefinitionsSet::getSources()
{
    std::string sourceName = "粵典—words.hk";
    std::string sourceShortName = "WHK";

    QCOMPARE(DictionarySourceUtils::addSource(sourceName, sourceShortName),
             true);

    std::vector<Definition::Definition> definitions{
        {"香港地區名；係香港嘅政治商業中心，大部份政府機構、銀行總行、跨國金融"
         "機構、外國領事館嘅所在地",
         "名詞",
         {}},
    };
    DefinitionsSet set{"粵典—words.hk", definitions};
    QCOMPARE(QString::fromStdString(set.getSource()),
             QString::fromStdString(sourceName));
    QCOMPARE(QString::fromStdString(set.getSourceLongString()),
             QString::fromStdString(sourceName));
    QCOMPARE(QString::fromStdString(set.getSourceShortString()),
             QString::fromStdString(sourceShortName));

    QCOMPARE(DictionarySourceUtils::removeSource(sourceName), true);
}

void TestDefinitionsSet::getDefinitions()
{
    std::vector<Definition::Definition> definitions{
        {"Government of the Hong Kong Special Administrative Region",
         "name",
         {}},
        {"香港地區名；係香港嘅政治商業中心，大部份政府機構、銀行總行、跨國金融"
         "機構、外國領事館嘅所在地\nCentral, a central business district of "
         "Hong Kong",
         "名詞",
         {}},
    };
    DefinitionsSet set{"粵典—words.hk", definitions};
    QCOMPARE(set.isEmpty(), false);
    QCOMPARE(std::vector<Definition::Definition>(set.getDefinitions().begin(),
                                                 set.getDefinitions().end()),
             definitions);
    QCOMPARE(set.getDefinitions().size(), definitions.size());
    QCOMPARE(
        set.getDefinitionsSnippet(),
        "Government of the Hong Kong Special Administrative Region; "
        "香港地區名；係香港嘅政治商業中心，大部份政府機構、銀行總行、跨國金融"
        "機構、外國領事館嘅所在地");
}

QTEST_APPLESS_MAIN(TestDefinitionsSet)

#include "tst_definitionsset.moc"
