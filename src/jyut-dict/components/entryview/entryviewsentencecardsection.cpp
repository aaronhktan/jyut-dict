#include "entryviewsentencecardsection.h"

#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

EntryViewSentenceCardSection::EntryViewSentenceCardSection(std::shared_ptr<SQLDatabaseManager> manager,
                                         QWidget *parent)
    : QWidget(parent),
    _manager{manager}
{
    _enableUIUpdateTimer = new QTimer{this};
    _updateUITimer = new QTimer{this};

    _search = std::make_unique<SQLSearch>(_manager);
    _search->registerObserver(this);

    setupUI();

    // We need to do this because the callback is called from a different thread.
    // In order for the vector of SourceSentences to be copied to the UI thread,
    // Q_DECLARE_METATYPE and qRegisterMetaType must be called.
    qRegisterMetaType<std::vector<SourceSentence>>();
    qRegisterMetaType<sentenceSamples>();
    QObject::connect(this,
                     &EntryViewSentenceCardSection::callbackInvoked,
                     this,
                     &EntryViewSentenceCardSection::pauseBeforeUpdatingUI);
}

EntryViewSentenceCardSection::EntryViewSentenceCardSection(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
}

EntryViewSentenceCardSection::~EntryViewSentenceCardSection()
{
}

void EntryViewSentenceCardSection::callback(
    std::vector<SourceSentence> sourceSentences, bool emptyQuery)
{
    (void) (emptyQuery);
    std::lock_guard<std::mutex> update{updateMutex};
    sentenceSamples samples = getSamplesForEachSource(sourceSentences);
    emit callbackInvoked(sourceSentences, samples);
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
    _viewAllSentencesButton->setVisible(false);
    _viewAllSentencesButton->setToolButtonStyle(Qt::ToolButtonTextOnly);

    _sentenceCardsLayout->addWidget(_loadingWidget);
    _sentenceCardsLayout->setAlignment(_loadingWidget, Qt::AlignHCenter);
    _sentenceCardsLayout->addWidget(_viewAllSentencesButton);
    _sentenceCardsLayout->setAlignment(_viewAllSentencesButton, Qt::AlignRight);

    _showLoadingIconTimer = new QTimer{this};

    setStyle(Utils::isDarkMode());
    translateUI();
}

