#include "sentenceresultlistmodel.h"

#include "logic/search/sqlsearch.h"

#include <QModelIndex>
#include <QVariant>

SentenceResultListModel::SentenceResultListModel(
    std::shared_ptr<SQLSearch> sqlSearch,
    std::vector<SourceSentence> sentences,
    QObject *parent)
    : QAbstractListModel(parent)
    , _sentences{sentences}
    , _search{sqlSearch}
{
    _search->registerObserver(this);
}

void SentenceResultListModel::callback(
    [[maybe_unused]] const std::vector<Entry> &entries,
    [[maybe_unused]] bool emptyQuery)
{
}

void SentenceResultListModel::callback(
    const std::vector<SourceSentence> &sentences,
    [[maybe_unused]] bool emptyQuery)
{
    setSentences(sentences);
}

void SentenceResultListModel::setSentences(
    std::span<const SourceSentence> sentences)
{
    beginResetModel();
    _sentences.assign(sentences.begin(), sentences.end());
    endResetModel();
}

int SentenceResultListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return static_cast<int>(_sentences.size());
    }

    if (static_cast<unsigned long>(parent.row()) >= _sentences.size()) {
        return static_cast<int>(_sentences.size());
    }

    return static_cast<int>(_sentences.size() - 1
                            - static_cast<unsigned long>(parent.row()));
}

QVariant SentenceResultListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (static_cast<unsigned long>(index.row()) >= _sentences.size()) {
        return {};
    }

    if (role == Qt::DisplayRole) {
        QVariant var;
        var.setValue(_sentences.at(static_cast<unsigned long>(index.row())));
        return var;
    } else {
        return {};
    }
}

QVariant SentenceResultListModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    return {};
}
