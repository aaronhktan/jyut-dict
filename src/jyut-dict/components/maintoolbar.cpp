#include "maintoolbar.h"

#include "logic/search/searchoptionsmediator.h"

MainToolBar::MainToolBar(std::shared_ptr<SQLDatabaseManager> manager,
                         QWidget *parent) : QToolBar(parent)
{
    _toolBarWidget = new QWidget;
    _toolBarLayout = new QHBoxLayout;

    _searchOptions = new SearchOptionsMediator{};

    _searchBar = new SearchLineEdit(_searchOptions, manager, this);
    _searchOptions->registerLineEdit(_searchBar);
    _optionsBox = new SearchOptionsRadioGroupBox(_searchOptions, this);

    _toolBarLayout->addWidget(_searchBar);
    _toolBarLayout->addWidget(_optionsBox);
#ifdef Q_OS_WIN
    _toolBarLayout->setContentsMargins(6, 2, 6, 9);
#endif
//    _toolBarLayout->addStretch();

    _toolBarWidget->setLayout(_toolBarLayout);

    addWidget(_toolBarWidget);
    setMovable(false);
    setFloatable(false);
    setFocusPolicy(Qt::ClickFocus);

#ifdef Q_OS_WIN
    setStyleSheet("QToolBar { background-color: white; }");
#elif defined(Q_OS_LINUX)
    setStyleSheet("QToolBar { border-bottom: 1px solid lightgray; }");
#endif
}

MainToolBar::~MainToolBar()
{
    delete _searchOptions;
}

void MainToolBar::focusInEvent(QFocusEvent *event)
{
    QToolBar::focusInEvent(event);
    _searchBar->setFocus();
}
