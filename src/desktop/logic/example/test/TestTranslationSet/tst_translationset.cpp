#include <QtTest>

#include "logic/source/sourceutils.h"
#include "logic/example/translationset.h"

class TestTranslationSet : public QObject
{
    Q_OBJECT

public:
    TestTranslationSet();
    ~TestTranslationSet();

private slots:
    void isEmpty();
    void isEmptyFalse();

    void getTranslationSnippetSimple();
    void getTranslationSnippetEmpty();

    void getTranslations();

    void getSources();
};

TestTranslationSet::TestTranslationSet() {}

TestTranslationSet::~TestTranslationSet() {}

void TestTranslationSet::isEmpty()
{
    TranslationSet set{"CC-CEDICT"};
    QCOMPARE(set.isEmpty(), true);
}

void TestTranslationSet::isEmptyFalse()
{
    Translation::Translation translation{"egg waffle", "eng", true};
    TranslationSet set = {"粵典—words.hk", {translation}};
    QCOMPARE(set.isEmpty(), false);
}

void TestTranslationSet::getTranslationSnippetSimple()
{
    Translation::Translation translation{"egg waffle", "eng", true};
    TranslationSet set = {"粵典—words.hk", {translation}};
    QCOMPARE(std::vector<Translation::Translation>(
                 set.getTranslationSnippet().begin(),
                 set.getTranslationSnippet().end()),
             {translation});
}

void TestTranslationSet::getTranslationSnippetEmpty()
{
    TranslationSet set = {"粵典—words.hk", {}};
    QCOMPARE(std::vector<Translation::Translation>(
                 set.getTranslationSnippet().begin(),
                 set.getTranslationSnippet().end()),
             {});
}

void TestTranslationSet::getTranslations()
{
    std::vector<Translation::Translation> translations{
        {"I don't even know where I flung the key at.", "eng", true},
        {"shake your hands and shake your legs", "eng", true},
        {"While playing on the roundabout at the playground when "
         "I was small, I was thrown out, and I hurt my chin so "
         "badly I had to get stitched up at the hospital.",
         "eng",
         true},
        {"to shake something off", "eng", true},
        {"Can you stop shaking your wiener about?", "eng", true},
    };
    TranslationSet set{"粵典—words.hk", translations};
    QCOMPARE(set.isEmpty(), false);
    QCOMPARE(std::vector<Translation::Translation>(set.getTranslations().begin(),
                                                   set.getTranslations().end()),
             translations);
    QCOMPARE(set.getTranslations().size(), translations.size());
    QCOMPARE(
        std::vector<Translation::Translation>(set.getTranslationSnippet().begin(),
                                              set.getTranslationSnippet().end()),
        translations);
    QCOMPARE(set.getTranslationSnippet().size(), translations.size());
}

void TestTranslationSet::getSources()
{
    std::string sourceName = "粵典—words.hk";
    std::string sourceShortName = "WHK";

    QCOMPARE(SourceUtils::addSource(sourceName, sourceShortName), true);

    std::vector<Translation::Translation> translations{
        {"I don't even know where I flung the key at.", "eng", true},
    };
    TranslationSet set{"粵典—words.hk", translations};
    QCOMPARE(QString::fromStdString(set.getSource()),
             QString::fromStdString(sourceName));
    QCOMPARE(QString::fromStdString(set.getSourceLongString()),
             QString::fromStdString(sourceName));
    QCOMPARE(QString::fromStdString(set.getSourceShortString()),
             QString::fromStdString(sourceShortName));

    QCOMPARE(SourceUtils::removeSource(sourceName), true);
}

QTEST_APPLESS_MAIN(TestTranslationSet)

#include "tst_translationset.moc"
