#include "entryviewexamplecardsection.h"

#include "components/examplecard/examplecardwidget.h"
#include "components/examplecard/loadingwidget.h"
#include "components/examplewindow/examplesplitter.h"
#include "logic/database/sqldatabasemanager.h"
#include "logic/search/sqlsearch.h"
#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QEvent>
#include <QString>
#include <QTimer>
#include <QToolButton>
#include <QVBoxLayout>

EntryViewExampleCardSection::EntryViewExampleCardSection(
    std::shared_ptr<SQLDatabaseManager> manager, QWidget *parent)
    : QWidget{parent}
    , _manager{manager}
    , _search{new SQLSearch(_manager)}
    , _settings{Settings::getSettings(this)}
    , _enableUIUpdateTimer{new QTimer{this}}
    , _updateUITimer{new QTimer{this}}
{
    _search->registerObserver(this);

    setupUI();

    // We need to do this because the callback is called from a different thread.
    // In order for the vector of Examples to be copied to the UI thread,
    // Q_DECLARE_METATYPE and qRegisterMetaType must be called.
    qRegisterMetaType<std::vector<Example>>();
    qRegisterMetaType<exampleSamples>();
    QObject::connect(this,
                     &EntryViewExampleCardSection::callbackInvoked,
                     this,
                     &EntryViewExampleCardSection::pauseBeforeUpdatingUI);
}

EntryViewExampleCardSection::EntryViewExampleCardSection(QWidget *parent)
    : QWidget{parent}
{
    setupUI();
}

void EntryViewExampleCardSection::callback(const std::vector<Example> &examples,
                                           [[maybe_unused]] bool emptyQuery)
{
    std::lock_guard<std::mutex> update{updateMutex};
    exampleSamples samples = getSamplesForEachSource(examples);
    emit callbackInvoked(examples, samples);
}

void EntryViewExampleCardSection::setupUI(void)
{
    _exampleCardsLayout = new QVBoxLayout{this};
    _exampleCardsLayout->setContentsMargins(0, 0, 0, 0);
    _exampleCardsLayout->setSpacing(11);

    _loadingWidget = new LoadingWidget{this};
    _loadingWidget->setVisible(false);

    // We can't use a QPushButton on macOS because it breaks the margin
    // See https://stackoverflow.com/questions/12327609/qpushbutton-changes-margins-on-other-widgets-in-the-same-layout
    // for more details.
    _viewAllExamplesButton = new QToolButton{this};
    _viewAllExamplesButton->setVisible(false);
    _viewAllExamplesButton->setToolButtonStyle(Qt::ToolButtonTextOnly);

    _exampleCardsLayout->addWidget(_loadingWidget);
    _exampleCardsLayout->setAlignment(_loadingWidget, Qt::AlignHCenter);
    _exampleCardsLayout->addWidget(_viewAllExamplesButton);
    _exampleCardsLayout->setAlignment(_viewAllExamplesButton, Qt::AlignRight);

    _showLoadingIconTimer = new QTimer{this};

    setStyle(Utils::isDarkMode());
    translateUI();
}

void EntryViewExampleCardSection::translateUI()
{
    _viewAllExamplesButton->setText(tr("View all examples →"));
}

void EntryViewExampleCardSection::cleanup(void)
{
    for (const auto &card : _exampleCards) {
        _exampleCardsLayout->removeWidget(card);
        delete card;
    }
    _exampleCards.clear();
    _exampleCardsLayout->removeWidget(_viewAllExamplesButton);
    _exampleCardsLayout->setContentsMargins(0, 0, 0, 0);
}

void EntryViewExampleCardSection::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [this] {
            _paletteRecentlyChanged = false;
        });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QWidget::changeEvent(event);
}

