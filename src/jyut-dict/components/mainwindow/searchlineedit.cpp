#include "searchlineedit.h"

#include "logic/settings/settings.h"
#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QApplication>
#include <QKeyEvent>
#include <QIcon>
#include <QTimer>

#include <vector>

SearchLineEdit::SearchLineEdit(
    std::shared_ptr<ISearchOptionsMediator> mediator,
    std::shared_ptr<ISearch> sqlSearch,
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
    QWidget *parent)
    : QLineEdit(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _settings = Settings::getSettings(this);
    _mediator = mediator;
    _search = sqlSearch;
    _timer = new QTimer{this};

    setupUI();
    translateUI();
    setStyle(Utils::isDarkMode());

    setMinimumWidth(parent->width() / 4);
}

void SearchLineEdit::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::PaletteChange && !_paletteRecentlyChanged) {
        // QWidget emits a palette changed event when setting the stylesheet
        // So prevent it from going into an infinite loop with this timer
        _paletteRecentlyChanged = true;
        QTimer::singleShot(10, this, [=, this]() { _paletteRecentlyChanged = false; });

        // Set the style to match whether the user started dark mode
        setStyle(Utils::isDarkMode());
    }
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
    QLineEdit::changeEvent(event);
}

// When in focus and text present, the clear button should be visible
void SearchLineEdit::focusInEvent(QFocusEvent *event)
{
    checkClearVisibility();
    QLineEdit::focusInEvent(event);
}

// When out of focus, the clear button should never be visible
void SearchLineEdit::focusOutEvent(QFocusEvent *event)
{
    removeAction(_clearLineEdit);
    QLineEdit::focusOutEvent(event);
}

// Since the textChanged event happens before letters are painted,
// it feels slow.
// Thus, only use the signals and slots mechanism on Windows, and only
// when inputting Chinese.
void SearchLineEdit::updateParameters(SearchParameters parameters)
{
    _parameters = parameters;
}

void SearchLineEdit::search(void)
{
    switch (_parameters) {
    case SearchParameters::SIMPLIFIED: {
        _search->searchSimplified(text().trimmed());
        break;
    }
    case SearchParameters::TRADITIONAL: {
        _search->searchTraditional(text().trimmed());
        break;
    }
    case SearchParameters::PINYIN: {
        _search->searchPinyin(text().trimmed());
        break;
    }
    case SearchParameters::JYUTPING: {
        _search->searchJyutping(text().trimmed());
        break;
    }
    case SearchParameters::ENGLISH: {
        _search->searchEnglish(text().trimmed());
        break;
    }
    case SearchParameters::AUTO_DETECT: {
        _search->searchAutoDetect(text().trimmed());
        break;
    }
    default: {
        break;
    }
    }

    addSearchTermToHistory(_parameters);
}

void SearchLineEdit::setupUI(void)
{
    _searchLineEdit = new QAction{"", this};
    addAction(_searchLineEdit, QLineEdit::LeadingPosition);

    _clearLineEdit = new QAction{"", this};
    connect(_clearLineEdit, &QAction::triggered, this, &QLineEdit::clear);

    _handwriting = new QAction{"", this};
    addAction(_handwriting, QLineEdit::TrailingPosition);
    connect(_handwriting,
            &QAction::triggered,
            this,
            &SearchLineEdit::startHandwriting);

#ifndef Q_OS_LINUX
    _microphone = new QAction{"", this};
    addAction(_microphone, QLineEdit::TrailingPosition);
    connect(_microphone,
            &QAction::triggered,
            this,
            &SearchLineEdit::startTranscription);
#endif

    // Customize the look of the searchbar to fit in better with platform styles
#ifdef Q_OS_WIN
    setMinimumHeight(30);
#endif

    connect(this,
            &QLineEdit::textChanged,
            this,
            &SearchLineEdit::searchTriggered);
}

void SearchLineEdit::translateUI(void)
{
    setPlaceholderText(tr("Search"));
}

