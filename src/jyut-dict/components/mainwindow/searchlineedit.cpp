#include "searchlineedit.h"

#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif

#include <QIcon>
#include <QTimer>

#include <vector>

SearchLineEdit::SearchLineEdit(ISearchOptionsMediator *mediator,
                               std::shared_ptr<SQLSearch> sqlSearch,
                               std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils,
                               QWidget *parent)
    : QLineEdit(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    translateUI();

    _mediator = mediator;

    _searchLineEdit = new QAction{"", this};
    addAction(_searchLineEdit, QLineEdit::LeadingPosition);

    _clearLineEdit = new QAction{"", this};
    addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    connect(_clearLineEdit, &QAction::triggered, this,
            [this](){
                this->clear();
                removeAction(_clearLineEdit);
                _search->searchEnglish(text());});

    // Customize the look of the searchbar to fit in better with platform styles
#ifdef Q_OS_WIN
    setMinimumHeight(25);
#endif

    setStyle(Utils::isDarkMode());

    setMinimumWidth(parent->width() / 2);

    _search = sqlSearch;

    connect(this, &QLineEdit::textChanged, [this]() {
        this->checkClearVisibility();
        this->search();
    });

    _timer = new QTimer{this};
}

void SearchLineEdit::checkClearVisibility()
{
    if (text().isEmpty()) {
        removeAction(_clearLineEdit);
    } else {
        addAction(_clearLineEdit, QLineEdit::TrailingPosition);
    }
}

void SearchLineEdit::changeEvent(QEvent *event)
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
    QLineEdit::changeEvent(event);
}

// When in focus and text present, the clear button should be visible
void SearchLineEdit::focusInEvent(QFocusEvent *event)
{
    checkClearVisibility();
    QLineEdit::focusInEvent(event);
}

// When out of focus, the clear button should never be visible.
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

void SearchLineEdit::search()
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
        default: {
            break;
        }
    }

    addSearchTermToHistory();
}

void SearchLineEdit::translateUI()
{
    setPlaceholderText(tr("Search"));
}

void SearchLineEdit::setStyle(bool use_dark)
{
    QIcon search = QIcon(":/images/search.png");
    QIcon clear = QIcon(":/images/x.png");
    QIcon search_inverted = QIcon(":/images/search_inverted.png");
    QIcon clear_inverted = QIcon(":/images/x_inverted.png");

#ifdef Q_OS_WIN
    if (use_dark) {
        setStyleSheet("QLineEdit { "
                       "  background-color: #586365; "
                       "  border: 1px solid black; "
                       "  font-size: 12px; "
                       "  selection-background-color: palette(alternate-base); "
                       "} ");
        _searchLineEdit->setIcon(search_inverted);
        _clearLineEdit->setIcon(clear_inverted);
    } else {
        setStyleSheet("QLineEdit { "
                      "   background-color: #ffffff; "
                      "   border-color: black; "
                      "   border-width: 2px; "
                      "   font-size: 12px; "
                      "} ");
        _searchLineEdit->setIcon(search);
        _clearLineEdit->setIcon(clear);
    }
#else
    if (use_dark) {
        setStyleSheet("QLineEdit { "
                      "   background-color: #586365; "
                      "   border-radius: 3px; "
                      "   font-size: 12px; "
                      "   padding-top: 4px; "
                      "   padding-bottom: 4px; "
                      "} "
                      ""
                      "QLineEdit:focus { "
                      "   border-radius: 2px; "
                      "} ");
        _searchLineEdit->setIcon(search_inverted);
        _clearLineEdit->setIcon(clear_inverted);
    } else {
        setStyleSheet("QLineEdit { "
                      "   background-color: #ffffff; "
                      "   border-radius: 3px; "
                      "   border: 1px solid palette(alternate-base); "
                      "   font-size: 12px; "
                      "   padding-top: 4px; "
                      "   padding-bottom: 4px; "
                      "} ");
         _searchLineEdit->setIcon(search);
         _clearLineEdit->setIcon(clear);
    }
#endif
}

void SearchLineEdit::addSearchTermToHistory(void)
{
    _timer->stop();
    disconnect(_timer, nullptr, nullptr, nullptr);
    _timer->setSingleShot(true);
    connect(_timer, &QTimer::timeout, this, [=]() {
        if (!text().isEmpty()) {
            _sqlHistoryUtils->addSearchToHistory(text().toStdString(),
                                                 static_cast<int>(_parameters));
        }
    });
    _timer->start(500);
}
