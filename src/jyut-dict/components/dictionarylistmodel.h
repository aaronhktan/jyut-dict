#ifndef DICTIONARYLISTMODEL_H
#define DICTIONARYLISTMODEL_H

#include "logic/dictionary/dictionarymetadata.h"

#include <QAbstractListModel>

#include <vector>

// The DictionaryListModel contains a list of all currently installed
// dictionaries.

class DictionaryListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DictionaryListModel(QObject *parent = nullptr);

    void setDictionaries(std::vector<DictionaryMetadata> dictionaries);
    bool setData(const QModelIndex &index,
                 const QVariant &value,
                 int role = Qt::EditRole) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section,
                        Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;

private:
    std::vector<DictionaryMetadata> _dictionaries;

signals:

public slots:
};

#endif // DICTIONARYLISTMODEL_H
