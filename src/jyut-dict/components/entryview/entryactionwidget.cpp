#include "entryactionwidget.h"

EntryActionWidget::EntryActionWidget(std::shared_ptr<SQLDatabaseManager> manager,
                                     QWidget *parent)
    : QWidget(parent)
    , _manager{manager}
{
    _utils = std::make_unique<SQLUserDataUtils>(_manager);
    _utils->registerObserver(this);
    _utils->searchForAllFavouritedWords();
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
    qDebug() << "entryExists is " << entryExists;
}

void EntryActionWidget::setEntry(Entry entry)
{
    _utils->checkIfEntryHasBeenFavourited(entry);
}
