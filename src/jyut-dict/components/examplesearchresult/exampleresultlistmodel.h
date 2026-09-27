#ifndef EXAMPLERESULTLISTMODEL_H
#define EXAMPLERESULTLISTMODEL_H

#include "logic/example/example.h"
#include "logic/search/isearchobservable.h"
#include "logic/search/isearchobserver.h"

#include <QAbstractListModel>

#include <span>
#include <vector>

class SQLSearch;

class QModelIndex;
class QVariant;

// The ExampleResultListModel contains data (a vector of Example objects)
// It is populated with the results of a search, being a searchobserver

// Example are returned as QVariants when an index is provided
// Header data override is "good manners", but currently is not useful

class ExampleResultListModel : public QAbstractListModel, public ISearchObserver
{
    Q_OBJECT
public:
    explicit ExampleResultListModel(std::shared_ptr<SQLSearch> sqlSearch,
                                    std::vector<Example> examples,
                                    QObject *parent = nullptr);

    void callback(const std::vector<Entry> &entries, bool emptyQuery) override;
    void callback(const std::vector<Example> &examples,
                  bool emptyQuery) override;
    void setExamples(std::span<const Example> examples);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<Example> _examples;

    std::shared_ptr<ISearchObservable> _search;
};

#endif // EXAMPLERESULTLISTMODEL_H
