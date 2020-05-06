#include "entryviewsentencecardsection.h"

#include "logic/settings/settingsutils.h"

EntryViewSentenceCardSection::EntryViewSentenceCardSection(std::shared_ptr<SQLDatabaseManager> manager,
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
                     &EntryViewSentenceCardSection::callbackInvoked,
                     this,
                     &EntryViewSentenceCardSection::updateUI);
}

EntryViewSentenceCardSection::EntryViewSentenceCardSection(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
}

EntryViewSentenceCardSection::~EntryViewSentenceCardSection()
{
}


void EntryViewSentenceCardSection::callback(std::vector<Entry> __unused entries,
                                   bool __unused emptyQuery)
{
    void();
}

void EntryViewSentenceCardSection::callback(
    std::vector<SourceSentence> sourceSentences, bool __unused emptyQuery)
{
    emit callbackInvoked(sourceSentences);
}

void EntryViewSentenceCardSection::updateUI(std::vector<SourceSentence> sourceSentences)
{
    cleanup();

    _calledBack = true;
    _loadingWidget->setVisible(false);

    _sentences = sourceSentences;

    std::unordered_map<std::string, std::vector<SourceSentence>> sources;
    sources = getSamplesForEachSource(sourceSentences);

    // This prevents an extra space from being added at the bottom when there
    // is nothing to display in the sentence card section.
    if (sources.empty()) {
        _sentenceCardsLayout->setContentsMargins(0, 0, 0, 0);
        return;
    } else {
        _sentenceCardsLayout->setContentsMargins(0, 11, 0, 0);
    }

    emit addingCards();
    for (const auto &item : sources) {
        _sentenceCards.push_back(new SentenceCardWidget{this});
        _sentenceCards.back()->displaySentences(item.second);

        _sentenceCardsLayout->addWidget(_sentenceCards.back(), Qt::AlignHCenter);
    }

    _sentenceCardsLayout->addWidget(_viewAllSentencesButton);
    _sentenceCardsLayout->setAlignment(_viewAllSentencesButton, Qt::AlignRight);
    _viewAllSentencesButton->setVisible(true);

    disconnect(_viewAllSentencesButton, nullptr, nullptr, nullptr);
    connect(_viewAllSentencesButton, &QToolButton::clicked, this, [&]() {
        openSentenceWindow(_sentences);
    });
    emit finishedAddingCards();
}

void EntryViewSentenceCardSection::setEntry(const Entry &entry)
{
    cleanup();
    _calledBack = false;
    _timer->stop();
    _timer->setInterval(1000);
    _timer->setSingleShot(true);
    QObject::connect(_timer, &QTimer::timeout, this, [=]() {
        if (!_calledBack) {
            showLoadingWidget();
        }
    });
    _timer->start();
    _search->searchTraditionalSentences(entry.getTraditional().c_str());
    _title = entry
                 .getCharacters(
                     Settings::getSettings()
                         ->value("characterOptions",
                                 QVariant::fromValue(
                                     EntryCharactersOptions::PREFER_TRADITIONAL))
                         .value<EntryCharactersOptions>(),
                     false)
                 .c_str();
    _title = _title.trimmed();
}

void EntryViewSentenceCardSection::setupUI(void)
{
    _sentenceCardsLayout = new QVBoxLayout{this};
    _sentenceCardsLayout->setContentsMargins(0, 0, 0, 0);
    _sentenceCardsLayout->setSpacing(11);

    _loadingWidget = new LoadingWidget{this};
    _loadingWidget->setVisible(false);

    // We can't use a QPushButton on macOS because it breaks the margin
    // See https://stackoverflow.com/questions/12327609/qpushbutton-changes-margins-on-other-widgets-in-the-same-layout
    // for more details.
    _viewAllSentencesButton = new QToolButton{this};
    _viewAllSentencesButton->setText("View all sentences →");
    _viewAllSentencesButton->setVisible(false);
    _viewAllSentencesButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    _viewAllSentencesButton->setStyleSheet(
        "QToolButton { border: 2px solid #323232; border-radius: 17px; font-size: 12px; padding: 6px; } ");

    _sentenceCardsLayout->addWidget(_loadingWidget);
    _sentenceCardsLayout->setAlignment(_loadingWidget, Qt::AlignHCenter);
    _sentenceCardsLayout->addWidget(_viewAllSentencesButton);
    _sentenceCardsLayout->setAlignment(_viewAllSentencesButton, Qt::AlignRight);

    _timer = new QTimer{this};
}

void EntryViewSentenceCardSection::cleanup(void)
{
    for (auto card : _sentenceCards) {
        _sentenceCardsLayout->removeWidget(card);
        delete card;
    }
    _sentenceCards.clear();
    _sentenceCardsLayout->removeWidget(_viewAllSentencesButton);
}

void EntryViewSentenceCardSection::showLoadingWidget(void)
{
    _loadingWidget->setVisible(true);
    _sentenceCardsLayout->addWidget(_loadingWidget);
    _sentenceCardsLayout->setAlignment(_loadingWidget, Qt::AlignHCenter);
}

void EntryViewSentenceCardSection::openSentenceWindow(
    std::vector<SourceSentence> sourceSentences)
{
    SentenceSplitter *splitter = new SentenceSplitter{_manager, nullptr};
    splitter->setParent(this, Qt::Window);
    splitter->setSourceSentences(sourceSentences);
    splitter->show();
    QString title = QString{"Sentences for %1 (%2 results)"}
                        .arg(_title, QString::number(sourceSentences.size()));
    splitter->setWindowTitle(title);
}

// Given some sourceSentences, returns a set of five (or fewer) sentences from
// each source that exists in the source sentence.
std::unordered_map<std::string, std::vector<SourceSentence>>
EntryViewSentenceCardSection::getSamplesForEachSource(
    const std::vector<SourceSentence> &sourceSentences)
{
    std::unordered_map<std::string, std::vector<SourceSentence>> sources;

    for (auto sourceSentence : sourceSentences) {
        for (auto sentenceSet : sourceSentence.getSentenceSets()) {
            std::string source = sentenceSet.getSource();
            if (sources[source].size() >= 2) {
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
