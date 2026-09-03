#include "exampleresultlistmodel.h"

#include "logic/search/sqlsearch.h"

#include <QModelIndex>
#include <QVariant>

ExampleResultListModel::ExampleResultListModel(
    std::shared_ptr<SQLSearch> sqlSearch,
    std::vector<Example> examples,
    QObject *parent)
    : QAbstractListModel(parent)
    , _examples{examples}
    , _search{sqlSearch}
{
    _search->registerObserver(this);
}

void ExampleResultListModel::callback(
    [[maybe_unused]] const std::vector<Entry> &entries,
    [[maybe_unused]] bool emptyQuery)
{
}

void ExampleResultListModel::callback(const std::vector<Example> &examples,
                                      [[maybe_unused]] bool emptyQuery)
{
    setExamples(examples);
}

void ExampleResultListModel::setExamples(std::span<const Example> examples)
{
    beginResetModel();
    _examples.assign(examples.begin(), examples.end());
    endResetModel();
}

int ExampleResultListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return static_cast<int>(_examples.size());
    }

    if (static_cast<unsigned long>(parent.row()) >= _examples.size()) {
        return static_cast<int>(_examples.size());
    }

    return static_cast<int>(_examples.size() - 1
                            - static_cast<unsigned long>(parent.row()));
}

QVariant ExampleResultListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (static_cast<unsigned long>(index.row()) >= _examples.size()) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        QVariant var;
        var.setValue(_examples.at(static_cast<unsigned long>(index.row())));
        return var;
    } else {
        return {};
    }
}

QVariant ExampleResultListModel::headerData(int section,
                                            Qt::Orientation orientation,
                                            int role) const
{
    return {};
}
