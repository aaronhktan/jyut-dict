#include "sentenceresultlistmodel.h"

SentenceResultListModel::SentenceResultListModel(std::shared_ptr<SQLSearch> sqlSearch,
                                 std::vector<SourceSentence> sentences, QObject *parent)
    : QAbstractListModel(parent)
    , _sentences{sentences}
{
    _search = sqlSearch;
    _search->registerObserver(this);
}

SentenceResultListModel::~SentenceResultListModel()
{
}

void SentenceResultListModel::callback(const std::vector<Entry> entries, bool emptyQuery)
{
    (void) (entries);
    (void) (emptyQuery);
}

void SentenceResultListModel::callback(const std::vector<SourceSentence> sentences,
                               bool emptyQuery)
{
    (void) (emptyQuery);
    setSentences(sentences);
}

void SentenceResultListModel::setSentences(std::vector<SourceSentence> sentences)
{
    beginResetModel();
    _sentences = sentences;
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
        return QVariant();
    }

    if (static_cast<unsigned long>(index.row()) >= _sentences.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QVariant var;
        var.setValue(_sentences.at(static_cast<unsigned long>(index.row())));
        return var;
    } else {
        return QVariant();
    }
}

QVariant SentenceResultListModel::headerData(int section, Qt::Orientation orientation,
                                    int role) const
{
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return QString("Row %1").arg(section);
    } else {
        return QVariant();
    }
}
