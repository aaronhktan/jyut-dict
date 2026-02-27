#include <QtTest>

#include "logic/dictionary/dictionarysource.h"
#include "logic/sentence/sentenceset.h"

class TestSentenceSet : public QObject
{
    Q_OBJECT

public:
    TestSentenceSet();
    ~TestSentenceSet();

private slots:
    void isEmpty();
    void addSentences();

    void getSources();
    void getSentences();
};

TestSentenceSet::TestSentenceSet() {}

TestSentenceSet::~TestSentenceSet() {}

void TestSentenceSet::isEmpty()
{
    SentenceSet set{"CC-CEDICT"};
    QCOMPARE(set.isEmpty(), true);
}

void TestSentenceSet::addSentences()
{
    std::vector<Sentence::TargetSentence> targetSentences{
        {"I don't even know where I flung the key at.", "eng", true},
    };
    SentenceSet set{"粵典—words.hk", targetSentences};
    QCOMPARE(set.isEmpty(), false);
    QCOMPARE(set.getSentences().size(), 1);

    set.pushSentence({"shake your hands and shake your legs", "eng", true});
    QCOMPARE(set.getSentences().size(), 2);
}

void TestSentenceSet::getSources()
{
    std::string sourceName = "粵典—words.hk";
    std::string sourceShortName = "WHK";

    QCOMPARE(DictionarySourceUtils::addSource(sourceName, sourceShortName),
             true);

    std::vector<Sentence::TargetSentence> targetSentences{
        {"I don't even know where I flung the key at.", "eng", true},
    };
    SentenceSet set{"粵典—words.hk", targetSentences};
    QCOMPARE(QString::fromStdString(set.getSource()),
             QString::fromStdString(sourceName));
    QCOMPARE(QString::fromStdString(set.getSourceLongString()),
             QString::fromStdString(sourceName));
    QCOMPARE(QString::fromStdString(set.getSourceShortString()),
             QString::fromStdString(sourceShortName));

    QCOMPARE(DictionarySourceUtils::removeSource(sourceName), true);
}

void TestSentenceSet::getSentences()
{
    std::vector<Sentence::TargetSentence> targetSentences{
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
    SentenceSet set{"粵典—words.hk", targetSentences};
    QCOMPARE(set.isEmpty(), false);
    QCOMPARE(std::vector<Sentence::TargetSentence>(set.getSentences().begin(),
                                                   set.getSentences().end()),
             targetSentences);
    QCOMPARE(set.getSentences().size(), targetSentences.size());
    QCOMPARE(
        std::vector<Sentence::TargetSentence>(set.getSentenceSnippet().begin(),
                                              set.getSentenceSnippet().end()),
        targetSentences);
    QCOMPARE(set.getSentenceSnippet().size(), targetSentences.size());

    std::vector<Sentence::TargetSentence> additionalTargetSentences = {
        {"Someone upstairs plays loud music every night. The "
         "volume is so loud that I can't watch the TV.",
         "eng",
         true},
        {"There is a spider on your hand! Shake it off!!", "eng", true},
    };
    for (const auto &i : additionalTargetSentences) {
        set.pushSentence(i);
    }
    auto allSentences = targetSentences;
    allSentences.insert(allSentences.end(),
                        additionalTargetSentences.begin(),
                        additionalTargetSentences.end());
    QCOMPARE(std::vector<Sentence::TargetSentence>(set.getSentences().begin(),
                                                   set.getSentences().end()),
             allSentences);
    QCOMPARE(set.getSentences().size(), allSentences.size());
    QCOMPARE(
        std::vector<Sentence::TargetSentence>(set.getSentenceSnippet().begin(),
                                              set.getSentenceSnippet().end()),
        targetSentences);
    QCOMPARE(set.getSentenceSnippet().size(), targetSentences.size());
}

QTEST_APPLESS_MAIN(TestSentenceSet)

#include "tst_sentenceset.moc"
