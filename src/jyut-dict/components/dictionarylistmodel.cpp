#include "dictionarylistmodel.h"

#include <QObject>
#include <QVariant>

DictionaryListModel::DictionaryListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    _dictionaries = {};
    setDictionaries(std::vector<DictionarySource>{CEDICT, CCCANTO, CFDICT});
}

void DictionaryListModel::setDictionaries(std::vector<DictionarySource> dictionaries)
{
    beginResetModel();
    _dictionaries = dictionaries;
    endResetModel();
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
