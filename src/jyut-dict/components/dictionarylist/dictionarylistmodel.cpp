#include "dictionarylistmodel.h"

DictionaryListModel::DictionaryListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    _dictionaries = {};
}

void DictionaryListModel::setDictionaries(
    std::vector<DictionaryMetadata> dictionaries)
{
    beginResetModel();
    _dictionaries = dictionaries;
    endResetModel();
}

// Currently, setData ignores the index.
// TODO: the data should set the data at index.row() instead of just pushing
// to the back of the dictionaries vector.
bool DictionaryListModel::setData(const QModelIndex &index,
                                  const QVariant &value,
                                  int role)
{
    if (role != Qt::EditRole) {
        return false;
    }

    try {
        _dictionaries.push_back(value.value<DictionaryMetadata>());
    } catch (std::exception &/*e*/) {
        //        qDebug() << e.what();
        return false;
    }

    dataChanged(index, index);
    return true;
}

bool DictionaryListModel::removeRows(int row,
                                     int count,
                                     const QModelIndex &parent)
{
    beginResetModel();
    auto start = _dictionaries.begin() + row
                 + (parent.isValid() ? parent.row() : 0);
    auto end = start + count;
    _dictionaries.erase(start, end);
    endResetModel();
    return true;
}

QModelIndex DictionaryListModel::index(int row,
                                       int column,
                                       const QModelIndex &parent) const
{
    return createIndex(parent.isValid() ? parent.row() + row : row,
                       column);
}

int DictionaryListModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid()) {
        return static_cast<int>(_dictionaries.size());
    }

    if (static_cast<unsigned long>(parent.row()) >= _dictionaries.size()) {
        return static_cast<int>(_dictionaries.size());
    }

    return static_cast<int>(_dictionaries.size()
                            - static_cast<unsigned long>(parent.row()));
}

QVariant DictionaryListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (static_cast<unsigned long>(index.row()) >= _dictionaries.size()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        QVariant var = QVariant::fromValue(_dictionaries.at(static_cast<unsigned long>(index.row())));
        return var;
    } else {
        return QVariant();
    }
}

QVariant DictionaryListModel::headerData(int section, Qt::Orientation orientation,
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
