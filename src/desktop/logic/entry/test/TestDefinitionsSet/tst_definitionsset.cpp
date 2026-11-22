#include <QtTest>

#include "logic/entry/definitionsset.h"

#include "logic/source/sourceutils.h"

class TestDefinitionsSet : public QObject
{
    Q_OBJECT

public:
    TestDefinitionsSet();
    ~TestDefinitionsSet();

private slots:
    void isEmpty();
    void cloneSimple();
    void equalsSimple();
    void getDefinitionsSnippetSimple();
    void getDefinitionsSnippetEmpty();
    void definitionsCopy();

    void simple();
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

void TestDefinitionsSet::cloneSimple()
{
    Definition::Definition
        definition{"Qingyuan, prefecture-level city in Guangdong", "", {}};
    DefinitionsSet definitionsSet{"CC-CEDICT", {definition}};

    DefinitionsSet secondDefinitionsSet = definitionsSet;
    QCOMPARE(definitionsSet, secondDefinitionsSet);
    QCOMPARE(&definitionsSet == &secondDefinitionsSet, false);

    definition.definitionContent
        = "(historical) Qingyuan (a former county of Guangdong, China)";
    QCOMPARE(definitionsSet, secondDefinitionsSet);
    QCOMPARE(definitionsSet.getDefinitions()[0].definitionContent,
             "Qingyuan, prefecture-level city in Guangdong");
    QCOMPARE(secondDefinitionsSet.getDefinitions()[0].definitionContent,
             "Qingyuan, prefecture-level city in Guangdong");
}

void TestDefinitionsSet::equalsSimple()
{
    Definition::Definition
        definition{"Qingyuan, prefecture-level city in Guangdong", "", {}};
    DefinitionsSet definitionsSet{"CC-CEDICT", {definition}};

    Definition::Definition
        secondDefinition{"Qingyuan, prefecture-level city in Guangdong", "", {}};
    DefinitionsSet secondDefinitionsSet{"CC-CEDICT", {secondDefinition}};

    QCOMPARE(definitionsSet, secondDefinitionsSet);
    QCOMPARE(&definitionsSet == &secondDefinitionsSet, false);
}

void TestDefinitionsSet::getDefinitionsSnippetSimple()
{
    Definition::Definition
        definition{"Qingyuan, prefecture-level city in Guangdong", "", {}};
    DefinitionsSet definitionsSet{"CC-CEDICT", {definition}};

    QCOMPARE(definitionsSet.getDefinitionsSnippet(),
             "Qingyuan, prefecture-level city in Guangdong");
}

void TestDefinitionsSet::getDefinitionsSnippetEmpty()
{
    DefinitionsSet definitionsSet{"CC-CEDICT", {}};
    QCOMPARE(definitionsSet.getDefinitionsSnippet(), "");
}

void TestDefinitionsSet::definitionsCopy()
{
    Definition::Definition
        definition{"Qingyuan, prefecture-level city in Guangdong", "", {}};
    std::vector<Definition::Definition> definitionsList{definition};
    DefinitionsSet definitionsSet{"CC-CEDICT", definitionsList};

    std::span<const Definition::Definition> s = definitionsSet.getDefinitions();
    std::vector<Definition::Definition> v;
    v.assign(s.begin(), s.end());
    QCOMPARE(v, definitionsList);
}

void TestDefinitionsSet::simple()
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
}

void TestDefinitionsSet::getSources()
{
    std::string sourceName = "粵典—words.hk";
    std::string sourceShortName = "WHK";

    QCOMPARE(SourceUtils::addSource(sourceName, sourceShortName),
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

    QCOMPARE(SourceUtils::removeSource(sourceName), true);
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