void EntryViewSentenceCardSection::translateUI()
{
    _viewAllSentencesButton->setText(tr("View all sentences →"));
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

void EntryViewSentenceCardSection::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void EntryViewSentenceCardSection::setStyle(bool use_dark)
{
    QColor textColour = use_dark ? QColor{LABEL_TEXT_COLOUR_DARK_R,
                                          LABEL_TEXT_COLOUR_DARK_G,
                                          LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{LABEL_TEXT_COLOUR_LIGHT_R,
                                          LABEL_TEXT_COLOUR_LIGHT_G,
                                          LABEL_TEXT_COLOUR_LIGHT_B};
    int borderRadius = 17;
    QString radiusString = QString::number(borderRadius);
    QColor borderColour = use_dark ? textColour.darker(300)
                                   : textColour.lighter(200);
    QString styleSheet = "QToolButton { border: 2px solid %1; "
                         "border-radius: %2px; "
                         "color: %3; "
                         "font-size: 12px; "
                         "padding: 6px; } ";
    _viewAllSentencesButton->setStyleSheet(styleSheet.arg(borderColour.name(), radiusString, textColour.name()));
    _viewAllSentencesButton->setMinimumHeight(borderRadius * 2);
}

void EntryViewSentenceCardSection::setEntry(const Entry &entry)
{
    {
        std::lock_guard<std::mutex> layout{layoutMutex};
        cleanup();
    }

    // Show loading widget only if search results are not found within
    // a certain deadline
    _calledBack = false;
    _showLoadingIconTimer->stop();
    _showLoadingIconTimer->setInterval(1000);
    _showLoadingIconTimer->setSingleShot(true);
    QObject::connect(_showLoadingIconTimer, &QTimer::timeout, this, [=]() {
        if (!_calledBack && _enableUIUpdate) {
            showLoadingWidget();
        }
    });
    _showLoadingIconTimer->start();

    // Actually start searching for sentences
    _search->searchTraditionalSentences(entry.getTraditional().c_str());
    _title = entry
                 .getCharactersNoSecondary(
                     Settings::getSettings()
                         ->value("characterOptions",
                                 QVariant::fromValue(
                                     EntryCharactersOptions::PREFER_TRADITIONAL))
                         .value<EntryCharactersOptions>(),
                     false)
                 .c_str();
    _title = _title.trimmed();
}

void EntryViewSentenceCardSection::updateUI(
    std::vector<SourceSentence> sourceSentences, sentenceSamples samples)
{
    std::lock_guard<std::mutex> layout{layoutMutex};
    cleanup();

    _calledBack = true;
    _loadingWidget->setVisible(false);

    _sentences = sourceSentences;

    // This prevents an extra space from being added at the bottom when there
    // is nothing to display in the sentence card section.
    if (samples.empty()) {
        _sentenceCardsLayout->setContentsMargins(0, 0, 0, 0);
        return;
    } else {
        _sentenceCardsLayout->setContentsMargins(0, 11, 0, 0);
    }

    emit addingCards();
    for (const auto &item : samples) {
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

void EntryViewSentenceCardSection::stallSentenceUIUpdate(void)
{
    _enableUIUpdate = false;
    _enableUIUpdateTimer->stop();
    disconnect(_enableUIUpdateTimer, nullptr, nullptr, nullptr);
    _enableUIUpdateTimer->setInterval(250);
    _enableUIUpdateTimer->setSingleShot(true);
    QObject::connect(_enableUIUpdateTimer, &QTimer::timeout, this, [=]() {
        _enableUIUpdate = true;
    });
    _enableUIUpdateTimer->start();
}

void EntryViewSentenceCardSection::pauseBeforeUpdatingUI(std::vector<SourceSentence> sourceSentences,
                                                         sentenceSamples samples)
{
    _updateUITimer->stop();
    disconnect(_updateUITimer, nullptr, nullptr, nullptr);

    _updateUITimer->setInterval(25);
    QObject::connect(_updateUITimer, &QTimer::timeout, this, [=]() {
        if (_enableUIUpdate) {
            _updateUITimer->stop();
            disconnect(_updateUITimer, nullptr, nullptr, nullptr);
            updateUI(sourceSentences, samples);
        }
    });
    _updateUITimer->start();
}

void EntryViewSentenceCardSection::showLoadingWidget(void)
{
    std::lock_guard<std::mutex> layout{layoutMutex};
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
    splitter->setSearchTerm(_title);
    splitter->show();
}

// Given some sourceSentences, returns a set of five (or fewer) sentences from
// each source that exists in the source sentence.
std::unordered_map<std::string, std::vector<SourceSentence>>
EntryViewSentenceCardSection::getSamplesForEachSource(
    const std::vector<SourceSentence> &sourceSentences)
{
    std::unordered_map<std::string, std::vector<SourceSentence>> samples;

    for (auto sourceSentence : sourceSentences) {
        for (auto sentenceSet : sourceSentence.getSentenceSets()) {
            std::string source = sentenceSet.getSource();
            if (samples[source].size() >= 2) {
                continue;
            }

            SourceSentence sentence
                = SourceSentence(sourceSentence.getSourceLanguage(),
                                 sourceSentence.getSimplified(),
                                 sourceSentence.getTraditional(),
                                 sourceSentence.getJyutping(),
                                 sourceSentence.getPinyin(),
                                 std::vector<SentenceSet>{sentenceSet});

            samples[source].push_back(sentence);
        }
    }

    return samples;
}
