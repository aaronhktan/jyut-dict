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
    void addTranslations();

    void getSources();
    void getTranslations();
};

TestTranslationSet::TestTranslationSet() {}

TestTranslationSet::~TestTranslationSet() {}

void TestTranslationSet::isEmpty()
{
    TranslationSet set{"CC-CEDICT"};
    QCOMPARE(set.isEmpty(), true);
}

void TestTranslationSet::addTranslations()
{
    std::vector<Translation::Translation> translations{
        {"I don't even know where I flung the key at.", "eng", true},
    };
    TranslationSet set{"粵典—words.hk", translations};
    QCOMPARE(set.isEmpty(), false);
    QCOMPARE(set.getTranslations().size(), 1);

    set.pushTranslation({"shake your hands and shake your legs", "eng", true});
    QCOMPARE(set.getTranslations().size(), 2);
}

void TestTranslationSet::getSources()
{
    std::string sourceName = "粵典—words.hk";
    std::string sourceShortName = "WHK";

    QCOMPARE(SourceUtils::addSource(sourceName, sourceShortName),
             true);

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

    std::vector<Translation::Translation> additionalTranslations = {
        {"Someone upstairs plays loud music every night. The "
         "volume is so loud that I can't watch the TV.",
         "eng",
         true},
        {"There is a spider on your hand! Shake it off!!", "eng", true},
    };
    for (const auto &i : additionalTranslations) {
        set.pushTranslation(i);
    }
    auto allTranslations = translations;
    allTranslations.insert(allTranslations.end(),
                           additionalTranslations.begin(),
                           additionalTranslations.end());
    QCOMPARE(std::vector<Translation::Translation>(set.getTranslations().begin(),
                                                   set.getTranslations().end()),
             allTranslations);
    QCOMPARE(set.getTranslations().size(), allTranslations.size());
    QCOMPARE(std::vector<Translation::Translation>(
                 set.getTranslationSnippet().begin(),
                 set.getTranslationSnippet().end()),
             translations);
    QCOMPARE(set.getTranslationSnippet().size(), translations.size());
}

QTEST_APPLESS_MAIN(TestTranslationSet)

#include "tst_translationset.moc"
