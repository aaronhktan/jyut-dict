#include "searchhistorytab.h"

#include "logic/settings/settingsutils.h"
#ifdef Q_OS_MAC
#include "logic/utils/utils_mac.h"
#elif defined (Q_OS_LINUX)
#include "logic/utils/utils_linux.h"
#elif defined(Q_OS_WIN)
#include "logic/utils/utils_windows.h"
#endif
#include "logic/utils/utils_qt.h"

#include <QTimer>

SearchHistoryTab::SearchHistoryTab(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _listView = new SearchHistoryListView{this};
    _model = new SearchHistoryListModel{_sqlHistoryUtils, this};
    _listView->setModel(_model);

    connect(_listView,
            &QListView::clicked,
            this,
            &SearchHistoryTab::handleClick);

    setupUI();
    translateUI();

    _sqlHistoryUtils->searchAllSearchHistory();
}

void SearchHistoryTab::changeEvent(QEvent *event)
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
}

void SearchHistoryTab::setupUI(void)
{
    _clearAllSearchHistoryButton = new QPushButton{this};
    connect(_clearAllSearchHistoryButton, &QPushButton::clicked, this, [=]() {
        _sqlHistoryUtils->clearAllSearchHistory();
    });

    _tabLayout = new QVBoxLayout{this};
    _tabLayout->addWidget(_listView);
    _tabLayout->addWidget(_clearAllSearchHistoryButton);
    _tabLayout->setAlignment(_clearAllSearchHistoryButton, Qt::AlignHCenter);

    _tabLayout->setSpacing(10);

#ifdef Q_OS_WIN
    setAttribute(Qt::WA_StyledBackground, true);
#endif
    setStyle(Utils::isDarkMode());
}

void SearchHistoryTab::translateUI(void)
{
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    foreach (const auto &button, buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _clearAllSearchHistoryButton->setText(tr("Clear All Search History"));
    _clearAllSearchHistoryButton->setFixedSize(
        _clearAllSearchHistoryButton->minimumSizeHint());

    _model->translateUI();
}

void SearchHistoryTab::setStyle(bool use_dark)
{
    _clearAllSearchHistoryButton->setFixedSize(
        _clearAllSearchHistoryButton->minimumSizeHint());
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: "
                  "13px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { "
                  "   border: 1px solid palette(base); "
                  "   font-size: 12px; "
                  "   height: 20px; "
                  "} ");
#endif

#if defined(Q_OS_MAC)
    setStyleSheet(use_dark ? "QListView { border: none; }"
                           : "QListView { border: 1px solid lightgrey; }");
#elif defined(Q_OS_LINUX)
    _listView->setStyleSheet("QListView { border: 1px solid palette(alternate-base); }");
#elif defined(Q_OS_WIN)
    setStyleSheet(use_dark ? "QListView { border: none; }"
                           : "QListView { border: 1px solid palette(window); }");
#endif

#ifdef Q_OS_MAC
    _tabLayout->setContentsMargins(0, 0, 0, use_dark ? 0 : 10);
#elif defined(Q_OS_WIN)
    setObjectName("searchHistoryTabWidget");
    setStyleSheet("QWidget#searchHistoryTabWidget { "
                  "   background-color: palette(alternate-base); "
                  "} ");
#endif
}

void SearchHistoryTab::handleClick(const QModelIndex &selection)
{
    searchTermHistoryItem pair = qvariant_cast<searchTermHistoryItem>(
        selection.data());
    bool isEmptyPair = pair.second == -1;
    if (isEmptyPair) {
        return;
    }

    emit searchHistoryClicked(pair);
}
