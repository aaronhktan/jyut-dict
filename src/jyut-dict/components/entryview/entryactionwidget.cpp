#include "entryactionwidget.h"

EntryActionWidget::EntryActionWidget(std::shared_ptr<SQLDatabaseManager> manager,
                                     QWidget *parent)
    : QWidget(parent)
    , _manager{manager}
{
    _utils = std::make_unique<SQLUserDataUtils>(_manager);
    _utils->registerObserver(this);
    _utils->searchForAllFavouritedWords();

    setupUI();
}

#include <QDebug>
void EntryActionWidget::callback(const std::vector<Entry> entries,
                                 bool emptyQuery)
{
    (void) (emptyQuery);
    for (auto &entry : entries) {
        qDebug() << entry.getDefinitionSnippet().c_str();
    }
}

void EntryActionWidget::callback(bool entryExists)
{
    qDebug() << entryExists;
    _bookmarkButton->setText(entryExists ? "yes" : "no");
}

void EntryActionWidget::setEntry(Entry entry)
{
    _bookmarkButton->setVisible(true);
    _utils->checkIfEntryHasBeenFavourited(entry);
    _entry = entry;
}

void EntryActionWidget::setupUI(void)
{
    _bookmarkButton = new QToolButton{this};
    _bookmarkButton->setVisible(false);
    _bookmarkButton->setText("Hi");
    _bookmarkButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    _layout = new QHBoxLayout{this};
    _layout->addWidget(_bookmarkButton);
}

void EntryActionWidget::addEntryToFavourites(Entry entry)
{
    (void) (entry);
}
