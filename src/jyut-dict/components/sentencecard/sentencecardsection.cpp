#include "sentencecardsection.h"

#include <QTimer>

SentenceCardSection::SentenceCardSection(std::shared_ptr<SQLDatabaseManager> manager,
                                         QWidget *parent)
    : QWidget(parent),
    _manager{manager}
{
    _search = std::make_unique<SQLSearch>(_manager);
    _search->registerObserver(this);

    setupUI();

    // We need to do this because the callback is in a different thread.
    // Otherwise, the UI will not appear.
    qRegisterMetaType<std::vector<SourceSentence>>();
    QObject::connect(this,
                     &SentenceCardSection::callbackInvoked,
                     this,
                     &SentenceCardSection::updateUI);
}

void SentenceCardSection::callback(std::vector<Entry> __unused entries,
                                   bool __unused emptyQuery)
{
    void();
}

void SentenceCardSection::callback(std::vector<SourceSentence> sourceSentences,
                                   bool __unused emptyQuery)
{
    emit callbackInvoked(sourceSentences);
}

void SentenceCardSection::updateUI(std::vector<SourceSentence> sourceSentences)
{
    cleanup();
    _calledBack = true;
    std::unordered_map<std::string, std::vector<SourceSentence>> sources;
    sources = getSamplesForEachSource(sourceSentences);

    for (const auto &item : sources) {
        _sentenceCards.push_back(new SentenceCardWidget{this});
        _sentenceCards.back()->displaySentences(item.second);

        _sentenceCardsLayout->addWidget(_sentenceCards.back(), Qt::AlignHCenter);
    }
    _loadingWidget->setVisible(false);
}

void SentenceCardSection::setEntry(const Entry &entry)
{
    cleanup();
    QTimer::singleShot(500, [=]() {
        if (!_calledBack) {
            showLoadingWidget();
        }
    });
    QString searchTerm = entry.getTraditional().c_str();
    _search->searchTraditionalSentences(searchTerm);
}

void SentenceCardSection::setupUI(void)
{
    _sentenceCardsLayout = new QVBoxLayout{this};
    _sentenceCardsLayout->setContentsMargins(0, 0, 0, 0);
    _sentenceCardsLayout->setSpacing(15);

    _loadingWidget = new LoadingWidget{this};
    _loadingWidget->setVisible(false);
    _sentenceCardsLayout->addWidget(_loadingWidget);
    _sentenceCardsLayout->setAlignment(_loadingWidget, Qt::AlignHCenter);
}

void SentenceCardSection::cleanup(void)
{
    for (auto card : _sentenceCards) {
        _sentenceCardsLayout->removeWidget(card);
        delete card;
    }
    _sentenceCards.clear();
}

void SentenceCardSection::showLoadingWidget(void)
{
    _loadingWidget->setVisible(true);
}

// Given some sourceSentences, returns a set of five (or fewer) sentences from
// each source that exists in the source sentence.
std::unordered_map<std::string, std::vector<SourceSentence>>
SentenceCardSection::getSamplesForEachSource(
    const std::vector<SourceSentence> &sourceSentences)
{
    std::unordered_map<std::string, std::vector<SourceSentence>> sources;

    for (auto sourceSentence : sourceSentences) {
        for (auto sentenceSet : sourceSentence.getSentenceSets()) {
            std::string source = sentenceSet.getSource();
            if (sources[source].size() >= 5) {
                continue;
            }

            SourceSentence sentence
                = SourceSentence(sourceSentence.getSimplified(),
                                 sourceSentence.getTraditional(),
                                 sourceSentence.getJyutping(),
                                 sourceSentence.getPinyin(),
                                 std::vector<SentenceSet>{sentenceSet});

            sources[source].push_back(sentence);
        }
    }

    return sources;
}