void EntryViewExampleCardSection::setStyle(bool use_dark)
{
    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());
    int bodyFontSize = Settings::bodyFontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));

    QColor textColour = use_dark ? QColor{Utils::LABEL_TEXT_COLOUR_DARK_R,
                                          Utils::LABEL_TEXT_COLOUR_DARK_G,
                                          Utils::LABEL_TEXT_COLOUR_DARK_B}
                                 : QColor{Utils::LABEL_TEXT_COLOUR_LIGHT_R,
                                          Utils::LABEL_TEXT_COLOUR_LIGHT_G,
                                          Utils::LABEL_TEXT_COLOUR_LIGHT_B};
    int borderRadius = static_cast<int>(bodyFontSize * 1.5);
    QString radiusString = QString::number(borderRadius);
    QColor borderColour = use_dark
                              ? QColor{Utils::CONTENT_BACKGROUND_COLOUR_DARK_R,
                                       Utils::CONTENT_BACKGROUND_COLOUR_DARK_G,
                                       Utils::CONTENT_BACKGROUND_COLOUR_DARK_B}
                              : QColor{Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_R,
                                       Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_G,
                                       Utils::CONTENT_BACKGROUND_COLOUR_LIGHT_B};
    QString styleSheet = "QToolButton { "
#ifdef Q_OS_WIN
                         "   border: 1px solid %1; "
#else
                         "   border: 2px solid %1; "
#endif
                         "   border-radius: %2px; "
                         "   color: %3; "
                         "   font-size: %4px; "
                         "   padding: %5px; "
                         "} "
                         ""
                         "QToolButton:hover { "
                         "   background-color: %1; "
#ifdef Q_OS_WIN
                         "   border: 1px solid %1; "
#else
                         "   border: 2px solid %1; "
#endif
                         "   border-radius: %2px; "
                         "   color: %3; "
                         "   font-size: %4px; "
                         "   padding: %5px; "
                         "} ";
    _viewAllExamplesButton->setStyleSheet(
        styleSheet.arg(borderColour.name(), radiusString, textColour.name())
            .arg(bodyFontSize)
            .arg(bodyFontSize / 4));
    _viewAllExamplesButton->setMinimumHeight(borderRadius * 2);
}