void SearchLineEdit::setStyle(bool use_dark)
{
#ifdef Q_OS_WIN
    QFont font = QFont{"Microsoft YaHei"};
    font.setStyleHint(QFont::System, QFont::PreferAntialias);
    setFont(font);
#endif

    QIcon search = QIcon{":/images/search.png"};
    QIcon clear = QIcon{":/images/x_action.png"};
    QIcon handwriting = QIcon{":/images/handwriting_action.png"};
    QIcon mic = QIcon{":/images/mic_action.png"};
    QIcon searchInverted = QIcon{":/images/search_inverted.png"};
    QIcon clearInverted = QIcon{":/images/x_action_inverted.png"};
    QIcon micInverted = QIcon{":/images/mic_action_inverted.png"};
    QIcon handwritingInverted = QIcon{
        ":/images/handwriting_action_inverted.png"};

    int interfaceSize = static_cast<int>(
        _settings
            ->value("Interface/size",
                    QVariant::fromValue(Settings::InterfaceSize::NORMAL))
            .value<Settings::InterfaceSize>());

    int h6FontSize = Settings::h6FontSize.at(
        static_cast<unsigned long>(interfaceSize - 1));

    if (use_dark) {
        setStyleSheet(QString{"QLineEdit { "
                              "   background-color: #586365; "
#ifdef Q_OS_WIN
                              "   border: 1px solid black; "
#endif
                              "   border-radius: 3px; "
                              "   font-size: %1px; "
                              "   icon-size: %1px; "
                              "   padding-top: 4px; "
                              "   padding-bottom: 4px; "
#ifdef Q_OS_WIN
                              "   placeholder-text-color: #999999; "
#endif
                              "} "
                              ""
                              "QLineEdit:focus { "
                              "   border-radius: 2px; "
                              "} "}
                          .arg(std::to_string(h6FontSize).c_str()));
        _searchLineEdit->setIcon(searchInverted);
        _clearLineEdit->setIcon(clearInverted);
        _handwriting->setIcon(handwritingInverted);
#ifndef Q_OS_LINUX
            _microphone->setIcon(micInverted);
#endif
    } else {
            setStyleSheet(QString{"QLineEdit { "
                                  "   background-color: #ffffff; "
#ifdef Q_OS_WIN
                                  "   border: 1px solid lightgrey; "
#endif
                                  "   border-radius: 3px; "
                                  "   font-size: %1px; "
                                  "   icon-size: %1px; "
                                  "   padding-top: 4px; "
                                  "   padding-bottom: 4px; "
                                  "} "
                                  ""
                                  "QLineEdit:focus { "
#ifdef Q_OS_WIN
                                  "   border: 1px solid lightgrey; "
#endif
                                  "   border-radius: 2px; "
                                  "} "}
                              .arg(std::to_string(h6FontSize).c_str()));
            _searchLineEdit->setIcon(search);
            _clearLineEdit->setIcon(clear);
            _handwriting->setIcon(handwriting);
#ifndef Q_OS_LINUX
            _microphone->setIcon(mic);
#endif
    }
}

void SearchLineEdit::checkClearVisibility(void)
{
    if (text().isEmpty()
        || !hasFocus()) { // Don't add the clear line edit action if the widget doesn't have focus!
        // Clicking on the clear line edit action causes a crash.
        removeAction(_clearLineEdit);
    } else {
        addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    }
}

void SearchLineEdit::startHandwriting(void)
{
    checkClearVisibility();

    _handwritingWindow = new HandwritingWindow{this};
    _handwritingWindow->setAttribute(Qt::WA_DeleteOnClose);
    _handwritingWindow->setFocus();
    _handwritingWindow->show();

    connect(_handwritingWindow,
            &HandwritingWindow::characterChosen,
            this,
            [&](QString character) {
                if (character == "\x8") {
                    if (!text().isEmpty()) {
                        setText(text().chopped(1));
                    }
                } else {
                    setText(text() + character);
                }
            });
    connect(_handwritingWindow,
            &HandwritingWindow::scriptSelected,
            this,
            [&](Handwriting::Script script) {
                SearchParameters params;
                switch (script) {
                case Handwriting::Script::SIMPLIFIED: {
                    params = SearchParameters::SIMPLIFIED;
                    break;
                }
                case Handwriting::Script::TRADITIONAL: {
                    params = SearchParameters::TRADITIONAL;
                    break;
                }
                }
                _mediator->setParameters(params);
            });
}

#ifndef Q_OS_LINUX
void SearchLineEdit::startTranscription(void)
{
    clear();
    checkClearVisibility();

    _transcriptionWindow = new TranscriptionWindow{this};
    _transcriptionWindow->setAttribute(Qt::WA_DeleteOnClose);
    _transcriptionWindow->setFocus();
    _transcriptionWindow->show();

    connect(_transcriptionWindow,
            &TranscriptionWindow::transcription,
            this,
            [&](QString result) { setText(result); });
    connect(_transcriptionWindow,
            &TranscriptionWindow::languageSelected,
            this,
            [&](TranscriptionLanguage lang) {
                SearchParameters params;
                switch (lang) {
                case TranscriptionLanguage::CANTONESE: {
                    params = SearchParameters::TRADITIONAL;
                    break;
                }
                case TranscriptionLanguage::MANDARIN: {
                    if (Settings::getCurrentLocale().territory()
                        == QLocale::Taiwan) {
                        params = SearchParameters::TRADITIONAL;
                    } else {
                        params = SearchParameters::SIMPLIFIED;
                    }
                    break;
                }
                case TranscriptionLanguage::ENGLISH: {
                    params = SearchParameters::ENGLISH;
                    break;
                }
                }
                _mediator->setParameters(params);
            });
}
#endif

void SearchLineEdit::addSearchTermToHistory(SearchParameters parameters) const
{
    _timer->stop();
    disconnect(_timer, nullptr, nullptr, nullptr);
    _timer->setSingleShot(true);
    connect(_timer, &QTimer::timeout, this, [=, this]() {
        if (!text().isEmpty()) {
            _sqlHistoryUtils->addSearchToHistory(text().toStdString(),
                                                 static_cast<int>(parameters));
        }
    });
    _timer->start(500);
}

void SearchLineEdit::searchTriggered(void)
{
    checkClearVisibility();
    if (_settings->value("Search/autoDetectLanguage", QVariant{true}).toBool()) {
        _search->searchAutoDetect(text().trimmed());
        addSearchTermToHistory(SearchParameters::AUTO_DETECT);
    } else {
        search();
    }
}

#ifndef Q_OS_LINUX
void SearchLineEdit::dictationRequested(void)
{
    startTranscription();
}
#endif

void SearchLineEdit::handwritingRequested(void)
{
    startHandwriting();
}
