#include "sentencesplitter.h"

#include "components/sentencesearchresult/sentenceresultlistmodel.h"
#include "components/sentencesearchresult/sentenceresultlistview.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_WIN
#include "logic/utils/utils_windows.h"
#endif

#include <QList>
#include <QVariant>

SentenceSplitter::SentenceSplitter(std::shared_ptr<SQLDatabaseManager> manager,
                                   QWidget *parent)
    : QSplitter(parent)
    , _manager{manager}
{
    setMinimumSize(QSize{700, 450});

    _sqlSearch = std::make_shared<SQLSearch>(manager);
    _model = new SentenceResultListModel{_sqlSearch, {}, this};

    _sentenceScrollArea = new SentenceScrollArea{this};
    _resultListView = new SentenceResultListView{this};
    _resultListView->setModel(_model);

    addWidget(_resultListView);
    addWidget(_sentenceScrollArea);

    // Don't use QListView::click, since it doesn't respond to changes
    // in the current index if user is navigating with the keyboard
    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &SentenceSplitter::handleClick);

    connect(_resultListView,
            &QListView::doubleClicked,
            this,
            &SentenceSplitter::handleDoubleClick);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#ifdef Q_OS_MAC
    setStyleSheet("QSplitter::handle { "
                  "   background-color: none; "
                  "} ");
#else
    setStyleSheet("QSplitter::handle { "
                  "   background-color: palette(alternate-base); "
                  "} ");
#endif
#ifdef Q_OS_WIN
    setStyle(Utils::isDarkMode());
#endif
}

void SentenceSplitter::changeEvent(QEvent *event)
{
#ifdef Q_OS_WIN
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
#endif
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QSplitter::changeEvent(event);
}

void SentenceSplitter::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        close();
    }
}

#ifdef Q_OS_WIN
void SentenceSplitter::setStyle(bool use_dark)
{
    (void) (use_dark);
    setStyleSheet("QSplitter { border-top: 1px solid palette(alternate-base); }");
}
#endif

void SentenceSplitter::setSourceSentences(
    std::span<const SourceSentence> sourceSentences)
{
    static_cast<SentenceResultListModel *>(_model)->setSentences(sourceSentences);
    _size = static_cast<int>(sourceSentences.size());
}

void SentenceSplitter::setSearchTerm(const QString &searchTerm)
{
    _searchTerm = searchTerm;
    translateUI();
}

void SentenceSplitter::translateUI(void)
{
    QString title;
    if (_size == 1) {
        title = QString{tr("Sentences for %1 (%2 result)")}.arg(_searchTerm,
                                                                QString::number(
                                                                    _size));
    } else {
        title = QString{tr("Sentences for %1 (%2 results)")}
                    .arg(_searchTerm, QString::number(_size));
    }
    setWindowTitle(title);
}

void SentenceSplitter::prepareSourceSentence(SourceSentence &sentence) const
{
    // Although the setting is named Entry/xxxPronunciationOptions, it applies
    // to all detail views (i.e. both the entry detail view and sentence detail
    // view.)
    CantoneseOptions cantoneseOptions
        = Settings::getSettings()
              ->value("Entry/cantonesePronunciationOptions",
                      QVariant::fromValue(CantoneseOptions::RAW_JYUTPING))
              .value<CantoneseOptions>();
    MandarinOptions mandarinOptions
        = Settings::getSettings()
              ->value("Entry/mandarinPronunciationOptions",
                      QVariant::fromValue(MandarinOptions::PRETTY_PINYIN))
              .value<MandarinOptions>();
    sentence.generatePhonetic(cantoneseOptions, mandarinOptions);
}

void SentenceSplitter::openCurrentSelectionInNewWindow(void)
{
    QModelIndex entryIndex = _resultListView->currentIndex();
    handleDoubleClick(entryIndex);
}

void SentenceSplitter::handleClick(const QModelIndex &selection)
{
    SourceSentence sentence = qvariant_cast<SourceSentence>(selection.data());

    prepareSourceSentence(sentence);

    _sentenceScrollArea->setSourceSentence(sentence);
}

void SentenceSplitter::handleDoubleClick(const QModelIndex &selection)
{
    SourceSentence sentence = qvariant_cast<SourceSentence>(selection.data());

    prepareSourceSentence(sentence);

    SentenceScrollArea *area = new SentenceScrollArea{nullptr};
    area->setParent(this, Qt::Window);
    area->setSourceSentence(sentence);
#ifndef Q_OS_MAC
    area->setWindowTitle(" ");
#endif
    area->show();
}

void SentenceSplitter::updateStyleRequested(void)
{
    static_cast<SentenceResultListView *>(_resultListView)
        ->paintWithApplicationState();
    _sentenceScrollArea->updateStyleRequested();
}
