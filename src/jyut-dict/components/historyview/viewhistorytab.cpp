#include "viewhistorytab.h"

#include "logic/settings/settingsutils.h"

ViewHistoryTab::ViewHistoryTab(
    std::shared_ptr<SQLUserHistoryUtils> sqlHistoryUtils, QWidget *parent)
    : QWidget(parent)
    , _sqlHistoryUtils{sqlHistoryUtils}
{
    _listView = new ViewHistoryListView{this};
    _model = new ViewHistoryListModel{_sqlHistoryUtils, this};
    _listView->setModel(_model);

    connect(_listView,
            &QListView::clicked,
            this,
            &ViewHistoryTab::handleClick);

    setupUI();
    translateUI();

    _sqlHistoryUtils->searchAllViewHistory();
}

void ViewHistoryTab::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        translateUI();
    }
}

void ViewHistoryTab::setupUI(void)
{
    _clearAllViewHistoryButton = new QPushButton{this};
    connect(_clearAllViewHistoryButton, &QPushButton::clicked, this, [=]() {
        _sqlHistoryUtils->clearAllViewHistory();
    });

    _tabLayout = new QVBoxLayout{this};
    _tabLayout->addWidget(_listView);
    _tabLayout->addWidget(_clearAllViewHistoryButton);
    _tabLayout->setAlignment(_clearAllViewHistoryButton, Qt::AlignHCenter);

    _tabLayout->setSpacing(10);
#ifdef Q_OS_MAC
    _tabLayout->setContentsMargins(0, 0, 0, 0);
#endif

    setStyle(/*use_dark=*/false);
}

void ViewHistoryTab::translateUI(void)
{
    setProperty("isHan", Settings::isCurrentLocaleHan());

    QList<QPushButton *> buttons = this->findChildren<QPushButton *>();
    for (auto button : buttons) {
        button->setProperty("isHan", Settings::isCurrentLocaleHan());
        button->style()->unpolish(button);
        button->style()->polish(button);
    }

    _clearAllViewHistoryButton->setText(tr("Clear All View History"));
    _clearAllViewHistoryButton->setFixedSize(
        _clearAllViewHistoryButton->minimumSizeHint());
}

void ViewHistoryTab::setStyle(bool use_dark)
{
    (void) (use_dark);
#ifdef Q_OS_MAC
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: "
                  "13px; height: 16px; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QPushButton[isHan=\"true\"] { font-size: 12px; height: 20px; }");
#endif

#ifdef Q_OS_LINUX
    setStyleSheet("QListView { border: 1px solid lightgrey; }");
#elif defined(Q_OS_WIN)
    setStyleSheet("QListView { border: 1px solid #b9b9b9; }");
#endif
}

void ViewHistoryTab::handleClick(const QModelIndex &selection)
{
    Entry entry = qvariant_cast<Entry>(selection.data());
    bool isWelcomeEntry = entry.isWelcome();
    bool isEmptyEntry = entry.isEmpty();
    if (isWelcomeEntry || isEmptyEntry) {
        return;
    }

    emit viewHistoryClicked(entry);
}