void EntryViewExampleCardSection::setEntry(const Entry &entry)
{
    {
        std::lock_guard<std::mutex> layout{layoutMutex};
        cleanup();
    }

    // Show loading widget only if search results are not found within
    // a certain deadline
    _calledBack = false;
    _showLoadingIconTimer->stop();
    _showLoadingIconTimer->setInterval(1500);
    _showLoadingIconTimer->setSingleShot(true);
    QObject::connect(_showLoadingIconTimer, &QTimer::timeout, this, [this] {
        if (!_calledBack && _enableUIUpdate) {
            showLoadingWidget();
        }
    });
    _showLoadingIconTimer->start();

    // Actually start searching for examples
    _search->searchTraditionalExamples(
        QString::fromStdString(entry.getTraditional())
            .replace("%", "\\%")
            .replace("_", "\\_"));
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

void EntryViewExampleCardSection::updateUI(const std::vector<Example> &examples,
                                           const exampleSamples &samples)
{
    std::lock_guard<std::mutex> layout{layoutMutex};
    cleanup();

    _calledBack = true;
    _loadingWidget->setVisible(false);

    _examples = examples;

    // This prevents an extra space from being added at the bottom when there
    // is nothing to display in the example card section.
    if (samples.empty()) {
        _exampleCardsLayout->setContentsMargins(0, 0, 0, 0);
        emit noCardsAdded();
        return;
    } else {
        _exampleCardsLayout->setContentsMargins(0, 11, 0, 0);
    }

    emit addingCards();
    for (const auto &item : samples) {
        _exampleCards.push_back(new ExampleCardWidget{this});
        _exampleCards.back()->displayExamples(item.second);

        _exampleCardsLayout->addWidget(_exampleCards.back(), Qt::AlignHCenter);
    }

    _exampleCardsLayout->addWidget(_viewAllExamplesButton);
    _exampleCardsLayout->setAlignment(_viewAllExamplesButton, Qt::AlignRight);
    _viewAllExamplesButton->setVisible(true);

    disconnect(_viewAllExamplesButton, nullptr, this, nullptr);
    connect(_viewAllExamplesButton, &QToolButton::clicked, this, [this] {
        openExampleWindow(_examples);
    });
    emit finishedAddingCards();
}

void EntryViewExampleCardSection::stallExampleUIUpdate(void)
{
    _enableUIUpdate = false;
    _enableUIUpdateTimer->stop();
    disconnect(_enableUIUpdateTimer, nullptr, this, nullptr);
#ifdef Q_OS_WIN
    _enableUIUpdateTimer->setInterval(800);
#else
    _enableUIUpdateTimer->setInterval(250);
#endif
    _enableUIUpdateTimer->setSingleShot(true);
    QObject::connect(_enableUIUpdateTimer, &QTimer::timeout, this, [this] {
        _enableUIUpdate = true;
    });
    _enableUIUpdateTimer->start();
}

void EntryViewExampleCardSection::updateStyleRequested(void)
{
    for (auto &card : _exampleCards) {
        card->updateStyleRequested();
    }

    QList<ExampleSplitter *> ExampleSplitters
        = this->findChildren<ExampleSplitter *>();
    foreach (auto &splitter, ExampleSplitters) {
        splitter->updateStyleRequested();
    }

    setStyle(Utils::isDarkMode());
}

void EntryViewExampleCardSection::viewAllExamplesRequested(void)
{
    if (!_examples.empty()) {
        _viewAllExamplesButton->click();
    }
}

void EntryViewExampleCardSection::pauseBeforeUpdatingUI(
    const std::vector<Example> &Examples, const exampleSamples &samples)
{
    _updateUITimer->stop();
    disconnect(_updateUITimer, nullptr, this, nullptr);

#ifdef Q_OS_WIN
    _updateUITimer->setInterval(400);
#else
    _updateUITimer->setInterval(25);
#endif
    QObject::connect(_updateUITimer,
                     &QTimer::timeout,
                     this,
                     [this, samples, Examples] {
                         if (_enableUIUpdate) {
                             _updateUITimer->stop();
                             disconnect(_updateUITimer, nullptr, this, nullptr);
                             updateUI(Examples, samples);
                         }
                     });
    _updateUITimer->start();
}

void EntryViewExampleCardSection::showLoadingWidget(void)
{
    std::lock_guard<std::mutex> layout{layoutMutex};
    _loadingWidget->setVisible(true);
    _exampleCardsLayout->addWidget(_loadingWidget);
    _exampleCardsLayout->setAlignment(_loadingWidget, Qt::AlignHCenter);
}

void EntryViewExampleCardSection::openExampleWindow(
    const std::vector<Example> &Examples)
{
    ExampleSplitter *splitter = new ExampleSplitter{_manager, nullptr};
    splitter->setParent(this, Qt::Window);
    splitter->setAttribute(Qt::WA_DeleteOnClose);
    splitter->setExamples(Examples);
    splitter->setSearchTerm(_title);
    splitter->show();
    splitter->move(window()->x()
                       + (window()->width() - splitter->size().width()) / 2,
                   window()->y()
                       + (window()->height() - splitter->size().height()) / 2);
}

// Given some examples, returns a set of five (or fewer) examples from
// each source that exists in the example.
std::unordered_map<std::string, std::vector<Example>>
EntryViewExampleCardSection::getSamplesForEachSource(
    const std::vector<Example> &examples) const
{
    std::unordered_map<std::string, std::vector<Example>> samples;

    for (const auto &e : examples) {
        for (const auto &translationset : e.getTranslationSets()) {
            std::string source = translationset.getSource();

            if (samples[source].size() >= 2) {
                continue;
            }

            Example example = Example(e.getSourceLanguage(),
                                      e.getSimplified(),
                                      e.getTraditional(),
                                      e.getJyutping(),
                                      e.getPinyin(),
                                      std::vector<TranslationSet>{
                                          translationset});

            samples[source].push_back(example);
        }
    }
    return samples;
}
