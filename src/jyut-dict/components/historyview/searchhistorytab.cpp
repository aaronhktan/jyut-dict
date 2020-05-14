#include "searchhistorytab.h"

#include "logic/settings/settingsutils.h"

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
    _tabLayout->setContentsMargins(0, 0, 0, 0);

    setStyle(/*use_dark=*/false);
}

void SearchHistoryTab::translateUI(void)
{
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    for (auto button : buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _clearAllSearchHistoryButton->setText(tr("Clear All Search History"));
    _clearAllSearchHistoryButton->setFixedSize(
        _clearAllSearchHistoryButton->minimumSizeHint());
}

void SearchHistoryTab::setStyle(bool use_dark)
{
    (void) (use_dark);
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: "
                  "13px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet(
        "QPushButton[isHan=\"true\"] { font-size: 12px; height: 20px; }");
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
