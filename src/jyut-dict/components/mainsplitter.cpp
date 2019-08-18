#include "mainsplitter.h"

#include "components/resultlistview.h"
#include "logic/entry/entry.h"
#include "logic/settings/settingsutils.h"

#include <QList>

MainSplitter::MainSplitter(QWidget *parent) : QSplitter(parent)
{
    _definitionScrollArea = new DefinitionScrollArea{this};
    _resultListView = new ResultListView{this};

    addWidget(_resultListView);
    addWidget(_definitionScrollArea);

    connect(_resultListView->selectionModel(),
            &QItemSelectionModel::currentChanged,
            this,
            &MainSplitter::handleSelectionChanged);

    setHandleWidth(1);
    setCollapsible(0, false);
    setCollapsible(1, false);
    setSizes(QList<int>({size().width() / 3, size().width() * 2 / 3}));
#ifdef Q_OS_WIN
    setStyleSheet("QSplitter::handle { background-color: #b9b9b9; }");
#elif defined(Q_OS_DARWIN)
    setStyleSheet("QSplitter::handle { background-color: none; }");
#else
    setStyleSheet("QSplitter::handle { background-color: lightgray; }");
#endif
}

MainSplitter::~MainSplitter()
{

}

void MainSplitter::handleSelectionChanged(const QModelIndex &selection)
{
    Entry entry = qvariant_cast<Entry>(selection.data());
    if (entry.getSimplified() == "Welcome!") {
        return;
    }

    entry.refreshColours(
        Settings::getSettings()
            ->value("entryColourPhoneticType",
                    QVariant::fromValue(EntryColourPhoneticType::JYUTPING))
            .value<EntryColourPhoneticType>());
    _definitionScrollArea->setEntry(entry);
}
