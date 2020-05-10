#include "maintoolbar.h"

#include "logic/search/searchoptionsmediator.h"

MainToolBar::MainToolBar(std::shared_ptr<SQLSearch> sqlSearch,
                         QWidget *parent) : QToolBar(parent)
{
    _toolBarWidget = new QWidget;
    _toolBarLayout = new QHBoxLayout;
    _toolBarLayout->setContentsMargins(11, 11, 11, 11);
    _toolBarLayout->setSpacing(0);

    _searchOptions = new SearchOptionsMediator{};

    _searchBar = new SearchLineEdit(_searchOptions, sqlSearch, this);
    _searchOptions->registerLineEdit(_searchBar);
    _optionsBox = new SearchOptionsRadioGroupBox(_searchOptions, this);

    _openButton = new QToolButton{this};
    _openButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _openButton->setIcon(QIcon{":/images/settings_inverted_nopadding.png"});
    _openButton->setIconSize(QSize{20, 20});
    _openButton->setCursor(Qt::PointingHandCursor);
    _openButton->setStyleSheet("QToolButton { padding: 1px; margin: 0px; }"
                               "QToolButton:hover { background-color: grey; border-radius: 3px; padding: 1px; margin: 0px; }");

    _openFavouritesButton = new QToolButton{this};
    _openFavouritesButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    _openFavouritesButton->setIcon(
        QIcon{":/images/star_inverted_nopadding.png"});
    _openFavouritesButton->setIconSize(QSize{19, 20});
    _openFavouritesButton->setCursor(Qt::PointingHandCursor);
    _openFavouritesButton->setStyleSheet("QToolButton { padding: 1px; margin-right: 4px; }"
                                         "QToolButton:hover { background-color: grey; border-radius: 3px; padding: 1px; margin-right: 4px; }");

    _toolBarLayout->addWidget(_searchBar);
    _toolBarLayout->addWidget(_optionsBox);
    _toolBarLayout->addWidget(_openFavouritesButton);
    _toolBarLayout->addWidget(_openButton);
#ifdef Q_OS_WIN
    _toolBarLayout->setContentsMargins(6, 2, 6, 9);
#endif

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

void MainToolBar::selectAllEvent(void)
{
    _searchBar->setFocus();
    _searchBar->selectAll();
}

void MainToolBar::changeOptionEvent(const Utils::ButtonOptionIndex index)
{
    _optionsBox->setOption(index);
}

void MainToolBar::setOpenSettingsAction(QAction *action)
{
    connect(_openButton, &QToolButton::pressed, this, [=]() {
        action->trigger();
    });
}

void MainToolBar::setOpenFavouritesAction(QAction *action)
{
    connect(_openFavouritesButton, &QToolButton::pressed, this, [=]() {
        action->trigger();
    });
}
