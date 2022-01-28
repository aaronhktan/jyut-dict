#ifndef SENTENCERESULTLISTMODEL_H
#define SENTENCERESULTLISTMODEL_H

#include "logic/search/isearchobservable.h"
#include "logic/search/isearchobserver.h"
#include "logic/search/sqlsearch.h"
#include "logic/sentence/sourcesentence.h"

#include <QAbstractListModel>
#include <QModelIndex>
#include <QObject>
#include <QVariant>

#include <vector>

// The SentenceResultListModel contains data (a vector of SourceSentence objects)
// It is populated with the results of a search, being a searchobserver

// SourceSentences are returned as QVariants when an index is provided
// Header data override is "good manners", but currently is not useful

class SentenceResultListModel : public QAbstractListModel, public ISearchObserver
{
    Q_OBJECT

public:
    explicit SentenceResultListModel(std::shared_ptr<SQLSearch> sqlSearch,
                                     std::vector<SourceSentence> sentences,
                                     QObject *parent = nullptr);

    void callback(const std::vector<Entry> &entries, bool emptyQuery) override;
    void callback(const std::vector<SourceSentence> &sentences,
                  bool emptyQuery) override;
    void setSentences(const std::vector<SourceSentence> &sentences);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<SourceSentence> _sentences;

    std::shared_ptr<ISearchObservable> _search;
};

#endif // SENTENCERESULTLISTMODEL_H
